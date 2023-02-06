// PreferencesMain.cpp : main project file.

#include "stdafx.h"

#pragma unmanaged
#include "BIConfig.h"
#include "LFPlatform.h"
#include "OpenVanilla.h"

#pragma managed
#include <vcclr.h>

#include "BIClientRPCInterface.h"
#include "BIServerRPCInterface.h"

using namespace OpenVanilla;
using namespace LFPlatform;
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Globalization;
using namespace System::Security::Permissions;
using namespace System::Threading;
using namespace TakaoPreference;
using namespace BaseIMEUI;

BISContext PMSharedContext = NULL;

ref class ManagedConnector : public BIServerConnector {
 public:
  virtual String ^ databaseVersion() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetDatabaseVersion,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ stringValueForConfigKeyOfModule(String ^ key,
                                                   String ^ moduleID) override {
    pin_ptr<const wchar_t> wchptr1 = PtrToStringChars(key);
    pin_ptr<const wchar_t> wchptr2 = PtrToStringChars(moduleID);

    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);

    RpcTryExcept BISStatelessFunctionGetStringWithStringAndString(
        BISFunctionStringValueForConfigKeyOfModule, output, wchptr1, wchptr2);
    RpcExcept(1) {}
    RpcEndExcept

        return gcnew String(output);
  }

  virtual void forceSyncModuleConfig(String ^ moduleID) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleID);
    RpcTryExcept BISStatelessFunctionSetString(BISFunctionForceSyncModuleConfig,
                                               wchptr);
    RpcExcept(1) {}
    RpcEndExcept
  }

  virtual void deleteAllKeysAndValuesInModuleConfig(String ^
                                                    moduleID) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleID);
    RpcTryExcept BISStatelessFunctionSetString(
        BISFunctionDeleteAllKeysAndValuesInModuleConfig, wchptr);
    RpcExcept(1) {}
    RpcEndExcept
  }

  virtual bool isAroundFilterEnabled(String ^ moduleIdentifier) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
    RpcTryExcept return !!BISStatelessFunctionGetBoolWithString(
        BISFunctionIsAroundFilterEnabled, wchptr);
    RpcExcept(1) { return false; }
    RpcEndExcept
  }

  virtual void toggleAroundFilter(String ^ moduleIdentifier) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
    RpcTryExcept BISStatelessFunctionSetString(BISFunctionToggleAroundFilter,
                                               wchptr);
    RpcExcept(1) {}
    RpcEndExcept
  }

  virtual bool moduleWithWildcardNameExists(String ^ wildcardName) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(wildcardName);
    RpcTryExcept return !!BISStatelessFunctionGetBoolWithString(
        BISFunctionModuleWithWildcardNameExists, wchptr);
    RpcExcept(1) { return false; }
    RpcEndExcept
  }
};

