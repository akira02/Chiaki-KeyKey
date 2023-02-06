//
// RegistryHelper.cpp
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#include "RegistryHelper.h"

#include <OpenVanilla.h>
#include <stdio.h>
#include <tchar.h>

#include <sstream>

#include "RegistryHelper.h"

namespace LFPlatform {

using namespace OpenVanilla;

bool RegistryHelper::KeyExists(HKEY prefix, const string& key) {
  HKEY hkey;
  if (ERROR_SUCCESS == RegOpenKeyEx(prefix, OVUTF16::FromUTF8(key).c_str(), 0,
                                    KEY_READ, &hkey)) {
    RegCloseKey(hkey);
    return true;
  }

  return false;
}

HKEY RegistryHelper::OpenKey(HKEY prefix, const string& key) {
  HKEY hkey;
  if (ERROR_SUCCESS ==
      RegOpenKeyEx(prefix, OVUTF16::FromUTF8(key).c_str(), 0, KEY_READ, &hkey))
    return hkey;

  return (HKEY)0;
}

const vector<string> RegistryHelper::SubkeysForKey(HKEY key) {
  vector<string> results;
  wchar_t* keyname =
      new wchar_t[256];  // 255 = max length of registry key length

  if (keyname) {
    DWORD index = 0;
    while (1) {
      FILETIME updateTime;
      DWORD length;

      wmemset(keyname, 0, 256);
      length = 255;
      DWORD rsp = RegEnumKeyEx(key, index, keyname, &length, NULL, NULL, NULL,
                               &updateTime);
      if (rsp != ERROR_SUCCESS && rsp != ERROR_MORE_DATA) break;

      index++;
      results.push_back(OVUTF8::FromUTF16(keyname));
    }

    delete[] keyname;
  }

  return results;
}

const string RegistryHelper::QueryStringValue(HKEY key,
                                              const string& nameOfValue) {
  string result;
  DWORD type;
  DWORD size = 1024;
  wchar_t* data = new wchar_t[(size_t)size];
  wmemset(data, 0, (size_t)size);

  size = size * sizeof(wchar_t);

  DWORD rsp = RegQueryValueEx(key, OVUTF16::FromUTF8(nameOfValue).c_str(), NULL,
                              &type, (LPBYTE)data, &size);
  if (rsp == ERROR_SUCCESS) {
    if (type == REG_EXPAND_SZ || type == REG_SZ)
      result = OVUTF8::FromUTF16(data);
  }

  delete[] data;
  return result;
}

};  // namespace LFPlatform
