//
// SupplementTool.cpp
//
// Copyright (c) 2004-2010 The OpenVanilla Project (http://openvanilla.org)
// All rights reserved.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <LFPlatform.h>
#include <OpenVanilla.h>
#include <shlwapi.h>
#include <tchar.h>
#include <windows.h>

#include <iostream>

#include "BIConfig.h"
#include "BIServerRPCInterface.h"

using namespace std;
using namespace OpenVanilla;
using namespace LFPlatform;

void InitConsole() {
  BOOL rc = FALSE;

  rc = AttachConsole((DWORD)GetCurrentProcess());

  if (!rc) rc = AllocConsole();

  if (rc) rc = _tfreopen(_T("CONOUT$"), _T("a"), stdout) != NULL;

  if (rc) ios::sync_with_stdio();
}

const string ShiftString(vector<string>& vec) {
  if (!vec.size()) return string();

  string result = vec[0];
  vec.erase(vec.begin());
  return result;
}

BISContext g_context;
RPCClientHelper::ServerHandle g_interfaceHandle = 0;

bool Connect() {
  // init the RPC server service interface
  if (!(g_interfaceHandle = RPCClientHelper::ConnectServer(
            BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
    printf("Cannot init the RPC service interface\n");
    return false;
  }

  return true;
}

void Disconnect() {
  RPCClientHelper::Disconnect(g_interfaceHandle,
                              &BIServerRPCInterfaceGlobalHandle);
}

void InstallSupplementDatabaseShutdownServer() {
  if (!Connect()) return;

  RpcTryExcept {
    boolean result = BISOpenConnection(&g_context);
    if (!result) {
      printf("Server returns error\n");
      return;
    }
  }
  RpcExcept(1) {
    printf("No RPC connection\n");
    return;
  }
  RpcEndExcept;

  RpcTryExcept { BISShutdownServerForDatabaseUpdate(g_context); }
  RpcExcept(1) {
    printf("Cannot shutdown server or connection is already lost\n");
  }
  RpcEndExcept;

  if (g_interfaceHandle) Disconnect();

  printf("Server shutdown.\n");
}

void InstallSupplementDatabaseRestartServer() {
  if (!Connect()) return;

  RpcTryExcept {
    boolean result = BISOpenConnection(&g_context);
    if (!result) {
      printf("Server returns error\n");
      return;
    }
  }
  RpcExcept(1) {
    printf("No RPC connection\n");
    return;
  }
  RpcEndExcept;

  RpcTryExcept { BISRestartServerAndCompleteDatabaseUpdate(g_context); }
  RpcExcept(1) {
    printf("Cannot restart server or connection is already lost\n");
  }
  RpcEndExcept;

  if (g_interfaceHandle) Disconnect();

  printf("Server restarted.\n");
}

void InstallSupplementDatabase() {
  // get current dir
  wchar_t wCurrentDir[1024];
  GetModuleFileName(0, wCurrentDir, 1024);
  string toolPath =
      OVPathHelper::DirectoryFromPath(OVUTF8::FromUTF16(wCurrentDir));

  // see if supplement database exists in our directory
  string installFile = OVPathHelper::PathCat(toolPath, "suppinst.db");

  printf("looking for install db: %s\n", installFile.c_str());

  if (!OVPathHelper::PathExists(installFile)) {
    printf("not exists\n");
    return;
  }

  // get the ServerLocation key
  string launchPath;
  if (!RegistryHelper::KeyExists(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY)) {
    printf("cannot find the registry key ServerLocation\n");
  } else {
    HKEY key =
        RegistryHelper::OpenKey(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY);
    printf("Querying value named: %s\n", BASEIME_SERVER_LOCATION_VALUE_NAME);
    string location = RegistryHelper::QueryStringValue(
        key, BASEIME_SERVER_LOCATION_VALUE_NAME);

    if (location.size()) {
      launchPath = location;
      printf("Found server location: %s\n", launchPath.c_str());
    }

    RegCloseKey(key);
  }

  if (!launchPath.size()) {
    return;
  }

  // copy the supplement database to <server_root>\Databases\

  string dbDir = OVPathHelper::PathCat(launchPath, "Databases");
  string deleteFile = OVPathHelper::PathCat(dbDir, "Supplement.db");

  string badTargetFile = OVPathHelper::PathCat(dbDir, "SuppementInstall.db");

  if (OVPathHelper::PathExists(badTargetFile)) {
    printf("Deleting stale bad install file\n");
    if (!DeleteFile(OVUTF16::FromUTF8(badTargetFile).c_str())) {
      printf("failed to delete stale bad install file!\n");
      InstallSupplementDatabaseRestartServer();
      return;
    }
  }

  string targetFile = OVPathHelper::PathCat(dbDir, "SupplementInstall.db");
  printf("target file: %s\n", targetFile.c_str());

  if (OVPathHelper::PathExists(targetFile)) {
    printf("Deleting stale install file\n");
    if (!DeleteFile(OVUTF16::FromUTF8(targetFile).c_str())) {
      printf("failed to delete stale install file!\n");
      return;
    }
  }

  if (!CopyFile(OVUTF16::FromUTF8(installFile).c_str(),
                OVUTF16::FromUTF8(targetFile).c_str(), FALSE)) {
    printf("file copy failed!\n");
    return;
  }

  // shutdown the server -- stage 1
  InstallSupplementDatabaseShutdownServer();

  // magical sleep for modules to back out themselves, freeing up the locked DB
  // file handle, etc.
  Sleep(2000);

  // delete <server_root>\Databases\Supplement.db
  if (OVPathHelper::PathExists(deleteFile)) {
    printf("deleting: %s\n", deleteFile.c_str());
    if (!DeleteFile(OVUTF16::FromUTF8(deleteFile).c_str())) {
      printf("failed to delete file!\n");
      InstallSupplementDatabaseRestartServer();
      return;
    }
  }

  // rename the copied supplement database to Supplement.db
  printf("renaming %s to %s\n", targetFile.c_str(), deleteFile.c_str());
  if (!MoveFile(OVUTF16::FromUTF8(targetFile).c_str(),
                OVUTF16::FromUTF8(deleteFile).c_str())) {
    printf("failed to rename file!");
  }

  // restart the server -- stage 2
  InstallSupplementDatabaseRestartServer();
}

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  InstallSupplementDatabase();
  return 0;
}
