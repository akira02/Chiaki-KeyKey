//
// BaseIMETool.cpp
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

#ifndef _MSC_VER
// thw three strings are like blow, we use this stupid MSC macro trick to avoid
// compiler confusion... 中文 (繁體) - Yahoo! 奇摩輸入法 Yahoo!
// 奇摩輸入法升級安裝
// 我們需要再次重新開機，以完成升級程序。開完機後就可以使用新版本了。\n\nYour
// system will be restarted once again to complete the upgrade.
#endif

#define BASEIME_INSTALL_PRODUCT_DESCRIPTION                       \
  "\xE4\xB8\xAD\xE6\x96\x87 (\xE7\xB9\x81\xE9\xAB\x94) - Yahoo! " \
  "\xE5\xA5\x87\xE6\x91\xA9\xE8\xBC\xB8\xE5\x85\xA5\xE6\xB3\x95"

#define BASEIME_CHECK_TITLE                                                  \
  "Yahoo! "                                                                  \
  "\xe5\xa5\x87\xe6\x91\xa9\xe8\xbc\xb8\xe5\x85\xa5\xe6\xb3\x95\xe5\x8d\x87" \
  "\xe7\xb4\x9a\xe5\xae\x89\xe8\xa3\x9d"

#define BASEIME_CHECK_DESCRIPTION                                            \
  "\xe6\x88\x91\xe5\x80\x91\xe9\x9c\x80\xe8\xa6\x81\xe5\x86\x8d\xe6\xac\xa1" \
  "\xe9\x87\x8d\xe6\x96\xb0\xe9\x96\x8b\xe6\xa9\x9f\xef\xbc\x8c\xe4\xbb\xa5" \
  "\xe5\xae\x8c\xe6\x88\x90\xe5\x8d\x87\xe7\xb4\x9a\xe7\xa8\x8b\xe5\xba\x8f" \
  "\xe3\x80\x82\xe9\x96\x8b\xe5\xae\x8c\xe6\xa9\x9f\xe5\xbe\x8c\xe5\xb0\xb1" \
  "\xe5\x8f\xaf\xe4\xbb\xa5\xe4\xbd\xbf\xe7\x94\xa8\xe6\x96\xb0\xe7\x89\x88" \
  "\xe6\x9c\xac\xe4\xba\x86\xe3\x80\x82\n\nYour system will be restarted "   \
  "once again to complete the upgrade."

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

// from http://msdn.microsoft.com/en-us/library/aa376871(VS.85).aspx
BOOL MySystemShutdown() {
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;

  // Get a token for this process.
  if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return FALSE;

  // Get the LUID for the shutdown privilege.
  LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

  tkp.PrivilegeCount = 1;  // one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // Get the shutdown privilege for this process.
  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

  if (GetLastError() != ERROR_SUCCESS) return FALSE;

  // Shut down the system and force all applications to close.

  if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
                     SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                         SHTDN_REASON_MINOR_UPGRADE |
                         SHTDN_REASON_FLAG_PLANNED))
    return FALSE;

  return TRUE;
}

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

void ShowUsage() {
  InitConsole();
  cout << "usage: IMETool command [params]" << endl;
  // cout << "    (console)install <full pathname>" << endl;
  // cout << "    (console)uninstall <filename>" << endl;

  // cout << "    (console)install <full pathname> [launch filename for Vista]"
  // << endl; cout << "    (console)uninstall <filename> - it also shuts down
  // the server" << endl; cout << "    (console)shutdown" << endl; cout << "
  // (console)removeuninstallkey <expression> <negative expression>" << endl;
}