ref class ManagedPreferenceConnector : public PreferenceConnector {
 public:
  virtual void syncLoaderAndCurrentModuleConfig() override {
    RpcTryExcept BISStatelessFunctionVoid(
        BISFunctionSyncLoaderAndCurrentModuleConfig);
    RpcExcept(1) {}
    RpcEndExcept
  }

  virtual String ^ versionInfoURL() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetVersionInfoURL,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ versionInfoSignatureURL() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(
        BISFunctionGetVersionInfoSignatureURL, output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ userInfoForPOST() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetUserInfoForPOST,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ temporaryFilename(String ^ filename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetStringWithString(
        BISFunctionGetTemporaryFilename, output, wchptr);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual bool validateFile(String ^ packageFile,
                            String ^ signatureFile) override {
    pin_ptr<const wchar_t> wchptr1 = PtrToStringChars(packageFile);
    pin_ptr<const wchar_t> wchptr2 = PtrToStringChars(signatureFile);
    RpcTryExcept return !!BISStatelessFunctionGetBoolWithStringAndString(
        BISFunctionValidateFile, wchptr1, wchptr2);
    RpcExcept(1) { return false; }
    RpcEndExcept
  }

  virtual bool shouldUpdate(String ^ XMLFilename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(XMLFilename);
    RpcTryExcept return !!BISStatelessFunctionGetBoolWithString(
        BISFunctionShouldUpdate, wchptr);
    RpcExcept(1) { return false; }
    RpcEndExcept
  }
  virtual String ^ updateAction() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetUpdateAction,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ actionURL() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetActionURL, output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ signatureURL() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetSignatureURL,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ localeTaggedChangeLogURL() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(
        BISFunctionGetLocaleTaggedChangeLogURL, output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual String ^ ManagedPreferenceConnector::primaryInputMethod() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(BISFunctionGetPrimaryInputMethod,
                                               output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual void closePreferencesPanel() override {
    // TO DO: who is calling this? need to find out
  }

  virtual bool isRunningUnderWow64() override {
    return ProcessHelper::IsRunningUnderWow64();
  }

  virtual bool importPhraseDB(String ^ filename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
    RpcTryExcept return BISImportUserPhraseDB(PMSharedContext, wchptr) == TRUE;
    RpcExcept(1) { return FALSE; }
    RpcEndExcept
  }

  virtual bool exportPhraseDB(String ^ filename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
    RpcTryExcept return BISExportUserPhraseDB(PMSharedContext, wchptr) == TRUE;
    RpcExcept(1) { return FALSE; }
    RpcEndExcept
  }

  virtual int numberOfUnloadableSignedModules() override {
    RpcTryExcept return BISStatelessFunctionGetInt(
        BISFunctionGetNumberOfUnloadableSignedModules);
    RpcExcept(1) { return 0; }
    RpcEndExcept
  }

  virtual String ^
      localizedNameOfUnloadableSignedModulesAtIndex(int index) override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetStringWithInt(
        BISFunctionGetLocalizedNameOfUnloadableSignedModulesAtIndex, output,
        index);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  virtual bool unloadSignedModuleAtIndex(int index) override {
    RpcTryExcept return !!BISStatelessFunctionGetBoolWithInt(
        BISFunctionUnloadSignedModuleAtIndex, index);
    RpcExcept(1) { return false; }
    RpcEndExcept
  }

  virtual String ^ userFreeCannedMessagePath() override {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);
    RpcTryExcept BISStatelessFunctionGetString(
        BISFunctionGetUserFreeCannedMessagePath, output);
    RpcExcept(1) {}
    RpcEndExcept return gcnew String(output);
  }

  static ::TakaoPreference::TakaoPreference ^ c_preferenceAppInstance = nullptr;
};

void BICRequestSendFakeKey(void) {
  ManagedPreferenceConnector::c_preferenceAppInstance->Close();
}

bool OpenConnection() {
  RpcTryExcept if (!BISOpenConnection(&PMSharedContext)) { return false; }
  RpcExcept(1) { return false; }
  RpcEndExcept;

  return true;
}

void GetAllNonSystemGenericInputMethodsIDAndNames(wchar_t *output) {
  RpcTryExcept BISStatelessFunctionGetString(
      BISFunctionGetAllNonSystemGenericInputMethodsIDAndNames, output);
  RpcExcept(1) {}
  RpcEndExcept
}

void CloseConnection() {
  RpcTryExcept BISCloseConnection(&PMSharedContext);
  RpcExcept(1) {}
  RpcEndExcept
}

[STAThreadAttribute] int main(array<System::String ^> ^ args) {
  CreateMutex(0, 1, LPCWSTR("PreferenceMain"));
  if (GetLastError() == 183) {
    exit(-1);
  };

  RPCClientHelper::ServerHandle c_RPCServerInterfaceHandle;

  if (!(c_RPCServerInterfaceHandle = RPCClientHelper::ConnectServer(
            BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
    return 1;
  }

  if (!OpenConnection()) {
    return 1;
  }

  // create the client RPC endpoint
  if (!RPCServerHelper::StartService("YKPreferencesMainRPC",
                                     BIClientRPCInterface_v1_0_s_ifspec)) {
    return 1;
  }

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

  // create the connector
  BIServerConnector::SharedInstance = gcnew ManagedConnector();
  PreferenceConnector::SharedInstance = gcnew ManagedPreferenceConnector();

  // test, remove it in the next release
  // bool result = PreferenceConnector::SharedInstance->importPhraseDB(gcnew
  // String("c:\\export.list"));

  // Enabling Windows XP visual effects before any controls are created
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  wchar_t output[BISMaxStringLength];
  wmemset(output, 0, BISMaxStringLength);

  GetAllNonSystemGenericInputMethodsIDAndNames(output);

  string ims = wcslen(output) ? OVUTF8::FromUTF16(output) : string();
  vector<string> imsv = OVStringHelper::Split(ims, '\t');

  // Create the main window and run it
  Dictionary<String ^, String ^> ^ dict =
      gcnew Dictionary<String ^, String ^>();
  for (size_t i = 0; i < imsv.size() / 2; i++) {
    string imid = imsv[i * 2];
    string imname = imsv[i * 2 + 1];

    String ^ sid = gcnew String(OVUTF16::FromUTF8(imid).c_str());
    String ^ sname = gcnew String(OVUTF16::FromUTF8(imname).c_str());

    dict->Add(sid, sname);
  }

  ManagedPreferenceConnector::c_preferenceAppInstance =
      gcnew ::TakaoPreference::TakaoPreference(dict);
  Application::Run(ManagedPreferenceConnector::c_preferenceAppInstance);

  CloseConnection();

  RPCServerHelper::StopService();

  if (c_RPCServerInterfaceHandle)
    RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                &BIServerRPCInterfaceGlobalHandle);

  return 0;
}
