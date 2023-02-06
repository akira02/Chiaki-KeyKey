//
// ManagedConnector.cpp
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

#include "ManagedConnector.h"

#include <vcclr.h>

#include "BPMFUserPhraseHelper.h"
#include "BaseIMEServer.h"
#include "Minotaur.h"
#include "RemoteDataFetcher.h"

#ifndef BISMaxStringLength
#define BISMaxStringLength 4096
#endif

#pragma managed

namespace BaseIMEServer {

using namespace System;
using namespace Manjusri;
using namespace Minotaur;
using namespace WindowsVanilla;

void ManagedConnector::sendOneKeyChar() {
  PVPlistValue* dict =
      IMEServer::SharedLoader()->configDictionaryForModule("OneKey");
  if (dict) {
    PVPlistValue* shortcut = dict->valueForKey("ShortcutKey");
    if (shortcut) {
      string sv = shortcut->stringValue();
      if (sv.size()) {
        IMEServer::SharedIMEServer()->sendCharToClient((char)sv[0]);
      } else {
        sendChar('`');
      }
    } else {
      sendChar('`');
    }
  }
}

void ManagedConnector::sendChar(wchar_t c) {
  IMEServer::SharedIMEServer()->sendCharToClient((char)c);
}

void ManagedConnector::gotoPrevPage() { sendChar(OVKeyCode::PageUp); }

void ManagedConnector::gotoNextPage() { sendChar(OVKeyCode::PageDown); }

void ManagedConnector::sendString(String ^ text) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(text);
  IMEServer::SharedIMEServer()->sendStringToClient(OVUTF8::FromUTF16(wchptr));
}

String ^ ManagedConnector::primaryInputMethod() {
  wstring im =
      OVUTF16::FromUTF8(IMEServer::SharedLoader()->primaryInputMethod());
  return gcnew String(im.c_str());
}

void ManagedConnector::setPrimaryInputMethod(String ^ moduleIdentifier,
                                             bool sendFakeKeyToCleanUpState) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
  wstring wstr = wchptr;

  wstring im =
      OVUTF16::FromUTF8(IMEServer::SharedLoader()->primaryInputMethod());
  if (wstr != im) {
    IMEServer::SharedLoader()->setPrimaryInputMethod(OVUTF8::FromUTF16(wstr));
    IMEServer::SharedLoader()->syncSandwichConfig();

    if (sendFakeKeyToCleanUpState) {
      IMEServer::SharedIMEServer()->sendStringToClient("");
    }
  }
}

bool ManagedConnector::isAlphanumericMode() {
  return IMEServer::SharedIMEServer()->isAlphanumericMode();
}

void ManagedConnector::setAlphanumericMode(bool mode) {
  IMEServer::SharedIMEServer()->setAlphanumericMode(mode);
}

bool ManagedConnector::isFullWidthCharacterMode() {
  return IMEServer::SharedIMEServer()->isFullWidthCharacterMode();
}

void ManagedConnector::setFullWidthCharacterMode(bool mode) {
  IMEServer::SharedIMEServer()->setFullWidthCharacterMode(mode);
}

bool ManagedConnector::isOutputFilterEnabled(String ^ moduleIdentifier) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
  wstring wstr = wchptr;
  return IMEServer::SharedLoader()->isOutputFilterActivated(
      OVUTF8::FromUTF16(wstr));
}

void ManagedConnector::toggleOutputFilter(String ^ moduleIdentifier) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
  wstring wstr = wchptr;
  IMEServer::SharedLoader()->toggleOutputFilter(OVUTF8::FromUTF16(wstr));
}

bool ManagedConnector::isAroundFilterEnabled(String ^ moduleIdentifier) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
  wstring wstr = wchptr;
  return IMEServer::SharedLoader()->isAroundFilterActivated(
      OVUTF8::FromUTF16(wstr));
}

void ManagedConnector::toggleAroundFilter(String ^ moduleIdentifier) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleIdentifier);
  wstring wstr = wchptr;
  IMEServer::SharedLoader()->toggleAroundFilter(OVUTF8::FromUTF16(wstr));
  IMEServer::SharedIMEServer()->sendStringToClient("");
}

