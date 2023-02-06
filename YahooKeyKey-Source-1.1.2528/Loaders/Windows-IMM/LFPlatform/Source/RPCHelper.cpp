//
// RPCHelper.cpp
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include "RPCHelper.h"

#include <OpenVanilla.h>
#include <wtsapi32.h>

#include "LogKit.h"

namespace LFPlatform {

using namespace std;
using namespace OpenVanilla;

const wstring RPCServiceGetUserName() {
  Logger logger;

  wstring result;
  wchar_t userName[UNLEN + 1];
  DWORD userNameLength = UNLEN;
  wmemset(userName, 0, UNLEN + 1);
  if (GetUserName(userName, &userNameLength)) {
    result = wstring(userName);
  }

  if (OVWildcard::Match(OVUTF8::FromUTF16(result), "SYSTEM")) {
    logger.debug("Attempting to use terminal service to get user name");
    DWORD sid = WTSGetActiveConsoleSessionId();
    if (sid != 0xffffffff) {
      HANDLE utoken;
      BOOL r = WTSQueryUserToken(sid, &utoken);
      if (r) {
        if (ImpersonateLoggedOnUser(utoken)) {
          if (GetUserName(userName, &userNameLength)) {
            result = wstring(userName);
          }
          RevertToSelf();
        }
        CloseHandle(utoken);
      }
    }
  }

  logger.debug("Get user name: %s", OVUTF8::FromUTF16(result).c_str());
  logger.debug("User path: %s", OVDirectoryHelper::UserHomeDirectory().c_str());

  return result;
}

bool RPCServerHelper::StartService(const string& endpointName,
                                   RPC_IF_HANDLE serverInterfaceHandle) {
  RPC_STATUS status;
  unsigned int minCalls = 1;
  unsigned int maxCalls = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
  wstring realEndpointName = wstring(L"ncalrpc:") + RPCServiceGetUserName() +
                             OVUTF16::FromUTF8(endpointName);

  status = RpcServerUseProtseqEp((RPC_WSTR)L"ncalrpc", maxCalls,
                                 (RPC_WSTR)realEndpointName.c_str(), NULL);
  if (status) {
    return false;
  }

  status = RpcServerRegisterIf(serverInterfaceHandle, NULL, NULL);
  if (status) {
    return false;
  }

  // starts the server, don't wait (handles the call in other threads)
  status = RpcServerListen(minCalls, maxCalls, 1);
  if (status) {
    if (status == RPC_S_ALREADY_LISTENING) {
      // ignores this result; the app itself already has an RPC service;
      // the service will now also listen to our interface
      return true;
    }

    return false;
  }

  return true;
}

bool RPCServerHelper::StopService() {
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

RPCClientHelper::ServerHandle RPCClientHelper::ConnectServer(
    const string& endpointName, handle_t* clientInterfaceHandle) {
  RPC_STATUS status;
  wstring realEndpointName = wstring(L"ncalrpc:") + RPCServiceGetUserName() +
                             OVUTF16::FromUTF8(endpointName);
  RPC_WSTR stringBindingHandle = NULL;

  status = RpcStringBindingCompose(NULL, (RPC_WSTR)L"ncalrpc", NULL,
                                   (RPC_WSTR)realEndpointName.c_str(), NULL,
                                   &stringBindingHandle);
  if (status) return 0;

  status =
      RpcBindingFromStringBinding(stringBindingHandle, clientInterfaceHandle);
  if (status) return 0;

  return (RPCClientHelper::ServerHandle)stringBindingHandle;
}

bool RPCClientHelper::Disconnect(RPCClientHelper::ServerHandle serverHandle,
                                 handle_t* clientInterfaceHandle) {
  RPC_STATUS status;
  RPC_WSTR rpcwstr = (RPC_WSTR)serverHandle;

  status = RpcStringFree(&rpcwstr);
  if (status) return false;

  status = RpcBindingFree(clientInterfaceHandle);
  if (status) return false;

  return true;
}

};  // namespace LFPlatform

void __RPC_FAR* __RPC_USER MIDL_user_allocate(size_t len) {
  return (malloc(len));
}

void __RPC_USER MIDL_user_free(void __RPC_FAR* ptr) { free(ptr); }
