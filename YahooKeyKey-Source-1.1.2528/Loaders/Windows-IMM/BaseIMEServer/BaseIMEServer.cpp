//
// BaseIMEServer.cpp
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

#pragma unmanaged
#include "BaseIMEServer.h"

#include <CareService.h>
#include <StaticPack.h>

#include <sstream>

#include "ModuleDigestSharedSecret.h"

#pragma managed
#include "BIServerRPCInterface.h"

#pragma unmanaged
#include "BIClientRPCInterface.h"

#ifdef OVLOADER_USE_SQLITE_CRYPTO
void InitSQLiteCrypto(sqlite3* db);
string FetchSQLiteCERODKey(const string& filename);
#endif

#include <set>

#include "BPMFUserPhraseHelper.h"
#include "Mandarin.h"

namespace BaseIMEServer {

class ImeServerStringTable : public OVLocalizationStringTable {
 public:
  ImeServerStringTable() {
    "User Defined";

#ifndef _MSC_VER
    add("zh_TW", "User Defined", "我自定的罐頭訊息");
    add("zh_CN", "User Defined", "我自定的罐头讯息");

    add("en", "BOM-LINE",
        "\xef\xbb\xbf=== Add your own pre-defined texts after this line ===\n");
    add("zh_TW", "BOM-LINE",
        "\xef\xbb\xbf=== 請從本行以下加入自定訊息，一行一則，每行不超過 80 "
        "中文或英數字，並請保留這一行 ===\n");
    add("zh_CN", "BOM-LINE",
        "\xef\xbb\xbf=== 请从本行以下加入自定讯息，一行一则，每行不超过 80 "
        "中文或英数字，并请保留这一行 ===\n");

    add("en", "EXAMPLE-LINE", "Hello!\n\n");
    add("zh_TW", "EXAMPLE-LINE", "你好！\n\n");
    add("zh_CN", "EXAMPLE-LINE", "你好！\n\n");

#else
    add("zh_TW", "User Defined",
        "\xe6\x88\x91\xe8\x87\xaa\xe5\xae\x9a\xe7\x9a\x84\xe7\xbd\x90\xe9\xa0"
        "\xad\xe8\xa8\x8a\xe6\x81\xaf");
    add("zh_CN", "User Defined",
        "\xe6\x88\x91\xe8\x87\xaa\xe5\xae\x9a\xe7\x9a\x84\xe7\xbd\x90\xe5\xa4"
        "\xb4\xe8\xae\xaf\xe6\x81\xaf");

    add("en", "BOM-LINE",
        "\xef\xbb\xbf=== Add your own pre-defined texts after this line ===\n");
    add("zh_TW", "BOM-LINE",
        "\xef\xbb\xbf=== "
        "\xe8\xab\x8b\xe5\xbe\x9e\xe6\x9c\xac\xe8\xa1\x8c\xe4\xbb\xa5\xe4\xb8"
        "\x8b\xe5\x8a\xa0\xe5\x85\xa5\xe8\x87\xaa\xe5\xae\x9a\xe8\xa8\x8a\xe6"
        "\x81\xaf\xef\xbc\x8c\xe4\xb8\x80\xe8\xa1\x8c\xe4\xb8\x80\xe5\x89\x87"
        "\xef\xbc\x8c\xe6\xaf\x8f\xe8\xa1\x8c\xe4\xb8\x8d\xe8\xb6\x85\xe9\x81"
        "\x8e 80 "
        "\xe4\xb8\xad\xe6\x96\x87\xe6\x88\x96\xe8\x8b\xb1\xe6\x95\xb8\xe5\xad"
        "\x97\xef\xbc\x8c\xe4\xb8\xa6\xe8\xab\x8b\xe4\xbf\x9d\xe7\x95\x99\xe9"
        "\x80\x99\xe4\xb8\x80\xe8\xa1\x8c ===\n");
    add("zh_CN", "BOM-LINE",
        "\xef\xbb\xbf=== "
        "\xe8\xaf\xb7\xe4\xbb\x8e\xe6\x9c\xac\xe8\xa1\x8c\xe4\xbb\xa5\xe4\xb8"
        "\x8b\xe5\x8a\xa0\xe5\x85\xa5\xe8\x87\xaa\xe5\xae\x9a\xe8\xae\xaf\xe6"
        "\x81\xaf\xef\xbc\x8c\xe4\xb8\x80\xe8\xa1\x8c\xe4\xb8\x80\xe5\x88\x99"
        "\xef\xbc\x8c\xe6\xaf\x8f\xe8\xa1\x8c\xe4\xb8\x8d\xe8\xb6\x85\xe8\xbf"
        "\x87 80 "
        "\xe4\xb8\xad\xe6\x96\x87\xe6\x88\x96\xe8\x8b\xb1\xe6\x95\xb0\xe5\xad"
        "\x97\xef\xbc\x8c\xe5\xb9\xb6\xe8\xaf\xb7\xe4\xbf\x9d\xe7\x95\x99\xe8"
        "\xbf\x99\xe4\xb8\x80\xe8\xa1\x8c ===\n");

    add("en", "EXAMPLE-LINE", "Hello!\n\n");
    add("zh_TW", "EXAMPLE-LINE", "\xe4\xbd\xa0\xe5\xa5\xbd\xef\xbc\x81\n\n");
    add("zh_CN", "EXAMPLE-LINE", "\xe4\xbd\xa0\xe5\xa5\xbd\xef\xbc\x81\n\n");

#endif
  }
};

typedef OVLocalization<ImeServerStringTable> ISL;

class CannedMessageFetcherDelegate : public RemoteDataFetcherDelegate {
 public:
  CannedMessageFetcherDelegate(IMEServer* repObj)
      : m_representedObject(repObj) {}

  virtual void handleFetchResult(const char* data, size_t size);

 protected:
  IMEServer* m_representedObject;
};

class OneKeyDataFetcherDelegate : public RemoteDataFetcherDelegate {
 public:
  OneKeyDataFetcherDelegate(IMEServer* repObj) : m_representedObject(repObj) {}

  virtual void handleFetchResult(const char* data, size_t size);

 protected:
  IMEServer* m_representedObject;
};

class WVLoaderHookedServiceDelegate : public PVLoaderHookedServiceDelegate {
 public:
  WVLoaderHookedServiceDelegate(IMEServer* repObj)
      : m_representedObject(repObj) {}
  virtual void* loaderSpecificDataObjectForName(const string& name);

 protected:
  IMEServer* m_representedObject;
};

class WVLoaderCallback : public PVLoaderCallback {
 public:
  virtual void loaderConfigChanged(PVLoader* loader) {}