Dictionary<String ^, String ^> ^
    ManagedConnector::allInputMethodIdentifiersAndNames() {
  vector<pair<string, string> > inputMethods;
  inputMethods = IMEServer::SharedLoader()->allInputMethodIdentifiersAndNames();
  size_t size, i;
  Dictionary<String ^, String ^> ^ dictionary =
      gcnew Dictionary<String ^, String ^>;
  size = inputMethods.size();
  for (i = 0; i < size; i++) {
    std::pair<string, string> inputMethod = inputMethods[i];
    string id = inputMethod.first;
    string name = inputMethod.second;
    if (id == "TraditionalMandarin" || id == "SmartMandarin" ||
        id == "Generic-cj-cin" || id == "Generic-simplex-cin")
      continue;
    dictionary->Add(gcnew String(id.c_str()),
                    gcnew String(OVUTF16::FromUTF8(name).c_str()));
  }
  return dictionary;
}

Dictionary<String ^, String ^> ^
    ManagedConnector::allOutputFilterIdentifiersAndNames() {
  vector<pair<string, string> > outputFilters;
  outputFilters =
      IMEServer::SharedLoader()->allOutputFilterIdentifiersAndNames();
  size_t size, i;
  Dictionary<String ^, String ^> ^ dictionary =
      gcnew Dictionary<String ^, String ^>;
  size = outputFilters.size();
  for (i = 0; i < size; i++) {
    std::pair<string, string> outputFilter = outputFilters[i];
    string id = outputFilter.first;
    string name = outputFilter.second;

    // @todo: change this logic
    if (id == "FullWidthCharacter" || id == "ChineseCharacterConvertor-TC2SC")
      continue;
    dictionary->Add(gcnew String(id.c_str()),
                    gcnew String(OVUTF16::FromUTF8(name).c_str()));
  }
  return dictionary;
}

Dictionary<String ^, String ^> ^
    ManagedConnector::allAroundFilterIdentifiersAndNames() {
  vector<pair<string, string> > aroundFilters;
  aroundFilters =
      IMEServer::SharedLoader()->allAroundFilterIdentifiersAndNames();
  size_t size, i;
  Dictionary<String ^, String ^> ^ dictionary =
      gcnew Dictionary<String ^, String ^>;
  size = aroundFilters.size();
  for (i = 0; i < size; i++) {
    std::pair<string, string> aroundFilter = aroundFilters[i];
    string id = aroundFilter.first;
    string name = aroundFilter.second;
    dictionary->Add(gcnew String(id.c_str()),
                    gcnew String(OVUTF16::FromUTF8(name).c_str()));
  }
  return dictionary;
}

bool ManagedConnector::hasLoaderConfigKey(String ^ key) {
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(key);
  wstring wkey = wchptr;
  return kvm.hasKey(OVUTF8::FromUTF16(wkey));
}

bool ManagedConnector::isLoaderConfigKeyTrue(String ^ key) {
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(key);
  wstring wkey = wchptr;
  return kvm.isKeyTrue(OVUTF8::FromUTF16(wkey));
}

void ManagedConnector::setLoaderConfigKeyStringValue(String ^ key,
                                                     String ^ value) {
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(key);
  wstring wkey = wchptr;
  wchptr = PtrToStringChars(value);
  wstring wvalue = wchptr;
  kvm.setKeyStringValue(OVUTF8::FromUTF16(wkey), OVUTF8::FromUTF16(wvalue));
  IMEServer::SharedLoader()->saveChangedRootDictionary();
}

String ^ ManagedConnector::stringValueForLoaderConfigKey(String ^ key) {
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(key);
  wstring wkey = wchptr;

  wstring wcstr =
      OVUTF16::FromUTF8(kvm.stringValueForKey(OVUTF8::FromUTF16(wkey)));
  return gcnew String(wcstr.c_str());
}

String ^ ManagedConnector::stringValueForConfigKeyOfModule(String ^ key,
                                                           String ^ moduleID) {
  pin_ptr<const wchar_t> wchptr1 = PtrToStringChars(key);
  pin_ptr<const wchar_t> wchptr2 = PtrToStringChars(moduleID);
  wstring wkey = wchptr1;
  wstring wmoduleID = wchptr2;

  PVPlistValue* pv = IMEServer::SharedLoader()->configDictionaryForModule(
      OVUTF8::FromUTF16(wmoduleID));
  if (!pv) {
    return gcnew String("");
  }

  PVPlistValue* v = pv->valueForKey(OVUTF8::FromUTF16(wkey));
  if (!v) {
    return gcnew String("");
  }

  return gcnew String(OVUTF16::FromUTF8(v->stringValue()).c_str());
}

