//
// TestLogKitMain.cpp
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef MACRO_STRING_TEST
#define MACRO_STRING_TEST "z! test"
#endif

#include <LFPlatform.h>
#include <OpenVanilla.h>
#include <windows.h>

using namespace OpenVanilla;
using namespace LFPlatform;

int main() {
  string tmpDir = OVDirectoryHelper::TempDirectory();
  string logFilename = OVPathHelper::PathCat(tmpDir, "test.log");

  Logger::PrepareLogger();
  Logger::UseLogFile(logFilename);
  Logger::UseConsole(true);

  Logger logger;
  logger.error("Test error message");

  Logger another("section");
  another.error("Another error message");
  another.debug("debug message");

  if (RegistryHelper::KeyExists(
          HKEY_LOCAL_MACHINE,
          "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts"))
    another.error("key exists");

  HKEY key = RegistryHelper::OpenKey(
      HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts");
  if (key) {
    vector<string> subkeys = RegistryHelper::SubkeysForKey(key);
    for (vector<string>::iterator si = subkeys.begin(); si != subkeys.end();
         ++si)
      another.error("subkey: %s", (*si).c_str());

    HKEY akey = RegistryHelper::OpenKey(key, "00000402");
    if (akey) {
      string v = RegistryHelper::QueryStringValue(akey, "Layout Text");
      another.error("value: %s", v.c_str());

      RegCloseKey(akey);
    }

    RegCloseKey(key);
  }

  logger.error(MACRO_STRING_TEST);

  return 0;
}