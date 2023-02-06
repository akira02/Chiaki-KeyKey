//
// ServerShooter.cpp
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

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

#include <LFPlatform.h>
#include <Lmcons.h>
#include <OpenVanilla.h>
#include <psapi.h>
#include <security.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <windows.h>
#include <wtsapi32.h>

#include <iostream>

#include "BIConfig.h"
#include "BIServerRPCInterface.h"

// #include <Msiquery.h>

using namespace std;
using namespace OpenVanilla;
using namespace LFPlatform;

BISContext g_context;
RPCClientHelper::ServerHandle g_interfaceHandle = 0;

void Log(const char* msg) {
  Logger logger("ServerShooter");
  logger.error(msg);
}

bool Connect() {
  // init the RPC server service interface
  if (!(g_interfaceHandle = RPCClientHelper::ConnectServer(
            BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
    Log("Cannot init the RPC service interface\n");
    return false;
  }

  return true;
}

void Disconnect() {
  RPCClientHelper::Disconnect(g_interfaceHandle,
                              &BIServerRPCInterfaceGlobalHandle);
}

void InitLog() {
  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);
  string logFilename = OVPathHelper::PathCat(appDataDir, "ServerShooter.log");
  Logger::UseLogFile(logFilename);
}

extern "C" OVEXPORT void CleanPropertyInstallerSignalFile() {
  InitLog();
  Log("CleanPropertyInstallerSignalFile");
}

/*
extern "C" OVEXPORT void CheckPropertyInstallerSignalFile(MSIHANDLE hInstall)
{
        InitLog();

        // set MSI property if the temp file exists (stupid, but yea)
        if (1) {
                UINT result = MsiSetProperty(hInstall, L"CALLPROPERTYINSTALLER",
L"1"); switch (result) { case ERROR_FUNCTION_FAILED:
                                Log("CheckPropertyInstallerSignalFile:
ERROR_FUNCTION_FAILED"); break;

                        case ERROR_INVALID_HANDLE:
                                Log("CheckPropertyInstallerSignalFile:
ERROR_INVALID_HANDLE"); break;

                        case ERROR_INVALID_PARAMETER:
                                Log("CheckPropertyInstallerSignalFile:
ERROR_INVALID_PARAMETER"); break;

                        case ERROR_SUCCESS:
                                Log("CheckPropertyInstallerSignalFile:
ERROR_SUCCESS"); break;

                }
        }
}
*/

extern "C" OVEXPORT void StopUsingSignedModules() {
  if (Connect()) {
    if (BISOpenConnection(&g_context)) {
      BISStatelessFunctionVoid(BISFunctionStopUsingSignedModules);
      BISCloseConnection(&g_context);
    }

    Disconnect();
  }
}

extern "C" OVEXPORT void ResumeUsingSignedModules() {
  if (Connect()) {
    if (BISOpenConnection(&g_context)) {
      BISStatelessFunctionVoid(BISFunctionResumeUsingSignedModules);
      BISCloseConnection(&g_context);
    }

    Disconnect();
  }
}

extern "C" OVEXPORT void Shutdown() {
  InitLog();
  OVWildcard wildcard("*" BASEIME_SERVER_EXECUTABLE);
  OVWildcard wildcard1("*PhraseEditor.exe");
  OVWildcard wildcard2("*PreferencesMain.exe");

  Logger logger("ServerShooter");
  logger.error("Server shooter begins, shooting: %s",
               "*" BASEIME_SERVER_EXECUTABLE);

  HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (processSnapshot == INVALID_HANDLE_VALUE) return;

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(processSnapshot, &pe32)) {
    CloseHandle(processSnapshot);
    return;
  }

  do {
    DWORD dwPriorityClass = 0;
    HANDLE processHandle =
        OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE,
                    pe32.th32ProcessID);
    if (processHandle) {
      string processName;
      size_t wssize = 1024 + 1;
      wchar_t* wstr = new wchar_t[wssize];
      wmemset(wstr, 0, wssize);
      DWORD rsp = GetProcessImageFileName(processHandle, wstr, (DWORD)wssize);
      if (rsp) processName = OVUTF8::FromUTF16(wstr);
      delete wstr;

      HANDLE processToken = 0;
      BOOL result = OpenProcessToken(processHandle, TOKEN_READ, &processToken);

      if (result) {
        DWORD desiredLength = 0;
        GetTokenInformation(processToken, TokenUser, NULL, 0, &desiredLength);

        if (desiredLength) {
          TOKEN_USER* userInfo = (TOKEN_USER*)calloc(1, desiredLength);
          BOOL tokenResult = GetTokenInformation(
              processToken, TokenUser, userInfo, desiredLength, &desiredLength);
          if (tokenResult) {
            PSID sid = userInfo->User.Sid;

            wchar_t name[1024];
            wchar_t domainName[1024];
            DWORD nameSize = 1024;
            DWORD domainNameSize = 1024;
            SID_NAME_USE nameUse;

            BOOL lookupResult =
                LookupAccountSid(NULL, sid, name, &nameSize, domainName,
                                 &domainNameSize, &nameUse);

            if (lookupResult) {
              string uname = OVUTF8::FromUTF16(name);

              logger.debug("Scanning process name: %s, user name: %s",
                           processName.c_str(), uname.c_str());
              Logger::FlushLogger();
              if (wildcard.match(processName) || wildcard1.match(processName) ||
                  wildcard2.match(processName)) {
                logger.debug("match!");
                Logger::FlushLogger();
                BOOL result = TerminateProcess(processHandle, 0);
                logger.debug(
                    ">>>>>>>>> MATCH process name: %s, user name: %s, "
                    "killing... result: %d",
                    processName.c_str(), uname.c_str(), result);
                Logger::FlushLogger();
              } else {
                logger.debug("NO MATCH process name: %s, user name: %s",
                             processName.c_str(), uname.c_str());
                Logger::FlushLogger();
              }
            }
          }
          free(userInfo);
        }

        CloseHandle(processToken);
      }
      CloseHandle(processHandle);
    } else {
      logger.debug("Failed to obtain process handle for ID %d (0x%08x)",
                   pe32.th32ProcessID, pe32.th32ProcessID);
    }
  } while (Process32Next(processSnapshot, &pe32));

  CloseHandle(processSnapshot);

  // sleep 2.5 seconds so that the server is completely shoot down (and
  // supplement.db can be removed)
  Sleep(2500);

  // deletes supplement DB file
  // get the ServerLocation key
  string launchPath;
  if (!RegistryHelper::KeyExists(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY)) {
    logger.debug("cannot find the registry key ServerLocation\n");
  } else {
    HKEY key =
        RegistryHelper::OpenKey(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY);
    logger.debug("Querying value named: %s\n",
                 BASEIME_SERVER_LOCATION_VALUE_NAME);
    string location = RegistryHelper::QueryStringValue(
        key, BASEIME_SERVER_LOCATION_VALUE_NAME);

    if (location.size()) {
      launchPath = location;
      logger.debug("Found server location: %s\n", launchPath.c_str());
    }

    RegCloseKey(key);
  }

  if (!launchPath.size()) {
    return;
  }

  // copy the supplement database to <server_root>\Databases\

  string dbDir = OVPathHelper::PathCat(launchPath, "Databases");
  string deleteFile = OVPathHelper::PathCat(dbDir, "Supplement.db");

  // delete <server_root>\Databases\Supplement.db
  if (OVPathHelper::PathExists(deleteFile)) {
    logger.debug("deleting: %s\n", deleteFile.c_str());
    if (!DeleteFile(OVUTF16::FromUTF8(deleteFile).c_str())) {
      logger.debug("failed to delete file!\n");
      return;
    }
  }
}