void ManagedConnector::forceSyncModuleConfig(String ^ moduleID) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleID);
  wstring wmoduleID = wchptr;
  IMEServer::SharedLoader()->forceWriteModuleConfig(
      OVUTF8::FromUTF16(wmoduleID));
}

void ManagedConnector::deleteAllKeysAndValuesInModuleConfig(String ^ moduleID) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(moduleID);
  wstring wmoduleID = wchptr;
  PVPlistValue* pv = IMEServer::SharedLoader()->configDictionaryForModule(
      OVUTF8::FromUTF16(wmoduleID));
  if (pv) {
    pv->removeAllKeysAndValues();
  }
}

List<String ^> ^ ManagedConnector::arrayValueForLoaderConfigKey(String ^ key) {
  List<String ^> ^ array = gcnew List<String ^>();
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(key);
  string u8key = OVUTF8::FromUTF16(wchptr);

  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  PVPlistValue* arrayValue =
      IMEServer::SharedLoader()->configRootDictionary()->valueForKey(u8key);
  if (arrayValue) {
    if (arrayValue->type() == PVPlistValue::Array) {
      size_t size = arrayValue->arraySize();
      for (size_t i = 0; i < size; i++) {
        PVPlistValue* stringValue = arrayValue->arrayElementAtIndex(i);
        if (stringValue) {
          array->Add(gcnew String(
              OVUTF16::FromUTF8(stringValue->stringValue()).c_str()));
        }
      }
    }
  }

  return array;
}

void ManagedConnector::syncLoaderConfig() {
  IMEServer::SharedLoader()->syncLoaderConfig();
}

bool ManagedConnector::moduleWithWildcardNameExists(String ^ wildcardName) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(wildcardName);
  OVWildcard exp(OVUTF8::FromUTF16(wchptr));

  vector<pair<string, string> > mods =
      IMEServer::SharedLoader()->allModuleIdentifiersAndNames();
  for (vector<pair<string, string> >::iterator mi = mods.begin();
       mi != mods.end(); ++mi) {
    if (exp.match((*mi).first)) {
      return true;
    }
  }

  return false;
}

bool ManagedConnector::isRunningUnderWow64() {
  return ProcessHelper::IsRunningUnderWow64();
}

String ^ ManagedConnector::databaseVersion() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->databaseVersion())
          .c_str());
}

void ManagedConnector::cancelPunctuationKeyboardWaitingState() {
  IMEServer::SharedIMEServer()->setWaitingForPunctuationKeyboardKey(false);
}

List<String ^> ^ ManagedConnector::cannedMessageCategories() {
  string locale = IMEServer::SharedLoaderService()->locale();
  PVPlistValue* cmsg =
      IMEServer::SharedIMEServer()->cannedMessagePlistValueCopy();
  PVPlistValue* msgs = cmsg->valueForKey("CannedMessages");
  List<String ^> ^ result = gcnew List<String ^>();

  if (msgs) {
    for (size_t i = 0; i < msgs->arraySize(); i++) {
      PVPlistValue* category = msgs->arrayElementAtIndex(i);
      PVPlistValue* name = category->valueForKey("Name");

      string nameStr = localizedStringForPlistValue(name, locale);
      result->Add(gcnew String(OVUTF16::FromUTF8(nameStr).c_str()));
    }
  }

  delete cmsg;
  return result;
}

