//
// BaseIMEServer.h
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

#ifndef BaseIMEServer_h
#define BaseIMEServer_h

#pragma managed(push, off)

#ifndef OV_USE_SQLITE
#define OV_USE_SQLITE
#endif

#include <CareService.h>
#include <LFPlatform.h>
#include <OpenVanilla.h>
#include <PlainVanilla.h>

#include "BIConfig.h"
#include "OVLoaderUserPersistence.h"
#include "RemoteDataFetcherDelegate.h"
#include "WVEncodingService.h"
#include "WVLoaderPolicy.h"
#include "WVLogEmitter.h"
#include "YKSignedModuleLoadingSystem.h"

namespace BaseIMEServer {
using namespace std;
using namespace OpenVanilla;
using namespace LFPlatform;
using namespace CareService;
using namespace WindowsVanilla;

class CannedMessageFetcherDelegate;
class OneKeyDataFetcherDelegate;
class WVLoaderHookedServiceDelegate;
class WVLoaderCallback;

class IMEServer {
 public:
  static bool Start(const string& locale = "en");
  static bool StartRPCService();
  static void Stop();

  static IMEServer* SharedIMEServer() { return c_sharedIMEServer; }

  static PVLoader* SharedLoader() {
    return c_sharedIMEServer ? c_sharedIMEServer->m_loader : 0;
  }

  static PVLoaderService* SharedLoaderService() {
    return c_sharedIMEServer ? c_sharedIMEServer->m_loaderService : 0;
  }

  static PVLoaderPolicy* SharedLoaderPolicy() {
    return c_sharedIMEServer ? c_sharedIMEServer->m_loaderPolicy : 0;
  }

  static VersionChecker* SharedVersionChecker() {
    return c_sharedIMEServer ? c_sharedIMEServer->m_versionChecker : 0;
  }

 public:
  // service for the RPC endpoint
  void setCurrentClientRPCEndpointName(const string& endpointName);
  const string currentClientRPCEndpointName() const;
  const string nextAvailableClientRPCEndpointName();

  PVLoaderContext* activeContext();
  void setActiveContext(PVLoaderContext* context);

  void sendStringToClient(const string& text);
  const string directTextToClient();
  void clearDirectTextToClient();

  void sendCharToClient(char c);
  char directCharToClient();
  void clearDirectCharToClient();

 public:
  // service for ManagedConnector / BIServerConnector
  bool isAlphanumericMode();
  void setAlphanumericMode(bool mode);
  bool isFullWidthCharacterMode();
  void setFullWidthCharacterMode(bool mode);

  const string fullWidthCharacterFilterIdentifier();
  const string fullWidthCharacterStringIfActivated(const string& text);

 public:
  void reload();

  // two-stage reload
  void shutdownServerForDatabaseUpdate();
  void restartServerAndCompleteDatabaseUpdate();

  bool removeSignedModuleAndRestart(const string& packageName);
  void removeFirstSignedModuleInList();

  void stopUsingSignedModules();
  void resumeUsingSignedModules();

  void unregisterVendorSignedModuleVersionInfo();
  void registerVendorSignedModuleVersionInfo();

  size_t numberOfUnloadableSignedModules();
  const string localizedNameOfUnloadableSignedModulesAtIndex(size_t index);
  bool unloadSignedModuleAtIndex(size_t index);

 protected:
  const string serverURLWithDefaultValueAndConfigurableKey(
      const string& defaultValue, const string& overrideKeyName);

 public:
  const string versionInfoURL();
  const string versionInfoSignatureURL();

  const string serverOneKeyDataEndpointURL();
  const string serverCannedMessagesEndpointURL();

 protected:
  static IMEServer* c_sharedIMEServer;

  IMEServer(const string& locale = "en");
  ~IMEServer();
  void createDatabaseServices();
  bool start();
  void performClientRPCRequest(Logger* logger);

 protected:
  bool m_alphanumericMode;
  bool m_fullWidthCharacterMode;

 protected:
  string m_programPath;
  string m_locale;

  WVLoaderPolicy* m_loaderPolicy;
  WVLogEmitter* m_logEmitter;
  WVEncodingService* m_encodingService;

  OVCINDatabaseService* m_CINDatabaseService;
  OVSQLiteDatabaseService* m_SQLiteDatabaseService;

