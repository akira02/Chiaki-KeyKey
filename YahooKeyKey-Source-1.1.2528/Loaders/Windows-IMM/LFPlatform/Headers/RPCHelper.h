//
// RPCHelper.h
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef RPCHelper_h
#define RPCHelper_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

#include <Lmcons.h>
#include <rpc.h>
#include <rpcdce.h>
#include <rpcndr.h>
#include <security.h>

#include <string>

namespace LFPlatform {
using namespace std;

class RPCServerHelper {
 public:
  // note: you don't have to add "ncalrpc" in front of your endpoint name, we'll
  // take care of that
  static bool StartService(const string& endpointName,
                           RPC_IF_HANDLE serverInterfaceHandle);
  static bool StopService();
};

class RPCClientHelper {
 public:
  typedef void* ServerHandle;

  static ServerHandle ConnectServer(const string& endpointName,
                                    handle_t* clientInterfaceHandle);
  static bool Disconnect(ServerHandle serverHandle,
                         handle_t* clientInterfaceHandle);
};
};  // namespace LFPlatform

extern "C" {
// shared memory allocation functions
void __RPC_FAR* __RPC_USER MIDL_user_allocate(size_t len);
void __RPC_USER MIDL_user_free(void __RPC_FAR* ptr);
}

#endif