//
// SystemInfo.cpp
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

#include <tchar.h>
#include <windows.h>

#if defined(__APPLE__)
#include <OpenVanilla/OpenVanilla.h>
#else
#include "OpenVanilla.h"
#endif

#include "SystemInfo.h"

namespace CareService {

using namespace OpenVanilla;

const string SystemInfo::PlatformSummary() {
  string unknown = "Unknown Windows Version";
  string result;
  const size_t bufferSize = 256;

  OSVERSIONINFOEX versionInfo;
  memset(&versionInfo, 0, sizeof(versionInfo));
  versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  if (!GetVersionEx((OSVERSIONINFO*)&versionInfo)) return unknown;

  SYSTEM_INFO systemInfo;
  memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
  GetSystemInfo(&systemInfo);

  if (!(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        versionInfo.dwMajorVersion > 4)) {
    return unknown;
  }

  result += "Microsoft ";

  if (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion == 0) {
    if (versionInfo.wProductType == VER_NT_WORKSTATION)
      result += "Windows Vista ";
    else
      result += "Windows Server 2008 ";

    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
      result += "64-bit";
    else if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
      result += "32-bit";
  }

  if (versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion == 2) {
    if (versionInfo.wSuiteMask == VER_SUITE_STORAGE_SERVER)
      result += "Windows Storage Server 2003";
    else if (versionInfo.wProductType == VER_NT_WORKSTATION &&
             systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
      result += "Windows XP Professional x64 Edition";
    else
      result += "Windows Server 2003";
  }

  if (versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion == 1) {
    result += "Windows XP ";
    if (versionInfo.wSuiteMask & VER_SUITE_PERSONAL)
      result += "Home Edition";
    else
      result += "Professional";
  }

  if (versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion == 0) {
    result += "Windows 2000";
  }

  if (_tcslen(versionInfo.szCSDVersion) > 0) {
    result += " ";
    result += OVUTF8::FromUTF16(versionInfo.szCSDVersion);
  }

  stringstream sst;
  sst << " (build " << versionInfo.dwBuildNumber << ")";
  result += sst.str();

  return result;
}

};  // namespace CareService
