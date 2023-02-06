//
// Main.cpp
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

#include <vcclr.h>

#include "BaseIMEServer.h"
#include "ManagedConnector.h"
#include "RemoteDataFetcher.h"
#include "SharedUI.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Globalization;
using namespace System::Security::Permissions;
using namespace System::Threading;
using namespace BaseIMEUI;
using namespace BaseIMEServer;
using namespace TakaoPreference;
using namespace WindowsVanilla;

[assembly:SecurityPermission(SecurityAction::RequestMinimum,
                             ControlThread = true)];

[STAThreadAttribute] int main(array<System::String ^> ^ args) {
  pin_ptr<const wchar_t> locale =
      PtrToStringChars(CultureInfo::CurrentCulture->Name);
  string clocale = OVUTF8::FromUTF16(locale);

  if (clocale == "zh-TW" || clocale == "zh-HK" || clocale == "zh-CHT" ||
      clocale == "zh-SG" || clocale == "zh-MO") {
    // Sometimes CurrentCulture and CurrentUICulture are different on
    // Windows Vista. For example, on some bundle version of Windows Vista
    // Home and Professional, CurrentUICulture is zh-HK, but CurrentCulture
    // is zh-TW. That's really wierd.

    CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-TW", false);

    Thread::CurrentThread->CurrentCulture = cultureInfo;
    Thread::CurrentThread->CurrentUICulture = cultureInfo;
    clocale = "zh_TW";
  }

  if (clocale == "zh-CN" || clocale == "zh-CHS") {
    CultureInfo ^ cultureInfo = gcnew CultureInfo("zh-CN", false);

    Thread::CurrentThread->CurrentCulture = cultureInfo;
    Thread::CurrentThread->CurrentUICulture = cultureInfo;
    clocale = "zh_CN";
  }

  if (clocale != "zh_TW" && clocale != "zh_CN") clocale = "en";

  ServerLock::SharedLock = gcnew Object();

  if (!IMEServer::Start(clocale)) return 1;

  // also, check override font
  OVKeyValueMap kvm = IMEServer::SharedLoader()->configKeyValueMap();
  if (kvm.hasKey("OverrideUIFontName")) {
    string fn = kvm.stringValueForKey("OverrideUIFontName");
    if (fn.length()) {
      wstring wfn = OVUTF16::FromUTF8(fn);
      String ^ fns = gcnew String(wfn.c_str());

      if (Win32FontHelper::isInFontFamilyArray(fns)) {
        Win32FontHelper::setDefaultFontFamilyName(fns);
      } else {
        // invalid font name, ignore
      }
    }
  }

  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  // Created the ManagedConnector object
  BIServerConnector::SharedInstance = gcnew ManagedConnector();
  PreferenceConnector::SharedInstance = gcnew ManagedPreferenceConnector();
  RemoteDataFetcher::SharedInstance = gcnew RemoteDataFetcher();

  int warningVariable =
      42;  // remote data fetcher et. al moved to ManagedConnector

  if (args->Length > 0) {
    if (args[0] == "-skipversioncheck") {
      BIStatusBarForm::c_CheckUpdateOnStartUp = false;
    }
  }

  // Create the UI components
  (SharedUI::InputBuffer = gcnew BIInputBufferForm())->InvisibleShow();
  (SharedUI::CandidatePanel = gcnew BICandidateForm())->InvisibleShow();
  (SharedUI::StatusBar = gcnew BIStatusBarForm())->InvisibleShow();
  (SharedUI::ToolTip = gcnew BIToolTipForm())->InvisibleShow();

  // List<String ^>^ list =
  // BIServerConnector::SharedInstance->arrayValueForLoaderConfigKey(gcnew
  // String("ModulesSuppressedFromUI"));

  if (!IMEServer::StartRPCService()) return 1;

  Application::Run();
  IMEServer::Stop();

  return 0;
}