  virtual void loaderConfigSaved(PVLoader* loader) {
    set<string> skipAFSet;

    // before we activate, we want to validate the AF config so that the
    // ReverseLookup AF that overlaps with primary IM will be disabled
    OVWildcard exp("ReverseLookup-*");
    size_t strBegin = strlen("ReverseLookup-");
    string IMID = loader->primaryInputMethod();
    vector<pair<string, string> > afs =
        loader->allAroundFilterIdentifiersAndNames();
    for (vector<pair<string, string> >::const_iterator ai = afs.begin();
         ai != afs.end(); ++ai) {
      const string& modId = (*ai).first;
      if (!loader->isAroundFilterActivated(modId)) continue;

      if (exp.match(modId)) {
        string correspondingIMID = modId.substr(strBegin);

        if (OVWildcard::Match(IMID, correspondingIMID)) {
          skipAFSet.insert(modId);
        } else if (OVWildcard::Match(IMID, "*Mandarin*") &&
                   correspondingIMID == "Mandarin-bpmf-cin") {
          skipAFSet.insert(modId);
        }
      }
    }

    // if ChineseCharacterConvertor-TC2SC is not on and YKPhraseConverter is on,
    // we turn it off
    if (!loader->isOutputFilterActivated("ChineseCharacterConvertor-TC2SC") &&
        loader->isAroundFilterActivated("YKPhraseConverter")) {
      skipAFSet.insert("YKPhraseConverter");
    }

    loader->setSkippedAroundFilterIdentifiers(skipAFSet);
  }
};

using namespace LFPlatform;
using namespace CareService;
using namespace Manjusri;
using namespace Formosa::Mandarin;

IMEServer* IMEServer::c_sharedIMEServer = 0;

bool IMEServer::Start(const string& locale) {
  ISL::SetDefaultLocale(locale);

  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);
  string logFilename =
      OVPathHelper::PathCat(appDataDir, BASEIME_SERVER_LOG_FILENAME);
  Logger::UseLogFile(logFilename);
  Logger::UseConsole(false);

  Logger logger("IMEServer");

  // if we're already loaded, skip
  wchar_t wCurrentDir[1024];
  GetModuleFileName(0, wCurrentDir, 1024);
  string exeExpression = "*";
  exeExpression +=
      OVPathHelper::FilenameWithoutPath(OVUTF8::FromUTF16(wCurrentDir));
  logger.debug("Finding if %s already exists in process table",
               exeExpression.c_str());
  size_t foundCount = 0;

  if (ProcessHelper::ProcessNameExistsForCurrentUser(exeExpression,
                                                     &foundCount)) {
    if (foundCount > 1) {
      logger.error("We are already loaded, abort");
      return false;
    }
  }

  if (c_sharedIMEServer) return true;

  logger.debug("Server started");

  c_sharedIMEServer = new IMEServer(locale);
  if (!c_sharedIMEServer) return false;

  if (!c_sharedIMEServer->start()) {
    delete c_sharedIMEServer;
    c_sharedIMEServer = 0;
    return false;
  }

  return true;
}

bool IMEServer::StartRPCService() {
  Logger logger("IMEServerInit");

  if (!RPCServerHelper::StartService(BASEIME_SERVER_IDENTIFIER,
                                     BIServerRPCInterface_v1_0_s_ifspec)) {
    logger.error("Cannot start RPC server service, must abort.");
    return false;
  }

  logger.debug("RPC service started");
  return true;
}

void IMEServer::Stop() {
  Logger* logger = new Logger("IMEServerInit");

  logger->debug("Shutting down the server");
  if (c_sharedIMEServer) {
    delete c_sharedIMEServer;
    c_sharedIMEServer = 0;
  }

  logger->debug("Stopping RPC service");
  RPCServerHelper::StopService();

  logger->debug("Signing off");
  delete logger;
  Logger::FlushLogger();

  // this is why we need to dynamic-allocate logger -- because we're shutting it
  // down
  Logger::CloseLogger();
}

const string IMEServer::loaderComponentName() { return m_loaderComponentName; }

const string IMEServer::databaseComponentName() {
  return m_databaseComponentName;
}

const string IMEServer::vendorModuleComponentName() {
  return string(TAKAO_VENDOR_MODULE_COMPONENT_NAME);
}

const string IMEServer::databaseVersion() { return m_databaseVersion; }

IMEServer::IMEServer(const string& locale)
    : m_locale(locale),
      m_logEmitter(0),
      m_loaderPolicy(0),
      m_encodingService(0),
      m_CINDatabaseService(0),
      m_SQLiteDatabaseService(0),
      m_signedModuleLoadingSystem(0),
      m_staticModulePackageLoadingSystem(0),
      m_loaderService(0),
      m_loader(0),
      m_clientRPCInstanceCount(0),
      m_alphanumericMode(false),
      m_fullWidthCharacterMode(false),
      m_activeContext(0),
      m_versionChecker(0),
      m_directCharToClient(0),
      m_waitingForPunctuationKeyboardKey(false)

      ,
      m_cannedMessageFetchDelegate(0),
      m_cannedMessagePlistValue(0),
      m_userCannedMessagePlist(0),
      m_mergedCannedMessagePlistValue(0),
      m_mergedButtonSymbolPlistValue(0)

      ,
      m_oneKeyDataFetchDelegate(0),
      m_oneKeyPlistValue(0),
      m_mergedOneKeyPlistValue(0),
      m_userOneKeyPlist(0)

      ,
      m_hookedServiceDelegate(0)

      ,
      m_userPhraseDB(0)

      ,
      m_loaderCallback(0),
      m_userPersistence(0)

      ,
      m_stopUsingServerSideUI(false) {
  Logger logger("IMEServer");

  wchar_t wCurrentDir[1024];
  GetModuleFileName(0, wCurrentDir, 1024);
  m_programPath =
      OVPathHelper::DirectoryFromPath(OVUTF8::FromUTF16(wCurrentDir));
  logger.debug("Initialized, executable loaded from %s", m_programPath.c_str());

  m_versionChecker = new VersionChecker;

  // determine if we're running under Wow64, if so, we append -x64 after the
  // Loader name (e.g. Takao-Windows-x64)
  if (ProcessHelper::IsRunningUnderWow64()) {
    m_loaderComponentName = BASEIME_LOADER_NAME "-x64";
    m_databaseComponentName = BASEIME_DATABASE_COMPONENT_NAME "-x64";
  } else {
    m_loaderComponentName = BASEIME_LOADER_NAME;
    m_databaseComponentName = BASEIME_DATABASE_COMPONENT_NAME;
  }

  m_versionChecker->registerComponentVersion(m_loaderComponentName,
                                             BASEIME_INSTALLER_VERSION, true);
  logger.debug("Registering component %s with version %s",
               m_loaderComponentName.c_str(), BASEIME_INSTALLER_VERSION);

#define PPK(k, v) m_punctuationKeyboardMap[k] = v;
  PPK('1', 0x250c);
  PPK('2', 0x252c);
  PPK('3', 0x2510);
  PPK('4', 0x301d);
  PPK('5', 0x301e);
  PPK('6', 0x2018);
  PPK('7', 0x2019);
  PPK('8', 0x201c);
  PPK('9', 0x201d);
  PPK('0', 0x300e);
  PPK('-', 0x300f);
  PPK('=', 0x300c);
  PPK('\\', 0x300d);
  PPK('q', 0x251c);
  PPK('w', 0x253c);
  PPK('e', 0x2524);
  PPK('r', 0x203b);
  PPK('t', 0x3008);
  PPK('y', 0x3009);
  PPK('u', 0x300a);
  PPK('i', 0x300b);
  PPK('o', 0x3010);
  PPK('p', 0x3011);
  PPK('[', 0xfe5d);
  PPK(']', 0xfe5e);
  PPK('a', 0x2514);
  PPK('s', 0x2534);
  PPK('d', 0x2518);
  PPK('f', 0x25cb);
  PPK('g', 0x25cf);
  PPK('h', 0x2191);
  PPK('j', 0x2193);
  PPK('k', 0xff01);
  PPK('l', 0xff1a);
  PPK(';', 0xff1b);
  PPK('\'', 0x3001);
  PPK('z', 0x2500);
  PPK('x', 0x2502);
  PPK('c', 0x25ce);
  PPK('v', 0x00a7);
  PPK('b', 0x2190);
  PPK('n', 0x2192);
  PPK('m', 0x3002);
  PPK(',', 0xff0c);
  PPK('.', 0x2027);
  PPK('/', 0xff1f);
#undef PPK

  m_cannedMessageFetchDelegate = new CannedMessageFetcherDelegate(this);
  m_cannedMessagePlistValue = new PVPlistValue(PVPlistValue::Dictionary);
  m_userCannedMessagePlist = new PVPropertyList(userCannedMessagePlistPath());
  m_mergedCannedMessagePlistValue = new PVPlistValue(PVPlistValue::Dictionary);
  m_mergedButtonSymbolPlistValue = new PVPlistValue(PVPlistValue::Array);

  m_oneKeyDataFetchDelegate = new OneKeyDataFetcherDelegate(this);
  m_oneKeyPlistValue = new PVPlistValue(PVPlistValue::Dictionary);
  m_mergedOneKeyPlistValue = new PVPlistValue(PVPlistValue::Dictionary);
  m_userOneKeyPlist = new PVPropertyList(userOneKeyPlistPath());

  m_hookedServiceDelegate = new WVLoaderHookedServiceDelegate(this);
}

