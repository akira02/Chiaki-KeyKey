// [AUTO_HEADER]

#ifndef LIRPCHelper_h
#define LIRPCHelper_h

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <rpc.h>
#include <rpcdce.h>
#include <rpcndr.h>

// note: you don't have to add "ncalrpc" in front of your endpoint name, we'll
// take care of that

// server-side
bool LIRStartRPCServer(const wchar_t* endpointName,
                       RPC_IF_HANDLE interfaceHandle);
bool LIRStopRPCServer();

// client-side
void* LIRConnectServer(const wchar_t* endpointName, handle_t* clientHandle);
bool LIRDisconnectServer(void* stringBindingHandle, handle_t* clientHandle);

// shared memory allocation functions
void __RPC_FAR* __RPC_USER MIDL_user_allocate(size_t len);
void __RPC_USER MIDL_user_free(void __RPC_FAR* ptr);

#endif
