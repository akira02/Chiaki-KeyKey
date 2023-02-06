//
// ProcessHelper.h
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef ProcessHelper_h
#define ProcessHelper_h

#include <string>

namespace LFPlatform {
using namespace std;

class ProcessHelper {
 public:
  static bool LaunchProgram(const string& executable, const string& launchPath);
  static const string CurrentProcessImageName();

  static bool IsSystemUser();
  static bool ProcessNameExistsForCurrentUser(const string& expression,
                                              size_t* count = 0);

  static bool IsRunningUnderWow64();
};
};  // namespace LFPlatform

#endif