IMEServer::~IMEServer() {
  if (m_loader) delete m_loader;

  if (m_loaderService) delete m_loaderService;

  if (m_signedModuleLoadingSystem) delete m_signedModuleLoadingSystem;

  if (m_staticModulePackageLoadingSystem)
    delete m_staticModulePackageLoadingSystem;

  if (m_loaderPolicy) delete m_loaderPolicy;

  if (m_SQLiteDatabaseService) delete m_SQLiteDatabaseService;

  if (m_CINDatabaseService) delete m_CINDatabaseService;

  if (m_encodingService) delete m_encodingService;

  if (m_logEmitter) delete m_logEmitter;

  if (m_versionChecker) delete m_versionChecker;

  delete m_cannedMessageFetchDelegate;
  delete m_cannedMessagePlistValue;
  delete m_userCannedMessagePlist;
  delete m_mergedCannedMessagePlistValue;
  delete m_mergedButtonSymbolPlistValue;

  delete m_oneKeyDataFetchDelegate;
  delete m_oneKeyPlistValue;
  delete m_mergedOneKeyPlistValue;
  delete m_userOneKeyPlist;

  delete m_hookedServiceDelegate;

  if (m_userPhraseDB) {
    delete m_userPhraseDB;
  }

  delete m_userPersistence;
  delete m_loaderCallback;

  DeleteCriticalSection(&m_win32CriticalSection);
}

string IMEServer::fetchDatabaseVersionInfo(OVSQLiteConnection* connection,
                                           const string& dbAndTableName) {
  string result;
  OVSQLiteStatement* statement = connection->prepare(
      "SELECT value FROM %s WHERE KEY = %Q", dbAndTableName.c_str(), "version");

  if (statement) {
    if (statement->step() == SQLITE_ROW) {
      result = statement->textOfColumn(0);
      while (statement->step() == SQLITE_ROW)
        ;
    }

    delete statement;
  }

  return result;
}

void IMEServer::createDatabaseServices() {
  Logger logger("IMEServer");

  if (m_CINDatabaseService) {
    delete m_CINDatabaseService;
    m_CINDatabaseService = 0;
  }

  if (m_SQLiteDatabaseService) {
    delete m_SQLiteDatabaseService;
    m_userPersistence->setDefaultDatabaseConnection(0, "");
    m_SQLiteDatabaseService = 0;
  }

  string cinDir = OVPathHelper::PathCat(m_programPath, "DataTables");
  string dbDir = OVPathHelper::PathCat(m_programPath, "Databases");

  string supplementDBFile = OVPathHelper::PathCat(dbDir, "Supplement.db");

#ifdef OPENVANILLA_CEROD_DATABASE_FILE
  string dbFile = OVPathHelper::PathCat(dbDir, OPENVANILLA_CEROD_DATABASE_FILE);
#else
  string dbFile =
      OVPathHelper::PathCat(dbDir, m_loaderPolicy->defaultDatabaseFileName());
#endif

  logger.debug("trying to load .cin from: %s", cinDir.c_str());
  logger.debug("trying to open the db file: %s", dbFile.c_str());

  string userDataPath = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  string userTablePath = OVPathHelper::PathCat(userDataPath, "DataTables");
  logger.debug("trying to load .cin from: %s", userTablePath.c_str());

  m_CINDatabaseService = new OVCINDatabaseService(cinDir, "*.cin", "", 0);

  if (m_CINDatabaseService) {
    if (OVPathHelper::PathExists(userTablePath)) {
      m_CINDatabaseService->addDirectory(userTablePath, "*.cin", "", 0);
    }

    logger.debug("Available data tables: %d",
                 m_CINDatabaseService->tables().size());
  }

  string supplementDBVersion;
  string mainDBVersion;

  if (OVPathHelper::PathExists(dbFile)) {
#ifndef OVLOADER_USE_SQLITE_CRYPTO
    m_SQLiteDatabaseService = OVSQLiteDatabaseService::Create(dbFile);
#else
#ifdef OPENVANILLA_CEROD_DATABASE_FILE
    dbFile = FetchSQLiteCERODKey(dbFile);
    OVSQLiteConnection* dbc = OVSQLiteConnection::Open(dbFile);

    if (OVPathHelper::PathExists(supplementDBFile)) {
      logger.debug("supplement DB exists");
      supplementDBFile = FetchSQLiteCERODKey(supplementDBFile);
      int attachResult =
          dbc->execute("ATTACH %Q AS supplement", supplementDBFile.c_str());
      logger.debug("attach result: %d", attachResult);

      if (attachResult == SQLITE_OK) {
        logger.debug("fetching attached db version info");
        supplementDBVersion =
            fetchDatabaseVersionInfo(dbc, "supplement.cooked_information");
      }
    }

    m_SQLiteDatabaseService =
        OVSQLiteDatabaseService::ServiceWithExistingConnection(dbc, true);
#else
    OVSQLiteConnection* dbc = OVSQLiteConnection::Open(dbFile);
    InitSQLiteCrypto(dbc->connection());
    m_SQLiteDatabaseService =
        OVSQLiteDatabaseService::ServiceWithExistingConnection(dbc, true);
#endif

    mainDBVersion = fetchDatabaseVersionInfo(
        m_SQLiteDatabaseService->connection(), "cooked_information");

    logger.debug("Main DB version: '%s', supplement DB version: '%s'",
                 mainDBVersion.c_str(), supplementDBVersion.c_str());

    if (supplementDBVersion.size()) {
      m_versionChecker->registerComponentVersion(m_databaseComponentName,
                                                 supplementDBVersion, true);
      logger.debug("Registered supplement DB '%s' version '%s'",
                   m_databaseComponentName.c_str(),
                   supplementDBVersion.c_str());

      m_databaseVersion = supplementDBVersion;

      // see if main DB's version is newer!
      if (mainDBVersion.size()) {
        if (VersionNumber(mainDBVersion) >=
            VersionNumber(supplementDBVersion)) {
          logger.debug("Detaching supplement DB because it's older");
          dbc->execute("DETACH supplement");
          m_versionChecker->registerComponentVersion(m_databaseComponentName,
                                                     mainDBVersion, true);
          m_databaseVersion = mainDBVersion;
        }
      }
    } else if (mainDBVersion.size()) {
      m_versionChecker->registerComponentVersion(m_databaseComponentName,
                                                 mainDBVersion, true);
      logger.debug("Registered main DB '%s' version '%s'",
                   m_databaseComponentName.c_str(), mainDBVersion.c_str());

      m_databaseVersion = mainDBVersion;
    }

    if (dbc->execute("PRAGMA synchronous = OFF") == SQLITE_OK) {
      logger.debug("pragma executed");
    } else {
      logger.debug("pragma execution failed");
    }

#endif
  }

  if (!m_SQLiteDatabaseService) {
    // we can't display dbFile here now because it shows... well... the password
    // -_-
    logger.debug("Cannot open database file, use in-memory version");
    m_SQLiteDatabaseService = OVSQLiteDatabaseService::Create();
  }

  m_userPersistence->setDefaultDatabaseConnection(
      m_SQLiteDatabaseService->connection(), "prepopulated_service_data");

  string oneKeyData = m_userPersistence->fetchLatestValueByKeyAndPopulateUserDB(
      "onekey_services");
  string cannedMsgs = m_userPersistence->fetchLatestValueByKeyAndPopulateUserDB(
      "canned_messages");

  PVPlistValue* okd = PVPropertyList::ParsePlistFromString(oneKeyData.c_str());
  PVPlistValue* cms = PVPropertyList::ParsePlistFromString(cannedMsgs.c_str());

  if (okd) {
    delete m_oneKeyPlistValue;
    m_oneKeyPlistValue = okd;
  }

  if (cms) {
    delete m_cannedMessagePlistValue;
    m_cannedMessagePlistValue = cms;
  }

  mergeCannedMessage();
  mergeOneKeyData();
}

