// PhraseEditor.cpp : main project file.

#include "stdafx.h"

#pragma unmanaged
#include "BIConfig.h"
#include "LFPlatform.h"
#include "OpenVanilla.h"

#pragma managed
#include <vcclr.h>

#include "BIServerRPCInterface.h"
#include "EditorForm.h"
#include "URLHandler.h"

using namespace OpenVanilla;
using namespace LFPlatform;
using namespace PhraseEditor;
using namespace System::Globalization;
using namespace System::Security::Permissions;
using namespace System::Threading;
using namespace System::Diagnostics;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

BISContext PMSharedContext = NULL;

class Service {
 public:
  static Process ^ GetRunningInstance() {
    Process ^ current = gcnew Process();
    current = Process::GetCurrentProcess();
    array<Process ^> ^ processes =
        Process::GetProcessesByName(current->ProcessName);
    int i;
    for (i = 0; i < processes->Length; i++) {
      Process ^ process = processes[i];
      if (process->Id != current->Id)
        if (Assembly::GetExecutingAssembly()->Location->Replace("/", "\\") ==
            current->MainModule->FileName)
          return process;
    }
    return nullptr;
  }
};

bool OpenConnection() {
  RpcTryExcept if (!BISOpenConnection(&PMSharedContext)) { return false; }
  RpcExcept(1) { return false; }
  RpcEndExcept;

  return true;
}

void CloseConnection() {
  RpcTryExcept BISCloseConnection(&PMSharedContext);
  RpcExcept(1) {}
  RpcEndExcept
}

[STAThreadAttribute] int main(array<System::String ^> ^ args) {
  RPCClientHelper::ServerHandle c_RPCServerInterfaceHandle;

  if (!(c_RPCServerInterfaceHandle = RPCClientHelper::ConnectServer(
            BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
    return 1;
  }

  if (!OpenConnection()) {
    return 1;
  }

  PreferenceConnector::SharedInstance = gcnew ManagedPreferenceConnector();

  if (args->Length) {
    String ^ url = args[0];
    if (url->StartsWith(L"ykeykey://")) {
      URLHandler::addPhraseFromURL(url);
      CloseConnection();

      if (c_RPCServerInterfaceHandle)
        RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                    &BIServerRPCInterfaceGlobalHandle);
      return 0;
    }
  }

  CreateMutex(0, 1, LPCWSTR("PhraseEditor"));
  if (GetLastError() == 183) {
    Process ^ instance = Service::GetRunningInstance();
    if (instance != nullptr) {
      SetForegroundWindow((HWND)instance->MainWindowHandle.ToPointer());
    }
    exit(-1);
  };

  String ^ locale = CultureInfo::CurrentCulture->Name;
  if (locale->Equals(L"zh-TW") || locale->Equals(L"zh-HK") ||
      locale->Equals(L"zh-CHT") || locale->Equals(L"zh-SG") ||
      locale->Equals(L"zh-MO")) {
    CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-TW", false);
    Thread::CurrentThread->CurrentCulture = cultureInfo;
    Thread::CurrentThread->CurrentUICulture = cultureInfo;
  }
  if (locale->Equals(L"zh-CN") || locale->Equals(L"zh-CHS")) {
    CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-CN", false);
    Thread::CurrentThread->CurrentCulture = cultureInfo;
    Thread::CurrentThread->CurrentUICulture = cultureInfo;
  }

  // Enabling Windows XP visual effects before any controls are created
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  // Create the main window and run it
  Application::Run(gcnew EditorForm());

  CloseConnection();

  if (c_RPCServerInterfaceHandle)
    RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                &BIServerRPCInterfaceGlobalHandle);

  return 0;
}
