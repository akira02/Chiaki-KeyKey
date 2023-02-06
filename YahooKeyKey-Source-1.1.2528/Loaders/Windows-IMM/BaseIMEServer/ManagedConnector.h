//
// ManagedConnector.h
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

#ifndef BIManagedConnector_h
#define BIManagedConnector_h

#pragma managed(push, on)

namespace BaseIMEServer {
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace BaseIMEUI;

ref class ServerLock {
 public:
  static Object ^ SharedLock = nullptr;
};

ref class ManagedConnector : public BIServerConnector {
 public:
  virtual void sendOneKeyChar() override;
  virtual void sendChar(wchar_t c) override;
  virtual void gotoPrevPage() override;
  virtual void gotoNextPage() override;
  virtual void sendString(String ^ text) override;
  virtual String ^ primaryInputMethod() override;
  virtual void setPrimaryInputMethod(String ^ moduleIdentifier,
                                     bool sendFakeKeyToCleanUpState) override;
  virtual bool isAlphanumericMode() override;
  virtual void setAlphanumericMode(bool mode) override;
  virtual bool isFullWidthCharacterMode() override;
  virtual void setFullWidthCharacterMode(bool mode) override;
  virtual bool isOutputFilterEnabled(String ^ moduleIdentifier) override;
  virtual void toggleOutputFilter(String ^ moduleIdentifier) override;
  virtual bool isAroundFilterEnabled(String ^ moduleIdentifier) override;
  virtual void toggleAroundFilter(String ^ moduleIdentifier) override;
  virtual Dictionary<String ^, String ^> ^
      allInputMethodIdentifiersAndNames() override;
  virtual Dictionary<String ^, String ^> ^
      allOutputFilterIdentifiersAndNames() override;
  virtual Dictionary<String ^, String ^> ^
      allAroundFilterIdentifiersAndNames() override;
  virtual bool hasLoaderConfigKey(String ^ key) override;
  virtual bool isLoaderConfigKeyTrue(String ^ key) override;
  virtual void setLoaderConfigKeyStringValue(String ^ key,
                                             String ^ value) override;
  virtual String ^ stringValueForLoaderConfigKey(String ^ key) override;
  virtual List<String ^> ^ arrayValueForLoaderConfigKey(String ^ key) override;
  virtual void syncLoaderConfig() override;

  virtual String ^
      stringValueForConfigKeyOfModule(String ^ key, String ^ moduleID) override;
  virtual void forceSyncModuleConfig(String ^ moduleID) override;
  virtual void deleteAllKeysAndValuesInModuleConfig(String ^ moduleID) override;

  virtual bool moduleWithWildcardNameExists(String ^ wildcardName) override;

  virtual bool isRunningUnderWow64() override;
  virtual String ^ databaseVersion() override;

  virtual void cancelPunctuationKeyboardWaitingState() override;

  virtual List<String ^> ^ cannedMessageCategories() override;
  virtual List<KeyValuePair<String ^, String ^> > ^
      cannedMessagesOfCategory(int index) override;

  virtual List<String ^> ^ symbolButtonCategories() override;
  virtual List<String ^> ^ symbolButtonsOfCategory(int index) override;

  virtual void startUpdateServerDataThread() override;

 protected:
  virtual const string localizedStringForPlistValue(PVPlistValue* dict,
                                                    const string& locale);
};

ref class ManagedPreferenceConnector
    : public TakaoPreference::PreferenceConnector {
 public:
  virtual void syncLoaderAndCurrentModuleConfig() override;

  virtual String ^ versionInfoURL() override;
  virtual String ^ versionInfoSignatureURL() override;

  virtual String ^ userInfoForPOST() override;
  virtual String ^ temporaryFilename(String ^ filename) override;
  virtual bool validateFile(String ^ packageFile,
                            String ^ signatureFile) override;
  virtual bool shouldUpdate(String ^ XMLFilename) override;
  virtual String ^ updateAction() override;
  virtual String ^ actionURL() override;
  virtual String ^ signatureURL() override;
  virtual String ^ localeTaggedChangeLogURL() override;
  virtual String ^ ManagedPreferenceConnector::primaryInputMethod() override;

  virtual bool isRunningUnderWow64() override;
  virtual void closePreferencesPanel() override;

  virtual String ^ userFreeCannedMessagePath() override;
};
};  // namespace BaseIMEServer

#pragma managed(pop)

#endif