void IMEServer::shutdownServerForDatabaseUpdate() {
  m_loaderService->logger("Loader")
      << "shutdownServerForDatabaseUpdate" << endl;
  m_loader->prepareReload();
  m_signedModuleLoadingSystem->unloadAllUnloadables();

  // and shut down static loading system too!
  m_staticModulePackageLoadingSystem->flushModules();

  // shut down all DB services -- we can't wait until createDatabase()
  delete m_CINDatabaseService;
  m_CINDatabaseService = 0;

  // detach supplement no matter if it's attached at all
  int result = m_SQLiteDatabaseService->connection()->execute(
      "DETACH DATABASE supplement");
  m_loaderService->logger("Loader")
      << "detach supplement result: " << result << endl;

  delete m_SQLiteDatabaseService;
  m_userPersistence->setDefaultDatabaseConnection(0, "");
  m_SQLiteDatabaseService = 0;
}

void IMEServer::restartServerAndCompleteDatabaseUpdate() {
  m_loaderService->logger("Loader")
      << "restartServerAndCompleteDatabaseUpdate" << endl;
  m_signedModuleLoadingSystem->rescan(m_loaderPolicy);

  createDatabaseServices();
  m_loaderService->setCINDatabaseService(m_CINDatabaseService);
  m_loaderService->setSQLiteDatabaseService(m_SQLiteDatabaseService);

  OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  loadStaticModules(pathInfo);

  unregisterVendorSignedModuleVersionInfo();
  m_loader->reload();
  registerVendorSignedModuleVersionInfo();
}

void IMEServer::stopUsingSignedModules() {
  unregisterVendorSignedModuleVersionInfo();
  m_loaderService->logger("Loader") << "stopUsingSignedModules" << endl;

  m_loader->prepareReload();
  m_signedModuleLoadingSystem->unloadAllUnloadables();
  m_staticModulePackageLoadingSystem->flushModules();
  m_signedModuleLoadingSystem->reset();
  // m_signedModuleLoadingSystem->rescan(m_loaderPolicy);
  OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  loadStaticModules(pathInfo);
  m_loader->reload();
}

void IMEServer::resumeUsingSignedModules() {
  unregisterVendorSignedModuleVersionInfo();
  m_loaderService->logger("Loader") << "resumeUsingSignedModules" << endl;

  m_loader->prepareReload();
  m_signedModuleLoadingSystem->unloadAllUnloadables();
  m_staticModulePackageLoadingSystem->flushModules();
  m_signedModuleLoadingSystem->reset();
  m_signedModuleLoadingSystem->rescan(m_loaderPolicy);
  OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  loadStaticModules(pathInfo);
  m_loader->reload();
  registerVendorSignedModuleVersionInfo();
}

void IMEServer::unregisterVendorSignedModuleVersionInfo() {
  m_versionChecker->unregisterComponent(TAKAO_VENDOR_MODULE_COMPONENT_NAME);
}

void IMEServer::registerVendorSignedModuleVersionInfo() {
  // we register the largest version num
  VersionNumber vn;
  string lastVersion;

  OVWildcard exp("com.yahoo.KeyKey.*");
  Logger logger("IMEServer::registerVendorSignedModuleVersionInfo");
  map<string, string> ivm =
      m_signedModuleLoadingSystem->identifiableVersionMap();
  for (map<string, string>::iterator mi = ivm.begin(); mi != ivm.end(); ++mi) {
    logger.debug("version %s -> %s", (*mi).first.c_str(), (*mi).second.c_str());

    if (exp.match((*mi).first)) {
      VersionNumber v((*mi).second);
      if (v > vn) {
        vn = v;
        lastVersion = (*mi).second;
      }
    }
  }

  if (lastVersion.length()) {
    logger.debug("registering component using version %s", lastVersion.c_str());
    m_versionChecker->registerComponentVersion(
        TAKAO_VENDOR_MODULE_COMPONENT_NAME, lastVersion, true);
  }
}

void IMEServer::removeFirstSignedModuleInList() {
  vector<string> packages = m_signedModuleLoadingSystem->availablePackages();
  if (packages.size()) {
    string toRemove = packages[0];
    m_loaderService->logger("Loader") << "Removing! " << toRemove;
    removeSignedModuleAndRestart(toRemove);
  }
}

bool IMEServer::removeSignedModuleAndRestart(const string& packageName) {
  if (!m_signedModuleLoadingSystem->packageExistsInLoadingSystem(packageName)) {
    m_loaderService->logger("Loader")
        << "removeSignedModuleAndRestart, fail, not exist: " << packageName
        << endl;
    return false;
  }

  m_loaderService->logger("Loader")
      << "removeSignedModuleAndRestart: " << packageName << endl;

  m_loader->prepareReload();
  m_signedModuleLoadingSystem->unloadAllUnloadables();
  m_staticModulePackageLoadingSystem->flushModules();

  // get the info
  OVPathInfo pathInfo = m_signedModuleLoadingSystem->pathInfoForPackage(
      packageName, m_loaderPolicy);
  m_loaderService->logger("Loader")
      << "Preparing to delete: " << pathInfo.loadedPath << endl;

  bool result = OVPathHelper::RemoveEverythingAtPath(pathInfo.loadedPath);

  // restart
  m_signedModuleLoadingSystem->reset();
  m_signedModuleLoadingSystem->rescan(m_loaderPolicy);
  pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  loadStaticModules(pathInfo);

  unregisterVendorSignedModuleVersionInfo();
  m_loader->reload();
  registerVendorSignedModuleVersionInfo();

  return result;
}

size_t IMEServer::numberOfUnloadableSignedModules() {
  return m_signedModuleLoadingSystem->availablePackages().size();
}