  PVLoaderService* m_loaderService;
  YKSignedModuleLoadingSystem* m_signedModuleLoadingSystem;
  PVStaticModulePackageLoadingSystem* m_staticModulePackageLoadingSystem;

  PVLoader* m_loader;

 protected:
  VersionChecker* m_versionChecker;

 protected:
  PVLoaderContext* m_activeContext;
  size_t m_clientRPCInstanceCount;
  string m_currentClientRPCEndpointName;
  string m_fullWidthCharacterFilterIdentifier;
  string m_directTextToClient;
  char m_directCharToClient;

 protected:
  string fetchDatabaseVersionInfo(OVSQLiteConnection* connection,
                                  const string& dbAndTableName);

 public:
  const string loaderComponentName();
  const string databaseComponentName();
  const string vendorModuleComponentName();
  const string databaseVersion();

 protected:
  void loadStaticModules(OVPathInfo& pathInfo);

  // for version update
  string m_loaderComponentName;
  string m_databaseComponentName;
  string m_databaseVersion;

 public:
  bool isWaitingForPunctuationKeyboardKey();
  void setWaitingForPunctuationKeyboardKey(bool waiting);

 protected:
  bool m_waitingForPunctuationKeyboardKey;

 public:
  const string lookupPunctuationKeyboardSymbol(char key);

 protected:
  map<char, wchar_t> m_punctuationKeyboardMap;

 protected:
  string m_lastCommittedText;

 public:
  const string lastCommittedText();
  void setLastCommittedText(const string& text);

 public:
  CRITICAL_SECTION m_win32CriticalSection;

 protected:
  friend class CannedMessageFetcherDelegate;

  CannedMessageFetcherDelegate* m_cannedMessageFetchDelegate;
  PVPlistValue* m_cannedMessagePlistValue;
  PVPropertyList* m_userCannedMessagePlist;
  PVPlistValue* m_mergedCannedMessagePlistValue;  // dictionary
  PVPlistValue* m_mergedButtonSymbolPlistValue;   // array
  OVFileTimestamp m_userFreeCannedMessageFileTimestamp;

  void mergeCannedMessage();

 public:
  const string userFreeCannedMessagePath();
  const string userCannedMessagePlistPath();

  RemoteDataFetcherDelegate* cannedMessageFetchDelegate();
  void syncUserCannedMessages();
  PVPlistValue* cannedMessagePlistValueCopy();
  PVPlistValue* buttonSymbolPlistValueCopy();

 protected:
  friend class OneKeyDataFetcherDelegate;
  OneKeyDataFetcherDelegate* m_oneKeyDataFetchDelegate;
  PVPlistValue* m_oneKeyPlistValue;
  PVPlistValue* m_mergedOneKeyPlistValue;
  PVPropertyList* m_userOneKeyPlist;
  void mergeOneKeyData();

 public:
  const string userOneKeyPlistPath();
  RemoteDataFetcherDelegate* oneKeyDataFetchDelegate();
  void syncUserOneKeyData();

 protected:
  friend class WVLoaderHookedServiceDelegate;
  WVLoaderHookedServiceDelegate* m_hookedServiceDelegate;

 protected:
  OVSQLiteConnection* m_userPhraseDB;
  const string qstringFromReading(const string& reading);
  bool userPhraseDBCheckConnection();

 public:
  bool userPhraseDBCanProvideService();
  int userPhraseDBNumberOfRow();
  const string userPhraseDBDataAtRow(int row);
  const string userPhraseDBReadingsForPhrase(const string& phrase);
  void userPhraseDBSave();
  void userPhraseDBSetNewReadingForPhraseAtRow(const string& reading, int row);
  void userPhraseDBDeleteRow(int row);
  void userPhraseDBAddNewRow(const string& phrase);
  void userPhraseDBSetPhrase(const string& phrase, int row);

 protected:
  friend class WVLoaderCallback;
  WVLoaderCallback* m_loaderCallback;

 protected:
  OVLoaderUserPersistence* m_userPersistence;

 public:
  void setStopUsingServerSideUI(bool stopUsing = false);
  bool serverSideUINotUsed();

 protected:
  bool m_stopUsingServerSideUI;
};
};  // namespace BaseIMEServer

#pragma managed(pop)

#endif