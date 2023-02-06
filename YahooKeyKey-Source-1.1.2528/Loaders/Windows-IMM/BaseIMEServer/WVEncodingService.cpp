//
// WVEncodingService.cpp
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "WVEncodingService.h"

#include <tchar.h>
#include <windows.h>

#pragma managed
namespace WVEncodingServicePrivate {
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace std;
using namespace OpenVanilla;

const string UTF8ToBig5(const string& text) {
  wstring wcp = OVUTF16::FromUTF8(text);
  Encoding ^ unicode = Encoding::Unicode;
  Encoding ^ big5 = Encoding::GetEncoding(gcnew String("big5"));
  array<unsigned char> ^ bytes = unicode->GetBytes(gcnew String(wcp.c_str()));
  array<unsigned char> ^ big5bytes = Encoding::Convert(unicode, big5, bytes);

  string result;
  for (int i = 0; i < big5bytes->Length; i++) result += (char)big5bytes[i];

  return result;
}
};  // namespace WVEncodingServicePrivate
#pragma unmanaged

namespace OpenVanilla {

WVEncodingService::WVEncodingService() {
  const wchar_t* fontName = L"PMingLiU";

  if (false) {
    // Windows Vista
    fontName = L"PMingLiU";
  }

  HDC dc = GetDC(0);

  HFONT font =
      CreateFont(0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, fontName);

  if (font) {
    HGDIOBJ old = SelectObject(dc, font);
    SelectObject(dc, font);

    DWORD size = GetFontUnicodeRanges(dc, NULL);
    LPGLYPHSET glyphset = (LPGLYPHSET)calloc(1, size);
    GetFontUnicodeRanges(dc, glyphset);

    for (DWORD index = 0; index < glyphset->cRanges; index++) {
      WCRANGE range = glyphset->ranges[index];
      // BIDebugLog("RANGE %08x, %d glyphs", range.wcLow, range.cGlyphs);

      for (wchar_t c = range.wcLow; c <= range.wcLow + range.cGlyphs; c++)
        m_systemFontSupportedCharacters.insert(c);
    }

    // BIDebugLog(">>>>>>>> total ranges: %d", glyphset->cRanges);

    free(glyphset);
    SelectObject(dc, old);
  }

  ReleaseDC(0, dc);
}

WVEncodingService::~WVEncodingService() {}

bool WVEncodingService::codepointSupportedByEncoding(const string& codepoint,
                                                     const string& encoding) {
  if (encoding == "UTF-8") {
    return true;
  } else if (encoding == "BIG-5") {
    wstring wcp = OVUTF16::FromUTF8(codepoint);
    char c;
    BOOL defaultUsed = FALSE;
    WideCharToMultiByte(950, WC_NO_BEST_FIT_CHARS, wcp.c_str(), wcp.length(),
                        &c, 0, NULL, &defaultUsed);
    return defaultUsed != TRUE;
  }

  return codepointSupportedBySystem(codepoint);
}

bool WVEncodingService::codepointSupportedBySystem(const string& codepoint) {
  UINT32 u32 = (UINT32)OVUTF8Helper::CodePointFromSingleUTF8String(codepoint);
  if (m_systemFontSupportedCharacters.find(u32) ==
      m_systemFontSupportedCharacters.end())
    return false;

  return true;
}

const vector<string> WVEncodingService::supportedEncodings() {
  vector<string> results;
  results.push_back("UTF-8");
  results.push_back("BIG-5");
  return results;
}

bool WVEncodingService::isEncodingSupported(const string& encoding) {
  if (encoding == "BIG-5" || encoding == "UTF-8") return true;

  return false;
}

bool WVEncodingService::isEncodingConversionSupported(
    const string& fromEncoding, const string& toEncoding) {
  if (fromEncoding == "UTF-8" && toEncoding == "BIG-5") return true;

  return false;
}

const pair<bool, string> WVEncodingService::convertEncoding(
    const string& fromEncoding, const string& toEncoding, const string& text) {
  if (fromEncoding == "UTF-8" && toEncoding == "BIG-5")
    return pair<bool, string>(true, WVEncodingServicePrivate::UTF8ToBig5(text));

  return pair<bool, string>(false, string());
}

};  // namespace OpenVanilla
