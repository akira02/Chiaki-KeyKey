//
// ProcessHelper.cpp
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

#include "ProcessHelper.h"

#include <Lmcons.h>
#include <OpenVanilla.h>
#include <psapi.h>
#include <security.h>
#include <tlhelp32.h>
#include <windows.h>
#include <wtsapi32.h>

#include "LogKit.h"

namespace LFPlatform {

using namespace OpenVanilla;

bool ProcessHelper::LaunchProgram(const string& executable,
                                  const string& launchPath) {
  wstring wLaunchPath = OVUTF16::FromUTF8(launchPath);
  wstring wExecutablePath = OVUTF16::FromUTF8(executable);

  STARTUPINFO startUpSearch;
  PROCESS_INFORMATION processInfo;
  GetStartupInfo(&startUpSearch);
  BOOL result = CreateProcess(wExecutablePath.c_str(), NULL, NULL, NULL, FALSE,
                              DETACHED_PROCESS, NULL, wLaunchPath.c_str(),
                              &startUpSearch, &processInfo);

  return result == TRUE;
}

const string ProcessHelper::CurrentProcessImageName() {
  string result;
  size_t wssize = 1024 + 1;
  wchar_t* wstr = new wchar_t[wssize];
  wmemset(wstr, 0, wssize);
  DWORD rsp = GetProcessImageFileName(GetCurrentProcess(), wstr, (DWORD)wssize);
  if (rsp) result = OVUTF8::FromUTF16(wstr);
  delete wstr;
  return result;
}

bool ProcessHelper::IsSystemUser() {
  wchar_t userName[UNLEN + 1];
  DWORD userNameLength = UNLEN;
  wmemset(userName, 0, UNLEN + 1);
  if (GetUserName(userName, &userNameLength)) {
    return !_wcsicmp(userName, L"SYSTEM");
  }

  return false;
}

bool ProcessHelper::ProcessNameExistsForCurrentUser(const string& expression,
                                                    size_t* count) {
  OVWildcard wilcard(expression);

  Logger logger("ProcessNameExistsForCurrentUser");

  string username;
  wchar_t userName[UNLEN + 1];
  DWORD userNameLength = UNLEN;
  wmemset(userName, 0, UNLEN + 1);
  if (GetUserName(userName, &userNameLength)) {
    username = OVUTF8::FromUTF16(userName);
  }

  HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (processSnapshot == INVALID_HANDLE_VALUE) return false;

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(processSnapshot, &pe32)) {
    CloseHandle(processSnapshot);
    return false;
  }

  size_t foundCount = 0;

  do {
    DWORD dwPriorityClass = 0;
    HANDLE processHandle =
        OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
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

              if (uname == username && wilcard.match(processName)) {
                foundCount++;
                // logger.debug("MATCH process name: %s, user name: %s; current
                // user = %s", processName.c_str(), uname.c_str(),
                // username.c_str());
              } else {
                // logger.debug("NO MATCH process name: %s, user name: %s;
                // current user = %s", processName.c_str(), uname.c_str(),
                // username.c_str());
              }
            }
          }
          free(userInfo);
        }

        CloseHandle(processToken);
      }
      CloseHandle(processHandle);
    }
  } while (Process32Next(processSnapshot, &pe32));

  CloseHandle(processSnapshot);

  if (count) *count = foundCount;

  return foundCount != 0;
}

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

bool ProcessHelper::IsRunningUnderWow64() {
  LPFN_ISWOW64PROCESS fnIsWow64Process;
  BOOL bIsWow64 = FALSE;
  fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
      GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
  if (NULL != fnIsWow64Process) {
    if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
      // handle error
    }
  }
  return bIsWow64 == TRUE;
}

};  // namespace LFPlatform