void Install(const string& file, const string& launchFile) {
  if (file == "") {
    cout << "see usage" << endl;
    return;
  }

  HKL kl = ImmInstallIME(
      OVUTF16::FromUTF8(file).c_str(),
      OVUTF16::FromUTF8(BASEIME_INSTALL_PRODUCT_DESCRIPTION).c_str());
  // HKL kl = ImmInstallIMEA(file.c_str(), BASEIME_INSTALL_PRODUCT_DESCRIPTION);
  printf("ImmInstallIME returns: 0x%08x\n", (UINT_PTR)kl);

  if (!kl) {
    printf("returns");
    return;
  }

  wchar_t* code = new wchar_t[1024];
  wmemset(code, 0, 1024);
  wsprintf(code, L"%08x", (UINT_PTR)kl);

  HKL result = LoadKeyboardLayout(code, KLF_ACTIVATE);
  printf("LoadKeyboardLayout returns: 0x%08x\n", (UINT_PTR)result);

  OSVERSIONINFOEX versionInfo;
  memset(&versionInfo, 0, sizeof(versionInfo));
  versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  if (!GetVersionEx((OSVERSIONINFO*)&versionInfo)) return;

  SYSTEM_INFO systemInfo;
  memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
  GetSystemInfo(&systemInfo);

  if (!(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        versionInfo.dwMajorVersion > 4))
    return;

  // fix for Vista and above
  if (versionInfo.dwMajorVersion >= 6) {
    //         result = LoadKeyboardLayout(code, KLF_REORDER);
    // printf("LoadKeyboardLayout returns: 0x%08x\n", (UINT_PTR)result);
    //
    //         result = LoadKeyboardLayout(code, KLF_SUBSTITUTE_OK);
    // printf("LoadKeyboardLayout returns: 0x%08x\n", (UINT_PTR)result);

    delete[] code;

    printf("launch file: %s\n", launchFile.c_str());
    if (launchFile != "") {
      wchar_t wCurrentDir[1024];
      GetModuleFileName(0, wCurrentDir, 1024);
      string programPath =
          OVPathHelper::DirectoryFromPath(OVUTF8::FromUTF16(wCurrentDir));
      string quickSetup = OVPathHelper::PathCat(programPath, launchFile);
      printf("launching %s from %s\n", quickSetup.c_str(), programPath.c_str());
      bool result = ProcessHelper::LaunchProgram(quickSetup, programPath);

      if (result)
        printf("launched\n");
      else
        printf("launch failed\n");
    }

  } else {
    printf("Non-Vista version doesn't load keyboard layout\n");
  }
}

