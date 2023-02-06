/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/
// CinInstaller.cpp : main project file.

#include "stdafx.h"

#pragma unmanaged
#include "BIConfig.h"
#include "LFPlatform.h"
#include "OpenVanilla.h"
using namespace LFPlatform;
using namespace OpenVanilla;

#pragma managed
#include <vcclr.h>

#include "BIServerRPCInterface.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Globalization;
using namespace System::IO;
using namespace System::Security::Permissions;
using namespace System::Threading;
using namespace System::Windows::Forms;
using namespace Microsoft::Win32;

// Get the path to install .cin files.
static String ^
    CinPath() {
      String ^ path = "";
      RegistryKey ^ registryKey = Registry::LocalMachine;
      registryKey = registryKey->OpenSubKey(L"SOFTWARE\\Yahoo\\KeyKey", true);
      String ^ locale = CultureInfo::CurrentUICulture->Name;

      try {
        path = registryKey->GetValue(L"ServerLocation")->ToString();
      } catch (Exception ^) {
        if (locale->Equals(L"zh-TW"))
          MessageBox::Show(
              L"\u60a8\u4e26\u6c92\u6709\u5b89\u88dd Yahoo! "
              L"\u5947\u6469\u8f38\u5165\u6cd5\uff0c\u7121\u6cd5\u5b89\u88dd"
              L"\u60a8\u7684 .cin "
              L"\u8f38\u5165\u6cd5\u8868\u683c\u6a94\u6848\uff01",
              L"\u932f\u8aa4");
        else if (locale->Equals(L"zh-CN"))
          MessageBox::Show(
              L"\u60a8\u5e76\u6ca1\u6709\u5b89\u88c5 Yahoo! "
              L"\u5947\u6469\u8f93\u5165\u6cd5\uff0c\u65e0\u6cd5\u5b89\u88c5"
              L"\u60a8\u7684 .cin "
              L"\u8f93\u5165\u6cd5\u8868\u683c\u6587\u4ef6\uff01",
              L"\u9519\u8bef");
        else
          MessageBox::Show(
              L"You did not install Yahoo! KeyKey. Unable to install your .cin "
              L"files.",
              L"Error!");
        return L"";
      }

      if (!path->Length) {
        if (locale->Equals(L"zh-TW"))
          MessageBox::Show(
              L"\u60a8\u4e26\u6c92\u6709\u5b89\u88dd Yahoo! "
              L"\u5947\u6469\u8f38\u5165\u6cd5\uff0c\u7121\u6cd5\u5b89\u88dd"
              L"\u60a8\u7684 .cin "
              L"\u8f38\u5165\u6cd5\u8868\u683c\u6a94\u6848\uff01",
              L"\u932f\u8aa4");
        else if (locale->Equals(L"zh-CN"))
          MessageBox::Show(
              L"\u60a8\u5e76\u6ca1\u6709\u5b89\u88c5 Yahoo! "
              L"\u5947\u6469\u8f93\u5165\u6cd5\uff0c\u65e0\u6cd5\u5b89\u88c5"
              L"\u60a8\u7684 .cin "
              L"\u8f93\u5165\u6cd5\u8868\u683c\u6587\u4ef6\uff01",
              L"\u9519\u8bef");
        else
          MessageBox::Show(
              L"You did not install Yahoo! KeyKey. Unable to install your .cin "
              L"files.",
              L"Error!");
        return L"";
      }

      path = Environment::GetFolderPath(
                 Environment::SpecialFolder::ApplicationData) +
             Path::DirectorySeparatorChar + L"Yahoo! KeyKey" +
             Path::DirectorySeparatorChar;

      if (!Directory::Exists(path)) Directory::CreateDirectory(path);

      if (!Directory::Exists(path)) {
        if (locale->Equals(L"zh-TW"))
          MessageBox::Show(
              L"\u60a8\u4e26\u6c92\u6709\u5b89\u88dd Yahoo! "
              L"\u5947\u6469\u8f38\u5165\u6cd5\uff0c\u7121\u6cd5\u5b89\u88dd"
              L"\u60a8\u7684 .cin "
              L"\u8f38\u5165\u6cd5\u8868\u683c\u6a94\u6848\uff01",
              L"\u932f\u8aa4");
        else if (locale->Equals(L"zh-CN"))
          MessageBox::Show(
              L"\u60a8\u5e76\u6ca1\u6709\u5b89\u88c5 Yahoo! "
              L"\u5947\u6469\u8f93\u5165\u6cd5\uff0c\u65e0\u6cd5\u5b89\u88c5"
              L"\u60a8\u7684 .cin "
              L"\u8f93\u5165\u6cd5\u8868\u683c\u6587\u4ef6\uff01",
              L"\u9519\u8bef");
        else
          MessageBox::Show(
              L"You did not install Yahoo! KeyKey. Unable to install your .cin "
              L"files.",
              L"Error!");
        return L"";
      }
      String ^ dataTablePath = path + L"\\DataTables\\";
      if (!Directory::Exists(dataTablePath)) {
        try {
          Directory::CreateDirectory(dataTablePath);
        } catch (Exception ^) {
          if (locale->Equals(L"zh-TW"))
            MessageBox::Show(
                L"\u5b89\u88dd\u5931\u6557\uff0c\u7121\u6cd5\u5728\u60a8\u7684"
                L"\u78c1\u789f\u4e0a\u6b63\u78ba\u5efa\u7acb\u6240\u9700\u8981"
                L"\u7684\u76ee\u9304\uff01",
                L"\u932f\u8aa4");
          else if (locale->Equals(L"zh-CN"))
            MessageBox::Show(
                L"\u5b89\u88c5\u5931\u8d25\uff0c\u65e0\u6cd5\u5728\u60a8\u7684"
                L"\u78c1\u789f\u4e0a\u6b63\u786e\u5efa\u7acb\u6240\u9700\u8981"
                L"\u7684\u76ee\u5f55\uff01",
                L"\u9519\u8bef");
          else
            MessageBox::Show(
                L"Failed to create new folder on your disk, installation "
                L"canceled.",
                L"Error!");
          return L"";
        }
      }
      String ^ genericPath = dataTablePath + L"\\Generic\\";
      if (!Directory::Exists(genericPath)) {
        try {
          Directory::CreateDirectory(genericPath);
        } catch (Exception ^) {
          if (locale->Equals(L"zh-TW"))
            MessageBox::Show(
                L"\u5b89\u88dd\u5931\u6557\uff0c\u7121\u6cd5\u5728\u60a8\u7684"
                L"\u78c1\u789f\u4e0a\u6b63\u78ba\u5efa\u7acb\u6240\u9700\u8981"
                L"\u7684\u76ee\u9304\uff01",
                L"\u932f\u8aa4");
          else if (locale->Equals(L"zh-CN"))
            MessageBox::Show(
                L"\u5b89\u88c5\u5931\u8d25\uff0c\u65e0\u6cd5\u5728\u60a8\u7684"
                L"\u78c1\u789f\u4e0a\u6b63\u786e\u5efa\u7acb\u6240\u9700\u8981"
                L"\u7684\u76ee\u5f55\uff01",
                L"\u9519\u8bef");
          else
            MessageBox::Show(
                L"Failed to create new folder on your disk. Installation "
                L"canceled.",
                L"Error!");
          return L"";
        }
      }
      return genericPath;
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

[STAThreadAttribute]

    int
    main(array<System::String ^> ^ args) {
      String ^ path = L"";
      for each (String ^ part in args) {
        path += part + " ";
      }
      path = path->Trim();

      if (!path->Length) return 1;

      // MessageBox::Show(L"short:" + path);
      pin_ptr<const wchar_t> wchptr = PtrToStringChars(path);
      wchar_t newPath[2048];
      GetLongPathName(wchptr, newPath, 2048);
      path = gcnew String(newPath);
      // MessageBox::Show(path);

      String ^ locale = CultureInfo::CurrentCulture->Name;

      if (locale->Equals(L"zh-TW") || locale->Equals(L"zh-HK") ||
          locale->Equals(L"zh-CHT") || locale->Equals(L"zh-SG") ||
          locale->Equals(L"zh-MO")) {
        CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-TW", false);
        Thread::CurrentThread->CurrentCulture = cultureInfo;
        Thread::CurrentThread->CurrentUICulture = cultureInfo;
      } else if (locale->Equals(L"zh-CN") || locale->Equals(L"zh-CHS")) {
        CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-CN", false);
        Thread::CurrentThread->CurrentCulture = cultureInfo;
        Thread::CurrentThread->CurrentUICulture = cultureInfo;
      }

      locale = CultureInfo::CurrentUICulture->Name;

      if (File::Exists(path)) {
        String ^ filename = Path::GetFileName(path);
        String ^ extension = Path::GetExtension(path);
        String ^ yahooKeyKey = L"Yahoo! KeKey";

        if (locale->Equals(L"zh-TW"))
          yahooKeyKey = L"Yahoo! \u5947\u6469\u8f38\u5165\u6cd5";
        else if (locale->Equals(L"zh-CN"))
          yahooKeyKey = L"Yahoo! \u5947\u6469\u8f93\u5165\u6cd5";

        if (extension != L".cin") return 1;

        String ^ cinPath = CinPath();
        if (!cinPath->Length) return 1;

        String ^ message;
        DialogResult result;
        String ^ targetPath = cinPath + filename;

        if (targetPath->Equals(path)) return 0;

        if (File::Exists(targetPath)) {
          if (locale->Equals(L"zh-TW"))
            message =
                L"\u60a8\u4e4b\u524d\u4fbf\u5df2\u7d93\u5b89\u88dd\u904e\u540d"
                L"\u70ba\u300c" +
                filename +
                L"\u300d\u7684\u8f38\u5165\u6cd5\u8868\u683c\u4e86\uff0c\u60a8"
                L"\u60f3\u8981\u522a\u9664\u820a\u7684\u7248\u672c\uff0c\u5b89"
                L"\u88dd\u65b0\u7684\u55ce\uff1f";
          else if (locale->Equals(L"zh-CN"))
            message =
                L"\u60a8\u4e4b\u524d\u4fbf\u5df2\u7ecf\u5b89\u88c5\u8fc7\u540d"
                L"\u4e3a\u2018" +
                filename +
                L"\u2019\u7684\u8f93\u5165\u6cd5\u8868\u683c\u4e86\uff0c\u60a8"
                L"\u60f3\u8981\u5220\u9664\u65e7\u7684\u7248\u672c\uff0c\u5b89"
                L"\u88c5\u65b0\u7684\u5417\uff1f";
          else
            message = L"The Input Method Table " + filename +
                      L" already exists.\n\nDo you want to delete the existing "
                      L"one and install the new one?";

          result =
              MessageBox::Show(message, yahooKeyKey, MessageBoxButtons::YesNo,
                               MessageBoxIcon::Question);

          if (result == DialogResult::Yes) {
            try {
              File::Delete(targetPath);
            } catch (Exception ^ ex) {
              String ^ errorMessage;
              if (locale->Equals(L"zh-TW"))
                errorMessage =
                    L"\u7121\u6cd5\u522a\u9664\u73fe\u6709\u7684\u8f38\u5165"
                    L"\u6cd5\u8868\u683c\uff0c\u5b89\u88dd\u53d6\u6d88\u3002";
              else if (locale->Equals(L"zh-CN"))
                errorMessage =
                    L"\u65e0\u6cd5\u5220\u9664\u73b0\u6709\u7684\u8f93\u5165"
                    L"\u6cd5\u8868\u683c\uff0c\u5b89\u88c5\u53d6\u6d88\u3002";
              else
                errorMessage =
                    L"Failed to delete existing Input Method Table. "
                    L"Installation canceled.";

              MessageBox::Show(errorMessage, yahooKeyKey, MessageBoxButtons::OK,
                               MessageBoxIcon::Error);
              return 1;
            }
          } else {
            String ^ errorMessage;
            if (locale->Equals(L"zh-TW"))
              errorMessage = L"\u5b89\u88dd\u5df2\u53d6\u6d88\u3002";
            else if (locale->Equals(L"zh-CN"))
              errorMessage = L"\u5b89\u88c5\u5df2\u53d6\u6d88\u3002";
            else
              errorMessage = L"Installation canceled.";

            MessageBox::Show(errorMessage, yahooKeyKey, MessageBoxButtons::OK,
                             MessageBoxIcon::Information);
            return 1;
          }
        }

        if (locale->Equals(L"zh-TW"))
          message =
              L"\u60a8\u78ba\u5b9a\u8981\u5b89\u88dd\u9019\u500b\u8f38\u5165"
              L"\u6cd5\u8868\u683c\u55ce\uff1f";
        else if (locale->Equals(L"zh-CN"))
          message =
              L"\u60a8\u786e\u5b9a\u8981\u5b89\u88c5\u8fd9\u4e2a\u8f93\u5165"
              L"\u6cd5\u8868\u683c\u5417\uff1f";
        else
          message = L"Do you really want to install the Input Method?";

        result =
            MessageBox::Show(message, yahooKeyKey, MessageBoxButtons::YesNo,
                             MessageBoxIcon::Question);

        if (result == DialogResult::Yes) {
          try {
            InstallSupplementDatabaseShutdownServer();
            File::Copy(path, targetPath);
            Sleep(2000);
            InstallSupplementDatabaseRestartServer();

            if (locale->Equals(L"zh-TW"))
              MessageBox::Show(L"\u5b89\u88dd\u5b8c\u6210\uff01", yahooKeyKey);
            else if (locale->Equals(L"zh-CN"))
              MessageBox::Show(L"\u5b89\u88c5\u5b8c\u6210\uff01", yahooKeyKey);
            else
              MessageBox::Show(L"Done!", yahooKeyKey);

          } catch (Exception ^ ex) {
            if (locale->Equals(L"zh-TW"))
              MessageBox::Show(L"\u5b89\u88dd\u5931\u6557\uff01", yahooKeyKey);
            else if (locale->Equals(L"zh-CN"))
              MessageBox::Show(L"\u5b89\u88c5\u5931\u8d25\uff01", yahooKeyKey);
            else
              MessageBox::Show(L"Failed to install Input Method.", yahooKeyKey);
          }
        } else {
          String ^ errorMessage;
          if (locale->Equals(L"zh-TW"))
            errorMessage = L"\u5b89\u88dd\u5df2\u53d6\u6d88\u3002";
          else if (locale->Equals(L"zh-CN"))
            errorMessage = L"\u5b89\u88c5\u5df2\u53d6\u6d88\u3002";
          else
            errorMessage = L"Installation canceled.";

          MessageBox::Show(errorMessage, yahooKeyKey, MessageBoxButtons::OK,
                           MessageBoxIcon::Information);
          return 1;
        }
      }

      return 0;
    }
