// ServiceUI.cpp : main project file.

#include "stdafx.h"

#pragma unmanaged
#include "LFPlatform.h"

#pragma managed
#include <vcclr.h>

#include "BIClientRPCInterface.h"
#include "BIConfig.h"
#include "BIServerRPCInterface.h"

#define SW_SHOWNORMAL 1
#define SW_SHOWNOACTIVATE 4
#define SW_HIDE 0

// #define HWND_TOPMOST 0xffff
// #define HWND_TOP 0x0000
#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOACTIVATE 0x0010
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOOWNERZORDER 0x0200

using namespace LFPlatform;
using namespace System;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Diagnostics;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;
using namespace TakaoPreference;

ref class ManagedPreferenceConnector : public PreferenceConnector {
 public:
  virtual bool validateFile(String ^ packageFile,
                            String ^ signatureFile) override;
  virtual void closePreferencesPanel() override;
};

class BaseIMEService {
 protected:
  static RPCClientHelper::ServerHandle c_RPCServerInterfaceHandle;

  static BISContext CreateContext() {
    BISContext context;

    RpcTryExcept {
      boolean result = BISOpenConnection(&context);
      if (!result) {
        return 0;
      }
    }
    RpcExcept(1) { return 0; }
    RpcEndExcept;

    return context;
  }

  static void DestroyContext(BISContext context) {
    if (context) {
      RpcTryExcept { BISCloseConnection(&context); }
      RpcExcept(1) {}
      RpcEndExcept;
    }
  }

  // static bool CallExport(BISContext context, const wchar_t* filename)
  // {
  //     RpcTryExcept {
  //         return BISExportUserPhraseDB(context, filename) == TRUE;
  //     }
  //     RpcExcept(1) {
  //         return false;
  //     }
  //     RpcEndExcept;
  //     return false;
  // }
  //
  // static bool CallImport(BISContext context, const wchar_t* filename)
  // {
  //     RpcTryExcept {
  //         return BISImportUserPhraseDB(context, filename) == TRUE;
  //     }
  //     RpcExcept(1) {
  //         return false;
  //     }
  //     RpcEndExcept;
  //     return false;
  // }

  static bool ValidateFile(BISContext context, const wchar_t* file,
                           const wchar_t* signature) {
    RpcTryExcept { return BISValidateFile(context, file, signature) == TRUE; }
    RpcExcept(1) { return false; }
    RpcEndExcept;
    return false;
  }

  static void CallClosePreferencesPanel(BISContext context) {
    RpcTryExcept { BISClosePreferences(context); }
    RpcExcept(1) {}
    RpcEndExcept;
  }