void Uninstall(const string& file) {
  if (file == "") {
    cout << "see usage" << endl;
    return;
  }

  HKEY layouts = RegistryHelper::OpenKey(
      HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");
  if (!layouts) {
    cout << "fails to open the registry key" << endl;
    return;
  }

  vector<string> subkeyNames = RegistryHelper::SubkeysForKey(layouts);
  for (vector<string>::const_iterator si = subkeyNames.begin();
       si != subkeyNames.end(); ++si) {
    HKEY subkey = RegistryHelper::OpenKey(layouts, *si);
    if (!subkey) {
      cout << "cannot open subkey: " << *si << endl;
      continue;
    }

    string imeName = RegistryHelper::QueryStringValue(subkey, "Ime File");
    cout << "iterating: " << *si << ", IME name: " << imeName << endl;
    RegCloseKey(subkey);

    if (imeName.length() && OVWildcard::Match(imeName, file)) {
      UINT_PTR localeid = 0;
      wstring wlid = OVUTF16::FromUTF8(*si);
      _stscanf_s(wlid.c_str(), L"%x", &localeid);
      BOOL result = UnloadKeyboardLayout((HKL)localeid);
      cout << "UnloadKeyboardLayout returns " << (int)result
           << " for locale id " << hex << localeid << endl;

      cout << "we have a match, deleting it... ";
      LONG lresult = RegDeleteKey(layouts, OVUTF16::FromUTF8(*si).c_str());

      cout << (lresult == ERROR_SUCCESS ? "done" : "error") << endl;
    }
  }

  RegCloseKey(layouts);
}

void Check(const string& regkey, const string& imePath) {
  if (regkey == "" || imePath == "") {
    cout << "see usage" << endl;
    return;
  }

  HKEY layouts = RegistryHelper::OpenKey(
      HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");
  if (!layouts) {
    cout << "fails to open the registry key" << endl;
    return;
  }

  bool found = false;

  vector<string> subkeyNames = RegistryHelper::SubkeysForKey(layouts);
  for (vector<string>::const_iterator si = subkeyNames.begin();
       si != subkeyNames.end(); ++si) {
    HKEY subkey = RegistryHelper::OpenKey(layouts, *si);
    if (!subkey) {
      cout << "cannot open subkey: " << *si << endl;
      continue;
    }

    string imeName = RegistryHelper::QueryStringValue(subkey, "Ime File");
    cout << "iterating: " << *si << ", IME name: " << imeName << endl;
    RegCloseKey(subkey);

    if (imeName.length() && OVWildcard::Match(imeName, regkey)) {
      found = true;
      cout << "we have a match, that's good... " << endl;
    }
  }

  RegCloseKey(layouts);

  if (!found) {
    MessageBox(NULL, OVUTF16::FromUTF8(BASEIME_CHECK_DESCRIPTION).c_str(),
               OVUTF16::FromUTF8(BASEIME_CHECK_TITLE).c_str(),
               MB_OK | MB_SYSTEMMODAL);

    wchar_t* code = new wchar_t[1024];
    wmemset(code, 0, 1024);
    wsprintf(code, L"%08x", 0x00000404);

    HKL result = LoadKeyboardLayout(code, KLF_ACTIVATE);
    printf("LoadKeyboardLayout (KBDUS for zh-TW) returns: 0x%08x\n",
           (UINT_PTR)result);

    // load US keyboard, then install
    Install(imePath, "");

    MySystemShutdown();
  }
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

// void Shutdown()
// {
//  if (!Connect())
//      return;
//
//     RpcTryExcept {
//         boolean result = BISOpenConnection(&g_context);
//      if (!result) {
//             printf("Server returns error\n");
//             return;
//      }
//     }
//     RpcExcept(1) {
//         printf("No RPC connection\n");
//         return;
//     }
//     RpcEndExcept;
//
//
//     RpcTryExcept {
//         BISShutdownServer(g_context);
//     }
//     RpcExcept(1) {
//         printf("Cannot shutdown server or connection is already lost\n");
//     }
//     RpcEndExcept;
//
//     if (g_interfaceHandle)
//      Disconnect();
//
//     Sleep(5000);
//     printf("Server shutdown.\n");
// }

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  vector<string> params = OVStringHelper::SplitBySpacesOrTabs(lpCmdLine);
  string pcmd = ShiftString(params);

  if (pcmd == "") {
    ShowUsage();
    return 1;
  }

  if (OVWildcard::Match(pcmd, "console*")) {
    InitConsole();
  }

  if (OVWildcard::Match(pcmd, "*uninstall")) {
    // Shutdown();
    string ime = ShiftString(params);
    Uninstall(ime);
  } else if (OVWildcard::Match(pcmd, "*install")) {
    string ime = ShiftString(params);
    string launch = ShiftString(params);
    Install(ime, launch);
  } else if (OVWildcard::Match(pcmd, "*check")) {
    string regkey = ShiftString(params);
    string imePath = ShiftString(params);

    // InitConsole();

    Check(regkey, imePath);
  }
  // else if (OVWildcard::Match(pcmd, "*shutdown")) {
  //     Shutdown();
  // }
  // else if (OVWildcard::Match(pcmd, "*removeuninstallkey")) {
  //     // we can't put these two shifts in a func call, because the evaluation
  //     order is from backward... orz string exp = ShiftString(params); string
  //     negexp = ShiftString(params); RemoveUninstallKey(exp, negexp);
  // }

  return 0;
}