List<KeyValuePair<String ^, String ^> > ^
    ManagedConnector::cannedMessagesOfCategory(int index) {
  string locale = IMEServer::SharedLoaderService()->locale();
  List<KeyValuePair<String ^, String ^> > ^ result =
      gcnew List<KeyValuePair<String ^, String ^> >();

  PVPlistValue* cmsg =
      IMEServer::SharedIMEServer()->cannedMessagePlistValueCopy();
  PVPlistValue* msgs = cmsg->valueForKey("CannedMessages");

  if (!msgs) {
    delete cmsg;
    return result;
  }

  size_t stidx = (size_t)index;
  if (stidx < msgs->arraySize()) {
    PVPlistValue* category = msgs->arrayElementAtIndex(stidx);
    PVPlistValue* texts = category->valueForKey("Messages");
    size_t ts = texts->arraySize();
    for (size_t i = 0; i < ts; i++) {
      PVPlistValue* m = texts->arrayElementAtIndex(i);

      string key, value;

      if (m->type() == PVPlistValue::String) {
        key = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
            m->stringValue(), "\\n", " ");
        value = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
            m->stringValue(), "\\n", "\r");
      } else if (m->type() == PVPlistValue::Dictionary) {
        key = localizedStringForPlistValue(m->valueForKey("Name"), locale);
        value = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
            localizedStringForPlistValue(m->valueForKey("Text"), locale), "\\n",
            "\r");
      }

      if (OVUTF16::FromUTF8(value).length() < 256) {
        KeyValuePair<String ^, String ^> nkvp(
            gcnew String(OVUTF16::FromUTF8(key).c_str()),
            gcnew String(OVUTF16::FromUTF8(value).c_str()));
        result->Add(nkvp);
      }
    }
  }

  delete cmsg;
  return result;
}

List<String ^> ^ ManagedConnector::symbolButtonCategories() {
  string locale = IMEServer::SharedLoaderService()->locale();
  PVPlistValue* btns =
      IMEServer::SharedIMEServer()->buttonSymbolPlistValueCopy();
  List<String ^> ^ result = gcnew List<String ^>();

  for (size_t i = 0; i < btns->arraySize(); i++) {
    PVPlistValue* category = btns->arrayElementAtIndex(i);
    string name =
        localizedStringForPlistValue(category->valueForKey("Name"), locale);
    result->Add(gcnew String(OVUTF16::FromUTF8(name).c_str()));
  }

  delete btns;
  return result;
}

List<String ^> ^ ManagedConnector::symbolButtonsOfCategory(int index) {
  List<String ^> ^ result = gcnew List<String ^>();
  PVPlistValue* btns =
      IMEServer::SharedIMEServer()->buttonSymbolPlistValueCopy();

  if ((size_t)index < btns->arraySize()) {
    PVPlistValue* category = btns->arrayElementAtIndex(index);
    PVPlistValue* buttons = category->valueForKey("Buttons");
    size_t ts = buttons->arraySize();
    for (size_t i = 0; i < ts; i++) {
      PVPlistValue* m = buttons->arrayElementAtIndex(i);
      result->Add(gcnew String(OVUTF16::FromUTF8(m->stringValue()).c_str()));
    }
  }

  return result;
}

void ManagedConnector::startUpdateServerDataThread() {
  RemoteDataFetcher::SharedInstance->fetch(
      IMEServer::SharedIMEServer()->serverCannedMessagesEndpointURL(),
      IMEServer::SharedIMEServer()->cannedMessageFetchDelegate());
  RemoteDataFetcher::SharedInstance->fetch(
      IMEServer::SharedIMEServer()->serverOneKeyDataEndpointURL(),
      IMEServer::SharedIMEServer()->oneKeyDataFetchDelegate());
}

const string ManagedConnector::localizedStringForPlistValue(
    PVPlistValue* dict, const string& locale) {
  if (!dict) {
    return string();
  }

  if (dict->type() == PVPlistValue::String) {
    return dict->stringValue();
  }

  PVPlistValue* value = dict->valueForKey(locale);
  if (!value) {
    value = dict->valueForKey("en");
  }

  if (!value) {
    vector<string> keys = dict->dictionaryKeys();
    if (keys.size()) {
      value = dict->valueForKey(keys[0]);
    }
  }

  if (!value) {
    return string();
  }

  if (value->type() != PVPlistValue::String) {
    return string();
  }

  return value->stringValue();
}

// @todo in deployment, make this better
extern "C" {
extern char VendorMotcle[];
extern size_t VendorMotcleSize;
};

void ManagedPreferenceConnector::syncLoaderAndCurrentModuleConfig() {
  // refresh the config
  IMEServer::SharedLoader()->syncLoaderConfig();
  IMEServer::SharedLoader()->syncSandwichConfig();
}

String ^ ManagedPreferenceConnector::versionInfoURL() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->versionInfoURL())
          .c_str());
}