const string IMEServer::localizedNameOfUnloadableSignedModulesAtIndex(
    size_t index) {
  vector<string> names = m_signedModuleLoadingSystem->availablePackages();
  if (index >= names.size()) {
    return string("No Package - Out of Bound");
  }

  return m_signedModuleLoadingSystem->localizedNameForPackage(names[index],
                                                              m_loaderService);
}

bool IMEServer::unloadSignedModuleAtIndex(size_t index) {
  vector<string> names = m_signedModuleLoadingSystem->availablePackages();
  if (index >= names.size()) {
    return false;
  }

  return removeSignedModuleAndRestart(names[index]);
}

void IMEServer::reload() {
  m_loaderService->logger("Loader")
      << "Preparing to reload the modules" << endl;
  m_loader->prepareReload();

  createDatabaseServices();

  m_signedModuleLoadingSystem->unloadAllUnloadables();
  m_signedModuleLoadingSystem->rescan(m_loaderPolicy);

  OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  loadStaticModules(pathInfo);

  m_loaderService->setCINDatabaseService(m_CINDatabaseService);
  m_loaderService->setSQLiteDatabaseService(m_SQLiteDatabaseService);
  m_loader->reload();
}

void IMEServer::loadStaticModules(OVPathInfo& pathInfo) {
  vector<pair<OVModulePackage*, string> > staticPackages =
      StaticPack::InitializedModulePackages(&pathInfo, m_loaderService);
  for (vector<pair<OVModulePackage*, string> >::iterator piter =
           staticPackages.begin();
       piter != staticPackages.end(); ++piter)
    m_staticModulePackageLoadingSystem->addInitializedPackage((*piter).second,
                                                              (*piter).first);
}

bool IMEServer::start() {
  if (m_loader) return true;

  InitializeCriticalSection(&m_win32CriticalSection);

  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);

  vector<string> moduleLoadPaths;
  moduleLoadPaths.push_back(OVPathHelper::PathCat(appDataDir, "Modules"));
  moduleLoadPaths.push_back(OVPathHelper::PathCat(m_programPath, "Modules"));

  // create user persistence database
  string userPersistenceDBPath =
      OVPathHelper::PathCat(appDataDir, "UserData.db");
  m_userPersistence = new OVLoaderUserPersistence(userPersistenceDBPath);

  m_logEmitter = new WVLogEmitter;
  m_loaderPolicy = new WVLoaderPolicy(moduleLoadPaths);
  createDatabaseServices();

  m_encodingService = new WVEncodingService;
  m_loaderService = new PVLoaderService(m_locale, m_CINDatabaseService,
                                        m_SQLiteDatabaseService, m_logEmitter,
                                        m_encodingService);
  m_loaderService->setHookedServiceDelegate(m_hookedServiceDelegate);

  m_signedModuleLoadingSystem = new YKSignedModuleLoadingSystem(m_loaderPolicy);

  OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
  m_staticModulePackageLoadingSystem =
      new PVStaticModulePackageLoadingSystem(pathInfo, true);
  loadStaticModules(pathInfo);

  vector<PVModulePackageLoadingSystem*> loadingSystems;
  loadingSystems.push_back(m_staticModulePackageLoadingSystem);
  loadingSystems.push_back(m_signedModuleLoadingSystem);

  m_loaderCallback = new WVLoaderCallback;

  m_loader = new PVLoader(m_loaderPolicy, m_loaderService, loadingSystems);
  m_loader->registerCallback(m_loaderCallback);

  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  bool writeConfig = false;

  string platformSummary = SystemInfo::PlatformSummary();
  string loaderVersion;
  loaderVersion = BASEIME_INSTALLER_VERSION;

  if (kvm.stringValueForKey("PlatformSummary") != platformSummary) {
    kvm.setKeyStringValue("PlatformSummary", platformSummary);
    writeConfig = true;
  }

  if (kvm.stringValueForKey("LoaderVersion") != loaderVersion) {
    kvm.setKeyStringValue("LoaderVersion", loaderVersion);
    writeConfig = true;
  }

  if (kvm.stringValueForKey("DatabaseVersion") != m_databaseVersion) {
    kvm.setKeyStringValue("DatabaseVersion", m_databaseVersion);
    writeConfig = true;
  }

  if (!kvm.hasKey("UUID")) {
    kvm.setKeyStringValue("UUID", UUIDHelper::CreateUUID());
    writeConfig = true;
  }

  // populate default settings
  if (!kvm.hasKey("ChineseConverterToggleKey")) {
    kvm.setKeyStringValue("ChineseConverterToggleKey", "s");
    writeConfig = true;
  }

  if (!kvm.hasKey("RepeatLastCommitTextKey")) {
    kvm.setKeyStringValue("RepeatLastCommitTextKey", "g");
    writeConfig = true;
  }

  if (writeConfig) IMEServer::SharedLoader()->syncLoaderConfig(true);

  // PVPlistValue* allPlists =
  // IMEServer::SharedLoader()->loaderAndModulePropertyListsCombined();
  // IMEServer::SharedLoaderService()->logger("LoaderConfig") << *allPlists <<
  // endl; delete allPlists;

  // find the output filter name
  Logger logger("LoaderStarter");
  vector<pair<string, string> > outputFilters;
  outputFilters = m_loader->allOutputFilterIdentifiersAndNames();
  for (vector<pair<string, string> >::iterator iter = outputFilters.begin();
       iter != outputFilters.end(); ++iter) {
    if (OVWildcard::Match((*iter).first, "*FullWidthCharacter*")) {
      logger.debug("Found full-width char filter: %s", (*iter).first.c_str());
      m_fullWidthCharacterFilterIdentifier = (*iter).first;
      break;
    }
  }

  // lists all known unloadable modules
  //    vector<string> allLoadablePkgs =
  //    m_signedModuleLoadingSystem->availablePackages(); for
  //    (vector<string>::iterator alpi = allLoadablePkgs.begin() ; alpi !=
  //    allLoadablePkgs.end() ; ++alpi) {
  //        m_loaderService->logger("Names: ") << *alpi << " -> " <<
  //        m_signedModuleLoadingSystem->localizedNameForPackage(*alpi,
  //        m_loaderService) << endl;
  //    }

  registerVendorSignedModuleVersionInfo();

  return true;
}

void IMEServer::setCurrentClientRPCEndpointName(const string& endpointName) {
  Logger logger("ClientSetRPC");
  logger.debug("Client set current RPC endpoint to %s", endpointName.c_str());
  m_currentClientRPCEndpointName = endpointName;
}

const string IMEServer::currentClientRPCEndpointName() const {
  return m_currentClientRPCEndpointName;
}

const string IMEServer::nextAvailableClientRPCEndpointName() {
  stringstream st;
  st << BASEIME_CLIENT_IDENTIFIER_PREFIX << (unsigned long)GetCurrentProcess()
     << "-" << m_clientRPCInstanceCount;
  ++m_clientRPCInstanceCount;

  return st.str();
}

PVLoaderContext* IMEServer::activeContext() { return m_activeContext; }

void IMEServer::setActiveContext(PVLoaderContext* context) {
  m_activeContext = context;
}

bool IMEServer::isFullWidthCharacterMode() { return m_fullWidthCharacterMode; }

void IMEServer::setFullWidthCharacterMode(bool mode) {
  m_fullWidthCharacterMode = mode;
}

bool IMEServer::isAlphanumericMode() { return m_alphanumericMode; }

void IMEServer::setAlphanumericMode(bool mode) { m_alphanumericMode = mode; }

