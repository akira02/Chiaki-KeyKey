//
// BaseIME.cpp
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

#ifndef IMECandidatePanel_h
#define IMECandidatePanel_h

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <immdev.h>
#include <tchar.h>
#include <windows.h>

#include <string>
#include <vector>

#include "IMEHelper.h"

namespace BaseIME {
using namespace std;

class IMECandidatePanel {
 public:
  static bool RegisterClass(HINSTANCE hInstance);
  static bool Create(HWND parent);
  static void Destroy();
  static IMECandidatePanel* SharedPanel();

  void show();
  void hide();
  void moveTo(int x, int y);
  void updateContent(wchar_t* candidatePageString,
                     wchar_t* candidateKeyString = 0,
                     size_t highlightIndex = 0);

 protected:
  IMECandidatePanel();

  static HINSTANCE c_sharedHInstance;
  static bool c_classRegistered;
  static IMECandidatePanel* c_sharedCandidatePanel;
  static LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

 protected:
  HWND m_window;
  IMECandidateInfo m_candidateInfo;
  vector<string> m_candidateKeys;
};
};  // namespace BaseIME

#endif