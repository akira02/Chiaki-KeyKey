// [AUTO_HEADER]
#include "LIRPCHelper.h"

#include <string>

#include "stdafx.h"

using namespace std;

const wstring LIRUserName() {
  wstring result;
  wchar_t userName[UNLEN + 1];
  DWORD userNameLength = 0;
  if (GetUserName(userName, &userNameLength)) {
    result = wstring(userName);
  }

  return result;
}

bool LIRStartRPCServer(const wchar_t* endpointName,
                       RPC_IF_HANDLE interfaceHandle) {
  RPC_STATUS status;
  unsigned int minCalls = 1;
  unsigned int maxCalls = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
  wstring realEndpointName =
      wstring(L"ncalrpc:") + LIRUserName() + wstring(endpointName);

  status = RpcServerUseProtseqEp((RPC_WSTR)L"ncalrpc", maxCalls,
                                 (RPC_WSTR)realEndpointName.c_str(), NULL);
  if (status) return false;

  status = RpcServerRegisterIf(interfaceHandle, NULL, NULL);
  if (status) return false;

  // starts the server, don't wait (handles the call in other threads)
  status = RpcServerListen(minCalls, maxCalls, 1);
  if (status) return false;

  return true;
}

bool LIRStopRPCServer() {
  RPC_STATUS status;
  status = RpcMgmtStopServerListening(NULL);

  if (status) return false;

  // We stop all on-going calls immediately. So if the server is shutting down
  // while some clients are in the middle of a call, they'll receive exceptions
  // (in theory).
  status = RpcServerUnregisterIf(NULL, NULL, FALSE);

  if (status) return false;

  return true;
}

void __RPC_FAR* __RPC_USER MIDL_user_allocate(size_t len) {
  return (malloc(len));
}

void __RPC_USER MIDL_user_free(void __RPC_FAR* ptr) { free(ptr); }

void* LIRConnectServer(const wchar_t* endpointName, handle_t* clientHandle) {
  RPC_STATUS status;
  wstring realEndpointName =
      wstring(L"ncalrpc:") + LIRUserName() + wstring(endpointName);
  RPC_WSTR stringBindingHandle = NULL;

  status = RpcStringBindingCompose(NULL, (RPC_WSTR)L"ncalrpc", NULL,
                                   (RPC_WSTR)realEndpointName.c_str(), NULL,
                                   &stringBindingHandle);
  if (status) return 0;

  status = RpcBindingFromStringBinding(stringBindingHandle, clientHandle);
  if (status) return 0;

  return (void*)stringBindingHandle;
}

bool LIRDisconnectServer(void* stringBindingHandle, handle_t* clientHandle) {
  RPC_STATUS status;
  RPC_WSTR rpcwstr = (RPC_WSTR)stringBindingHandle;

  status = RpcStringFree(&rpcwstr);
  if (status) return false;

  status = RpcBindingFree(clientHandle);
  if (status) return false;

  return true;
}
