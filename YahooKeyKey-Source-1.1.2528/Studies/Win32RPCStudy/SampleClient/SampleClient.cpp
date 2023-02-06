// SampleClient.cpp : Defines the entry point for the console application.
//

#include <string>

#include "LIClientRPCInterface.h"
#include "LIRPCHelper.h"
#include "LIServerRPCInterface.h"
#include "stdafx.h"

using namespace std;

void LICSendString(
    /* [string][in] */ const wchar_t* externalString) {
  wprintf(L"We received string from server: %s\n", externalString);
}

int _tmain(int argc, _TCHAR* argv[]) {
  SetConsoleCP(CP_UTF8);

  void* serverHandle;
  if (!(serverHandle =
            LIRConnectServer(L"sampleserver", &g_LIServerRPCInterfaceHandle))) {
    printf("failed to connet to server\n");
  }

  wchar_t endpointName[LICMaxStringLength];
  *endpointName = 0;

  RpcTryExcept {
    LISPing();

    LISNextAvailableClientRPCEndpointName(endpointName);
    wprintf(L"we obtained an endpoint name: %s\n", endpointName);
  }
  RpcExcept(1) {
    unsigned long ulCode = RpcExceptionCode();
    printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
  }
  RpcEndExcept;

  if (LIRStartRPCServer(endpointName, LIClientRPCInterface_v1_0_s_ifspec)) {
    printf("client's server started\n");

    while (!feof(stdin)) {
      char buf[256];

      int x, y;

      RpcTryExcept LISSetClientRPCEndpointName(endpointName);
      // fgets(buf, 255, stdin);
      fscanf(stdin, "%d %d", &x, &y);
      printf("input = %d %d\n", x, y);
      LISPing();
      LISSetPosition(x, y);
      LISEndCurrentClientRPCEndpoint();

      RpcExcept(1) {
        ULONG ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
        break;
      }
      RpcEndExcept;
    }
  } else {
    printf("client's server registration failed\n");
  }

  if (!LIRDisconnectServer(serverHandle, &g_LIServerRPCInterfaceHandle)) {
    printf("failed to disconnect from server\n");
  }

  return 0;
}
