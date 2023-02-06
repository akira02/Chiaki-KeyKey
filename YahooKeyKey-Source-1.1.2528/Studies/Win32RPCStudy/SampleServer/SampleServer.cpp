// SampleServer.cpp : Defines the entry point for the console application.
//

#include <sstream>
#include <string>
#include <vector>

#include "LIClientRPCInterface.h"
#include "LIRPCHelper.h"
#include "LIServerRPCInterface.h"
#include "stdafx.h"

using namespace std;

wstring g_clientEndpointBase = wstring(L"sampleclient-");
vector<wstring> g_clientEndpointNames;
wstring g_currentClientEndpoint;

void LISPing(void) { printf("server: ping\n"); }

void LISSetPosition(
    /* [in] */ int x,
    /* [in] */ int y) {
  printf("server: setPosition %d, %d\n", x, y);
}

void LISHandleKey(
    /* [in] */ int keyCode,
    /* [ref][out] */ boolean *hasHandled) {
  printf("server: handleKey, keyCode = %d\n", keyCode);
  *hasHandled = true;
}

void LISCompositionString(
    /* [size_is][string][out] */ wchar_t *compositionString) {
  printf("server: returning composition string\n");
  *compositionString = 0;
}

void LISNextAvailableClientRPCEndpointName(
    /* [size_is][string][out] */ wchar_t *endpointName) {
  wstringstream wss;
  wss << g_clientEndpointBase << g_clientEndpointNames.size();
  g_clientEndpointNames.push_back(wss.str());
  wcscpy_s(endpointName, LICMaxStringLength, wss.str().c_str());
}

void LISSetClientRPCEndpointName(
    /* [string][in] */ const wchar_t *endpointName) {
  g_currentClientEndpoint = wstring(endpointName);
}

void LISEndCurrentClientRPCEndpoint(void) {
  g_currentClientEndpoint = wstring();
}

int _tmain(int argc, _TCHAR *argv[]) {
  SetConsoleCP(CP_UTF8);
  wstring serverEndpoint = wstring(L"sampleserver");

  if (!LIRStartRPCServer(serverEndpoint.c_str(),
                         LIServerRPCInterface_v1_0_s_ifspec)) {
    printf("server registration failed\n");
    return 1;
  } else {
    printf("server started\n");
  }

  char buf[256];
  while (!feof(stdin)) {
    fgets(buf, 255, stdin);
    printf("user typed %s\n", buf);

    if (g_currentClientEndpoint.length()) {
      wprintf(L"we have a client %s, attemping to send\n",
              g_currentClientEndpoint.c_str());
      void *client = LIRConnectServer(g_currentClientEndpoint.c_str(),
                                      &g_LIClientRPCInterfaceHandle);

      if (!client) {
        printf("failed connecting to client\n");
      } else {
        wchar_t wbuf[LICMaxStringLength];
        int len = MultiByteToWideChar(CP_UTF8, 0, buf, strlen(buf), wbuf,
                                      LICMaxStringLength);
        wbuf[len] = 0;
        LICSendString(wbuf);

        if (LIRDisconnectServer(client, &g_LIClientRPCInterfaceHandle)) {
          printf("client disconnected\n");
        } else {
          printf("failed to disconnect client\n");
        }
      }
    }
  }

  if (!LIRStopRPCServer()) {
    printf("cannot stop server, program terminates nonetheless\n");
  } else {
    printf("server terminated\n");
  }

  return 0;
}