void IMEServer::sendCharToClient(char c) {
  Logger logger("SendCharToClient");
  logger.debug("Char: %c (%d)", c);

  m_directCharToClient = c;

  if (!m_currentClientRPCEndpointName.size()) {
    logger.error("No client endpoint is available");
  } else {
    RPCClientHelper::ServerHandle client = RPCClientHelper::ConnectServer(
        m_currentClientRPCEndpointName, &BIClientRPCInterfaceHandle);

    if (!client) {
      logger.error("Cannot init the RPC service interface");
      return;
    }

    performClientRPCRequest(&logger);
    RPCClientHelper::Disconnect(client, &BIClientRPCInterfaceHandle);
  }
}

char IMEServer::directCharToClient() { return m_directCharToClient; }

void IMEServer::clearDirectCharToClient() { m_directCharToClient = 0; }

void IMEServer::sendStringToClient(const string& text) {
  Logger logger("SendStringToClient");
  logger.debug("Sending: %s", text.c_str());

  m_directTextToClient = text;

  if (!m_currentClientRPCEndpointName.size()) {
    logger.error("No client endpoint is available");
  } else {
    RPCClientHelper::ServerHandle client = RPCClientHelper::ConnectServer(
        m_currentClientRPCEndpointName, &BIClientRPCInterfaceHandle);

    if (!client) {
      logger.error("Cannot init the RPC service interface");
      return;
    }

    performClientRPCRequest(&logger);
    RPCClientHelper::Disconnect(client, &BIClientRPCInterfaceHandle);
  }
}

void IMEServer::performClientRPCRequest(Logger* logger) {
  RpcTryExcept { BICRequestSendFakeKey(); }
  RpcExcept(1) { logger->error("exception in calling"); }
  RpcEndExcept;
}

const string IMEServer::fullWidthCharacterFilterIdentifier() {
  return m_fullWidthCharacterFilterIdentifier;
}

const string IMEServer::directTextToClient() { return m_directTextToClient; }

void IMEServer::clearDirectTextToClient() { m_directTextToClient = ""; }

const string IMEServer::fullWidthCharacterStringIfActivated(
    const string& text) {
  if (!isFullWidthCharacterMode()) return text;

  OVModule* module =
      m_loader->moduleWithName(fullWidthCharacterFilterIdentifier());
  OVEventHandlingContext* context = module->createContext();
  string converted = context->filterText(text, m_loaderService);
  delete context;
  return converted;
}

const string IMEServer::serverURLWithDefaultValueAndConfigurableKey(
    const string& defaultValue, const string& overrideKeyName) {
  Logger logger("IMEServer");

  string result;
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  if (!kvm.hasKey(overrideKeyName)) {
    result = defaultValue;
  } else {
    result = kvm.stringValueForKey(overrideKeyName);
  }

  logger.debug("Querying %s = %s", overrideKeyName.c_str(), result.c_str());

  return result;
}

const string IMEServer::versionInfoURL() {
  //	return
  //serverURLWithDefaultValueAndConfigurableKey("http://w1.keykey.vip.tpc.yahoo.com/ykk/VersionInfo.php",
  //"VersionInfoURL");
  return serverURLWithDefaultValueAndConfigurableKey(
      "http://tw.media.yahoo.com/ykk/VersionInfo.php", "VersionInfoURL");
}

const string IMEServer::versionInfoSignatureURL() {
  //    return
  //    serverURLWithDefaultValueAndConfigurableKey("http://w1.keykey.vip.tpc.yahoo.com/ykk/VersionInfo.sig",
  //    "VersionInfoSignatureURL");
  return serverURLWithDefaultValueAndConfigurableKey(
      "http://tw.media.yahoo.com/ykk/VersionInfo.sig",
      "VersionInfoSignatureURL");
}

const string IMEServer::serverOneKeyDataEndpointURL() {
  //    return
  //    serverURLWithDefaultValueAndConfigurableKey("http://w1.keykey.vip.tpc.yahoo.com/ykk/OneKey.plist",
  //    "OneKeyDataEndpointURL");
  return serverURLWithDefaultValueAndConfigurableKey(
      "http://tw.media.yahoo.com/ykk/OneKey.plist", "OneKeyDataEndpointURL");
}
const string IMEServer::serverCannedMessagesEndpointURL() {
  //    return
  //    serverURLWithDefaultValueAndConfigurableKey("http://w1.keykey.vip.tpc.yahoo.com/ykk/CannedMessages.plist",
  //    "CannedMessagesEndpointURL");
  return serverURLWithDefaultValueAndConfigurableKey(
      "http://tw.media.yahoo.com/ykk/CannedMessages.plist",
      "CannedMessagesEndpointURL");
}

bool IMEServer::isWaitingForPunctuationKeyboardKey() {
  return m_waitingForPunctuationKeyboardKey;
}

void IMEServer::setWaitingForPunctuationKeyboardKey(bool waiting) {
  m_waitingForPunctuationKeyboardKey = waiting;
}

const string IMEServer::lookupPunctuationKeyboardSymbol(char key) {
  map<char, wchar_t>::iterator mi = m_punctuationKeyboardMap.find(key);
  if (mi != m_punctuationKeyboardMap.end()) {
    wstring ws(1, (*mi).second);
    return OVUTF8::FromUTF16(ws);
  }

  return string();
}

const string IMEServer::lastCommittedText() { return m_lastCommittedText; }

void IMEServer::setLastCommittedText(const string& text) {
  m_lastCommittedText = text;
}

RemoteDataFetcherDelegate* IMEServer::cannedMessageFetchDelegate() {
  return m_cannedMessageFetchDelegate;
}

void IMEServer::syncUserCannedMessages() {
  string path = userFreeCannedMessagePath();
  if (!OVPathHelper::PathExists(path)) {
    // populate the file with UTF-8 BOM
    FILE* stream = OVFileHelper::OpenStream(path, "w");
    if (stream) {
      fputs(ISL::S("BOM-LINE").c_str(), stream);
      fputs(ISL::S("EXAMPLE-LINE").c_str(), stream);

      fclose(stream);
    }
  }

  bool shouldMerge = false;
  OVFileTimestamp newTS = OVPathHelper::TimestampForPath(path);
  if (newTS > m_userFreeCannedMessageFileTimestamp) {
    m_userFreeCannedMessageFileTimestamp = newTS;
    shouldMerge = true;
  }

  if (m_userCannedMessagePlist->shouldReadSync()) {
    m_userCannedMessagePlist->readSync();
    shouldMerge = true;
  }

  if (shouldMerge) {
    mergeCannedMessage();
  }
}

