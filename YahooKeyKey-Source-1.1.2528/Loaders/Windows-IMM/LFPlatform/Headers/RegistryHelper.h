//
// RegistryHelper.h
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef RegistryHelper_h
#define RegistryHelper_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <string>
#include <vector>

namespace LFPlatform {
using namespace std;

class RegistryHelper {
 public:
  static bool KeyExists(HKEY prefix, const string& key);
  static HKEY OpenKey(HKEY prefix, const string& key);
  static const vector<string> SubkeysForKey(HKEY key);
  static const string QueryStringValue(HKEY key, const string& nameOfValue);
};
};  // namespace LFPlatform

#endif