String ^ ManagedPreferenceConnector::versionInfoSignatureURL() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->versionInfoSignatureURL())
          .c_str());
}

String ^ ManagedPreferenceConnector::userInfoForPOST() {
  stringstream sst;
  PVPlistValue* allPlists =
      IMEServer::SharedLoader()->loaderAndModulePropertyListsCombined();
  sst << *allPlists << endl;
  delete allPlists;

  if (sst.str().length() >= BISMaxStringLength) {
    return gcnew String(
        OVUTF16::FromUTF8(sst.str().substr(0, BISMaxStringLength - 1)).c_str());
  }

  return gcnew String(OVUTF16::FromUTF8(sst.str()).c_str());
}

String ^ ManagedPreferenceConnector::temporaryFilename(String ^ filename) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
  string ufn = OVUTF8::FromUTF16(wchptr);
  string result =
      OVPathHelper::PathCat(OVDirectoryHelper::TempDirectory(), ufn);
  return gcnew String(OVUTF16::FromUTF8(result).c_str());
}

bool ManagedPreferenceConnector::validateFile(String ^ packageFile,
                                              String ^ signatureFile) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(packageFile);
  string upf = OVUTF8::FromUTF16(wchptr);
  wchptr = PtrToStringChars(signatureFile);
  string usf = OVUTF8::FromUTF16(wchptr);

  pair<char*, size_t> sigfile = OVFileHelper::SlurpFile(usf);

  if (!sigfile.first) return false;

  bool valid = false;

  if (Minos::ValidateFile(upf, sigfile,
                          pair<char*, size_t>(VendorMotcle, VendorMotcleSize)))
    valid = true;

  free(sigfile.first);
  return valid;
}

bool ManagedPreferenceConnector::shouldUpdate(String ^ XMLFilename) {
  pin_ptr<const wchar_t> wchptr = PtrToStringChars(XMLFilename);
  string ufn = OVUTF8::FromUTF16(wchptr);

  string loaderComponentName =
      IMEServer::SharedIMEServer()->loaderComponentName();
  string databaseComponentName =
      IMEServer::SharedIMEServer()->databaseComponentName();
  string moduleComponentName =
      IMEServer::SharedIMEServer()->vendorModuleComponentName();

  if (!IMEServer::SharedVersionChecker()->loadVersionInfoXMLFile(ufn))
    return false;

  if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(
          loaderComponentName)) {
    // now all update info is about loader component
    return true;
  }

  if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(
          databaseComponentName)) {
    return true;
  }

  if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(
          moduleComponentName)) {
    return true;
  }

  // then if loader doesn't need updating, check if the database component does
  return false;
}

String ^ ManagedPreferenceConnector::updateAction() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->updateAction())
          .c_str());
}

String ^ ManagedPreferenceConnector::actionURL() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->actionURL())
          .c_str());
}

String ^ ManagedPreferenceConnector::signatureURL() {
  return gcnew String(
      OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->signatureURL())
          .c_str());
}

String ^ ManagedPreferenceConnector::primaryInputMethod() {
  wstring im =
      OVUTF16::FromUTF8(IMEServer::SharedLoader()->primaryInputMethod());
  return gcnew String(im.c_str());
}

String ^ ManagedPreferenceConnector::localeTaggedChangeLogURL() {
  string changeLogBaseURL =
      IMEServer::SharedVersionChecker()->changeLogBaseURL();
  string localeTag = IMEServer::SharedVersionChecker()->changeLogLocaleTagURL();
  string changeLogURL =
      OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
          changeLogBaseURL, localeTag,
          IMEServer::SharedLoaderService()->locale());
  wstring ws = OVUTF16::FromUTF8(changeLogURL);
  return gcnew String(ws.c_str());
}

bool ManagedPreferenceConnector::isRunningUnderWow64() {
  return ProcessHelper::IsRunningUnderWow64();
}

void ManagedPreferenceConnector::closePreferencesPanel() {
  // to be implemented in ServiceUI, with RPC calls
}

String ^ ManagedPreferenceConnector::userFreeCannedMessagePath() {
  return gcnew String(
      OVUTF16::FromUTF8(
          IMEServer::SharedIMEServer()->userFreeCannedMessagePath())
          .c_str());
}

};  // namespace BaseIMEServer