void IMEServer::mergeCannedMessage() {
  string localDT = OVDateTimeHelper::LocalDateTimeString();
  EnterCriticalSection(&m_win32CriticalSection);

  m_mergedCannedMessagePlistValue->removeAllKeysAndValues();
  m_mergedButtonSymbolPlistValue->removeAllArrayElements();

  PVPlistValue msgArray(PVPlistValue::Array);
  PVPlistValue* pvs[2];
  pvs[0] = m_cannedMessagePlistValue;
  pvs[1] = m_userCannedMessagePlist->rootDictionary();

  for (size_t pi = 0; pi < 2; pi++) {
    PVPlistValue* msgs = pvs[pi]->valueForKey("CannedMessages");
    if (msgs) {
      for (size_t i = 0; i < msgs->arraySize(); i++) {
        PVPlistValue* category = msgs->arrayElementAtIndex(i);

        string notBefore = category->stringValueForKey("NotBefore");
        if (notBefore.length() && localDT < notBefore) {
          continue;
        }

        string notAfter = category->stringValueForKey("NotAfter");
        if (notAfter.length() && localDT > notAfter) {
          continue;
        }

        if (category->isKeyTrue("IsSymbolButtonList")) {
          m_mergedButtonSymbolPlistValue->addArrayElement(category);
        } else {
          msgArray.addArrayElement(category);
        }
      }
    }
  }

  vector<string> userMessages;
  ifstream ifs;
  ifs.open(userFreeCannedMessagePath().c_str(), ifstream::in);

  if (ifs.good()) {
    // ignore the first line
    string emptyLine;
    getline(ifs, emptyLine);
  }

  while (ifs.good()) {
    string line;
    getline(ifs, line);
    if (line.length()) {
      userMessages.push_back(line);
    }
  }
  ifs.close();

  if (userMessages.size()) {
    PVPlistValue userCategory(PVPlistValue::Dictionary);
    userCategory.setKeyValue("Name", ISL::S("User Defined"));
    PVPlistValue messages(PVPlistValue::Array);
    for (vector<string>::iterator umi = userMessages.begin();
         umi != userMessages.end(); ++umi) {
      PVPlistValue msg(*umi);
      messages.addArrayElement(&msg);
    }
    userCategory.setKeyValue("Messages", &messages);
    msgArray.addArrayElement(&userCategory);
  }

  LeaveCriticalSection(&m_win32CriticalSection);

  m_mergedCannedMessagePlistValue->setKeyValue("CannedMessages", &msgArray);
}

const string IMEServer::userFreeCannedMessagePath() {
  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);
  return OVPathHelper::PathCat(appDataDir, "UserCannedMessages.txt");
}

const string IMEServer::userCannedMessagePlistPath() {
  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);
  return OVPathHelper::PathCat(appDataDir, "UserCannedMessages.plist");
}

PVPlistValue* IMEServer::cannedMessagePlistValueCopy() {
  EnterCriticalSection(&m_win32CriticalSection);
  PVPlistValue* value = m_mergedCannedMessagePlistValue->copy();
  LeaveCriticalSection(&m_win32CriticalSection);
  return value;
}

PVPlistValue* IMEServer::buttonSymbolPlistValueCopy() {
  EnterCriticalSection(&m_win32CriticalSection);
  PVPlistValue* value = m_mergedButtonSymbolPlistValue->copy();
  LeaveCriticalSection(&m_win32CriticalSection);
  return value;
}

bool IMEServer::userPhraseDBCheckConnection() {
  if (!m_userPhraseDB) {
    OVPathInfo pathInfo = m_loaderPolicy->modulePackagePathInfoFromPath("");
    m_userPhraseDB =
        BPMFUserPhraseHelper::OpenUserPhraseDB(&pathInfo, m_loaderService);
  }

  return !!m_userPhraseDB;
}

bool IMEServer::userPhraseDBCanProvideService() {
  return userPhraseDBCheckConnection();
}

int IMEServer::userPhraseDBNumberOfRow() {
  int count = 0;

  if (!userPhraseDBCheckConnection()) {
    return count;
  }

  OVSQLiteStatement* st =
      m_userPhraseDB->prepare("SELECT count(*) FROM user_unigrams");
  if (st) {
    while (st->step() == SQLITE_ROW) {
      count = st->intOfColumn(0);
    }
    delete st;
  }

  return count;
}

const string IMEServer::userPhraseDBDataAtRow(int row) {
  string result;
  if (!userPhraseDBCheckConnection()) {
    return result;
  }

  OVSQLiteStatement* select = m_userPhraseDB->prepare(
      "SELECT current, qstring FROM user_unigrams WHERE rowid = %d", row + 1);
  while (select->step() == SQLITE_ROW) {
    result = string(select->textOfColumn(0)) + "\t" +
             BPMFUserPhraseHelper::BPMFString(string(select->textOfColumn(1)));
  }

  return result;
}

const string IMEServer::userPhraseDBReadingsForPhrase(const string& phrase) {
  vector<string> results;
  vector<string> codepoints = OVUTF8Helper::SplitStringByCodePoint(phrase);

  OVSQLiteStatement* select = dynamic_cast<OVSQLiteDatabaseService*>(
                                  m_loaderService->SQLiteDatabaseService())
                                  ->connection()
                                  ->prepare(
                                      "SELECT qstring FROM unigrams WHERE "
                                      "current = ? ORDER BY probability DESC");

  OVKeyValueDataTableInterface* tbl =
      m_loaderService->SQLiteDatabaseService()
          ->createKeyValueDataTableInterface("Mandarin-bpmf-cin");

  vector<vector<string> > phraseBPMFs;
  phraseBPMFs.push_back(vector<string>());

  OVWildcard exp("*#");
  for (vector<string>::const_iterator cpi = codepoints.begin();
       cpi != codepoints.end(); ++cpi) {
    vector<string> bpmfs;
    set<string> dedup;

    if (select) {
      select->bindTextToColumn(*cpi, 1);
      while (select->step() == SQLITE_ROW) {
        string b = select->textOfColumn(0);

        if (exp.match(b)) continue;

        cerr << b << endl;
        dedup.insert(b);
        bpmfs.push_back(b);
      }
      select->reset();
    }

    vector<string> extBpmfs = tbl->keysForValue(*cpi);
    for (vector<string>::iterator ebi = extBpmfs.begin(); ebi != extBpmfs.end();
         ++ebi) {
      if (dedup.find(*ebi) == dedup.end()) {
        dedup.insert(*ebi);
        bpmfs.push_back(*ebi);
      }
    }

    if (!bpmfs.size()) {
      // a BPMF "B" here
      bpmfs = tbl->keysForValue("\xe3\x84\x85");
    }

    vector<vector<string> > npb;
    for (vector<vector<string> >::const_iterator pbi = phraseBPMFs.begin();
         pbi != phraseBPMFs.end(); ++pbi) {
      for (vector<string>::const_iterator bi = bpmfs.begin(); bi != bpmfs.end();
           ++bi) {
        vector<string> newEntry = *pbi;
        newEntry.push_back(BPMF::FromAbsoluteOrderString(*bi).composedString());
        npb.push_back(newEntry);
      }
    }
    phraseBPMFs = npb;
  }

  for (vector<vector<string> >::const_iterator pbi = phraseBPMFs.begin();
       pbi != phraseBPMFs.end(); ++pbi) {
    results.push_back(OVStringHelper::Join(*pbi, ","));
  }

  if (select) {
    delete select;
  }

  return OVStringHelper::Join(results, "\t");
}

void IMEServer::userPhraseDBSave() {
  if (userPhraseDBCheckConnection()) {
    m_userPhraseDB->execute("VACUUM");
    delete m_userPhraseDB;
    m_userPhraseDB = 0;
  }

  m_loader->forceSyncModuleConfigForNextRound("SmartMandarin");
}

const string IMEServer::qstringFromReading(const string& reading) {
  vector<string> readings = OVStringHelper::Split(reading, ',');
  string newReading;

  for (vector<string>::const_iterator ri = readings.begin();
       ri != readings.end(); ++ri) {
    BPMF b = BPMF::FromComposedString(*ri);
    if (b.isEmpty()) continue;

    newReading += b.absoluteOrderString();
  }
  return newReading;
}