 public:
  static bool PrepareRPC() {
    Logger logger;
    if (!(c_RPCServerInterfaceHandle = RPCClientHelper::ConnectServer(
              BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
      logger.error("Cannot init the RPC service interface");
      return false;
    }

    return true;
  }

  static void CloseRPC() {
    if (c_RPCServerInterfaceHandle)
      RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                  &BIServerRPCInterfaceGlobalHandle);
  }

  static void PerformClosePreferencesPanel() {
    RpcTryExcept { BICRequestSendFakeKey(); }
    RpcExcept(1) {}
    RpcEndExcept;
  }

  static void ClosePreferencesPanel() {
    RPCClientHelper::ServerHandle client = RPCClientHelper::ConnectServer(
        "YKPreferencesMainRPC", &BIClientRPCInterfaceHandle);

    if (!client) {
      return;
    }

    PerformClosePreferencesPanel();

    RPCClientHelper::Disconnect(client, &BIClientRPCInterfaceHandle);

    //        BISContext context = CreateContext();
    //		CallClosePreferencesPanel(context);
    //		DestroyContext(context);
  }

  static bool Validate(String ^ file, String ^ sig) {
    BISContext context = CreateContext();
    if (!context) return false;
    pin_ptr<const wchar_t> filePtr = PtrToStringChars(file);
    pin_ptr<const wchar_t> sigPtr = PtrToStringChars(sig);
    bool result = ValidateFile(context, filePtr, sigPtr);
    DestroyContext(context);
    return result;
  }

  //     static bool Export()
  //     {
  //         BISContext context = CreateContext();
  //         if (!context)
  //             return false;
  //
  //         SaveFileDialog^ u_exportFileDialog;
  //         u_exportFileDialog = gcnew SaveFileDialog();
  //         u_exportFileDialog->DefaultExt = "txt";
  //         u_exportFileDialog->Filter = "Text file|*.txt|All files|*.*";
  //         u_exportFileDialog->FileName = "";
  //         u_exportFileDialog->ShowDialog();
  //         String^ filename = u_exportFileDialog->FileName;
  //         pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
  //         bool result = false;
  //
  //         if (filename->Length > 0 && filename->Length < BISMaxStringLength -
  //         1)
  //         {
  //             result = CallExport(context, wchptr);
  //
  // 	        if (result)
  // 	        {
  // 		::MessageBox(0, L"Export done", L"Service UI", MB_OK |
  // MB_ICONINFORMATION | MB_SYSTEMMODAL);
  // 	        }
  //     else
  // 	{
  //                 ::MessageBox(0, L"Export error", L"Service UI", MB_OK |
  //                 MB_ICONINFORMATION | MB_SYSTEMMODAL);
  // 	        }
  // }
  //
  //         DestroyContext(context);
  //
  // return result;
  //     }

  // static bool Import()
  // {
  //     BISContext context = CreateContext();
  //     if (!context)
  //         return false;
  //
  //     OpenFileDialog^ u_importFileDialog;
  //     u_importFileDialog = gcnew OpenFileDialog();
  //     u_importFileDialog->DefaultExt = "txt";
  //     u_importFileDialog->Filter = "Text file|*.txt|All files|*.*";
  //     u_importFileDialog->FileName = "";
  //     u_importFileDialog->ShowDialog();
  //     String^ filename = u_importFileDialog->FileName;
  //     pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
  //     bool result = false;
  //
  //     if (filename->Length > 0 && filename->Length < BISMaxStringLength - 1)
  //     {
  //         result = CallImport(context, wchptr);
  //
  //         if (result)
  //         {
  //     		::MessageBox(0, L"Import done", L"Service UI", MB_OK |
  //     MB_ICONINFORMATION | MB_SYSTEMMODAL);
  //         }
  //         else
  //     	{
  //             ::MessageBox(0, L"Import error", L"Service UI", MB_OK |
  //             MB_ICONINFORMATION | MB_SYSTEMMODAL);
  //         }
  //     }
  //
  //     DestroyContext(context);
  //
  //     return result;
  // }

  static Process ^
      GetRunningInstance() {
        Process ^ current = gcnew Process();
        current = Process::GetCurrentProcess();
        array<Process ^> ^ processes =
            Process::GetProcessesByName(current->ProcessName);
        int i;
        for (i = 0; i < processes->Length; i++) {
          Process ^ process = processes[i];
          if (process->Id != current->Id)
            if (Assembly::GetExecutingAssembly()->Location->Replace(
                    "/", "\\") == current->MainModule->FileName)
              return process;
        }
        return nullptr;
      }

      // static void CustomerCare(String^ userInfoFilePath, String^
      // primaryInputMethod)
      //{
      //	FormCustomerCare^ form = gcnew
      //FormCustomerCare(userInfoFilePath, primaryInputMethod);
      //	Application::Run(form);
      // }

      static void Download(String ^ sourceURL, String ^ sourceFile,
                           String ^ sigURL, String ^ sigFile,
                           String ^ changeLogURL, String ^ hide) {
    // ask user if s/he wants to download the update
    // if yes, download sourceURL -> sourceFile, sigURL -> sigFile
    // and valide with ValidateFile, or use the one in ManagedConnector if you
    // like download done, ask user if s/he wants to run it now (and end
    // ServiceUI) if yes, run it with msiexec /i (anyway package.msi will do all
    // the clean-up stuff) if no, schedule that in RunOnce
    if (hide == L"false") {
      UpdateHelper::DownloadUpdate(sourceURL, sourceFile, sigURL, sigFile,
                                   changeLogURL, false);
    } else {
      UpdateHelper::DownloadUpdate(sourceURL, sourceFile, sigURL, sigFile,
                                   changeLogURL, true);
    }
  }
};

RPCClientHelper::ServerHandle BaseIMEService::c_RPCServerInterfaceHandle = 0;

bool ManagedPreferenceConnector::validateFile(String ^ packageFile,
                                              String ^ signatureFile) {
  return BaseIMEService::Validate(packageFile, signatureFile);
}

void ManagedPreferenceConnector::closePreferencesPanel() {
  return BaseIMEService::ClosePreferencesPanel();
}

[STAThreadAttribute]

    void
    showTopMost(HWND h) {
      SetWindowPos(h, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
      SetForegroundWindow(h);
    }

void showForm(Form ^ form) {
  form->FormBorderStyle = FormBorderStyle::None;
  form->StartPosition = FormStartPosition::Manual;
  form->TopMost = true;
  form->ClientSize = *(gcnew Size(0, 0));
  form->Top = 0;
  form->Left = 0;
  form->ShowInTaskbar = false;
  form->MaximizeBox = false;
  form->MinimizeBox = false;
  form->ShowIcon = false;
  Region ^ region = gcnew Region(*(gcnew RectangleF(0, 0, 0, 0)));
  form->Region = region;

  showTopMost((HWND)form->Handle.ToPointer());
}

[STAThreadAttribute] int main(array<System::String ^> ^ args) {
  // We don't do anything when there is no args.
  if (args->Length == 0) return 0;

  PreferenceConnector::SharedInstance = gcnew ManagedPreferenceConnector();

  Process ^ instance = BaseIMEService::GetRunningInstance();

  // Enabling Windows XP visual effects before any controls are created
  if (instance == nullptr) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
  } else {
    showTopMost((HWND)instance->MainWindowHandle.ToPointer());
    System::Media::SystemSounds::Beep->Play();
    return 1;
  }

  if (!BaseIMEService::PrepareRPC()) {
    return 1;
  }

  Form ^ u_form = gcnew Form();

  if (args->Length > 0) {
    // if (args[0] == "export") {
    // 	showForm(u_form);
    // 	BaseIMEService::Export();
    // }
    // else if (args[0] == "import") {
    // 	showForm(u_form);
    // 	BaseIMEService::Import();
    // }
    // else
    // if (args[0] == "customercare" && args->Length >= 3) {
    //	showForm(u_form);
    //	BaseIMEService::CustomerCare(args[1], args[2]);
    //}
    if (args[0] == "download" && args->Length >= 5) {
      showForm(u_form);
      if (args->Length > 6)
        BaseIMEService::Download(args[1], args[2], args[3], args[4], args[5],
                                 L"false");
      else
        BaseIMEService::Download(args[1], args[2], args[3], args[4], args[5],
                                 L"true");
    }
  }

  if (u_form != nullptr) u_form->Close();

  BaseIMEService::CloseRPC();
  Application::Exit();

  return 0;
}