void IMEServer::userPhraseDBSetNewReadingForPhraseAtRow(const string& reading,
                                                        int row) {
  if (!userPhraseDBCheckConnection()) {
    return;
  }

  m_userPhraseDB->execute(
      "UPDATE user_unigrams SET qstring = %Q WHERE rowid = %d",
      qstringFromReading(reading).c_str(), row + 1);
  m_loader->forceSyncModuleConfigForNextRound("SmartMandarin");
}

void IMEServer::userPhraseDBDeleteRow(int row) {
  if (!userPhraseDBCheckConnection()) {
    return;
  }

  m_userPhraseDB->execute("BEGIN");
  m_userPhraseDB->execute("CREATE TEMP TABLE uu_temp(a, b, c, d)");
  m_userPhraseDB->execute("INSERT INTO uu_temp SELECT * from user_unigrams");
  m_userPhraseDB->execute("DELETE FROM uu_temp WHERE rowid = %d", row + 1);
  m_userPhraseDB->execute("DELETE FROM user_unigrams");
  m_userPhraseDB->execute("INSERT INTO user_unigrams SELECT * from uu_temp");
  m_userPhraseDB->execute("DROP TABLE uu_temp");
  m_userPhraseDB->execute("END");
  m_loader->forceSyncModuleConfigForNextRound("SmartMandarin");
}

void IMEServer::userPhraseDBAddNewRow(const string& phrase) {
  if (!userPhraseDBCheckConnection()) {
    return;
  }

  vector<string> readings =
      OVStringHelper::Split(userPhraseDBReadingsForPhrase(phrase), '\t');
  if (!readings.size()) {
    return;
  }

  string reading = readings[0];
  m_userPhraseDB->execute(
      "INSERT INTO user_unigrams (qstring, current, probability, backoff) "
      "VALUES (%Q, %Q, %f, %f)",
      qstringFromReading(reading).c_str(), phrase.c_str(), -1.0, 0.0);

  m_loader->forceSyncModuleConfigForNextRound("SmartMandarin");
}

void IMEServer::userPhraseDBSetPhrase(const string& phrase, int row) {
  if (!userPhraseDBCheckConnection()) {
    return;
  }

  vector<string> readings =
      OVStringHelper::Split(userPhraseDBReadingsForPhrase(phrase), '\t');
  if (!readings.size()) {
    return;
  }

  m_userPhraseDB->execute(
      "UPDATE user_unigrams SET qstring = %Q, current = %Q WHERE rowid = %d",
      qstringFromReading(readings[0]).c_str(), phrase.c_str(), row + 1);
  m_loader->forceSyncModuleConfigForNextRound("SmartMandarin");
}

void CannedMessageFetcherDelegate::handleFetchResult(const char* data,
                                                     size_t size) {
  EnterCriticalSection(&m_representedObject->m_win32CriticalSection);
  char* buf = (char*)calloc(1, size + 1);
  memcpy(buf, data, size);

  PVPlistValue* newValue = PVPropertyList::ParsePlistFromString(buf);

  if (newValue) {
    // lock here!
    PVPlistValue* msgArray = newValue->valueForKey("CannedMessages");
    if (msgArray) {
      PVPlistValue* tmp = m_representedObject->m_cannedMessagePlistValue;
      m_representedObject->m_cannedMessagePlistValue = newValue;
      delete tmp;

      m_representedObject->m_userPersistence->populateIfValueDifferentUserDB(
          "canned_messages", buf);
    }
  }

  free(buf);
  LeaveCriticalSection(&m_representedObject->m_win32CriticalSection);
  m_representedObject->mergeCannedMessage();
}

RemoteDataFetcherDelegate* IMEServer::oneKeyDataFetchDelegate() {
  return m_oneKeyDataFetchDelegate;
}

const string IMEServer::userOneKeyPlistPath() {
  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);
  return OVPathHelper::PathCat(appDataDir, "UserOneKey.plist");
}

void IMEServer::syncUserOneKeyData() {
  if (m_userOneKeyPlist->shouldReadSync()) {
    m_userOneKeyPlist->readSync();
    mergeOneKeyData();
  }
}

void IMEServer::mergeOneKeyData() {
  EnterCriticalSection(&m_win32CriticalSection);
  m_mergedOneKeyPlistValue->removeAllKeysAndValues();
  vector<string> keys = m_oneKeyPlistValue->dictionaryKeys();
  for (vector<string>::iterator ki = keys.begin(); ki != keys.end(); ++ki) {
    m_mergedOneKeyPlistValue->setKeyValue(*ki,
                                          m_oneKeyPlistValue->valueForKey(*ki));
  }

  PVPlistValue farray(PVPlistValue::Array);
  PVPlistValue* features = m_mergedOneKeyPlistValue->valueForKey("Features");
  if (!features) {
    m_mergedOneKeyPlistValue->setKeyValue("Features", &farray);
  } else {
    if (features->type() != PVPlistValue::Array) {
      m_mergedOneKeyPlistValue->setKeyValue("Features", &farray);
    }
  }

  features = m_mergedOneKeyPlistValue->valueForKey("Features");
  // put vendor features in
  size_t fc = features->arraySize();
  for (size_t i = 0; i < fc; i++) {
    PVPlistValue* vf = features->arrayElementAtIndex(i);
    if (vf) {
      vf->setKeyValue("IsVendorFeature", "true");
    }
  }

  PVPlistValue* userDict = m_userOneKeyPlist->rootDictionary();

  PVPlistValue* userFeatures = userDict->valueForKey("Features");
  if (userFeatures) {
    if (userFeatures->type() == PVPlistValue::Array) {
      size_t as = userFeatures->arraySize();
      for (size_t ai = 0; ai < as; ai++) {
        features->addArrayElement(userFeatures->arrayElementAtIndex(ai));
      }
    }
  }

  LeaveCriticalSection(&m_win32CriticalSection);
}

void OneKeyDataFetcherDelegate::handleFetchResult(const char* data,
                                                  size_t size) {
  EnterCriticalSection(&m_representedObject->m_win32CriticalSection);
  char* buf = (char*)calloc(1, size + 1);
  memcpy(buf, data, size);

  PVPlistValue* newValue = PVPropertyList::ParsePlistFromString(buf);

  if (newValue) {
    // lock here!
    PVPlistValue* tmp = m_representedObject->m_oneKeyPlistValue;
    m_representedObject->m_oneKeyPlistValue = newValue;
    m_representedObject->m_userPersistence->populateIfValueDifferentUserDB(
        "onekey_services", buf);

    delete tmp;
  } else {
  }

  free(buf);
  LeaveCriticalSection(&m_representedObject->m_win32CriticalSection);
  m_representedObject->mergeOneKeyData();
}

void* WVLoaderHookedServiceDelegate::loaderSpecificDataObjectForName(
    const string& name) {
  if (name == "OneKeyDataCopy") {
    EnterCriticalSection(&m_representedObject->m_win32CriticalSection);
    void* copy = m_representedObject->m_mergedOneKeyPlistValue->copy();
    LeaveCriticalSection(&m_representedObject->m_win32CriticalSection);
    return copy;
  }

  if (name == "ServerSideUIDisabled") {
    return (void*)m_representedObject->m_stopUsingServerSideUI;
  }

  return 0;
}

void IMEServer::setStopUsingServerSideUI(bool stopUsing) {
  m_stopUsingServerSideUI = stopUsing;
}

bool IMEServer::serverSideUINotUsed() { return m_stopUsingServerSideUI; }

};  // namespace BaseIMEServer
