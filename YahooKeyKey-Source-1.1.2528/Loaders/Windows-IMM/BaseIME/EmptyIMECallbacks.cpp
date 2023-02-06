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

#include "BaseIME.h"
#include "OpenVanilla.h"

using namespace OpenVanilla;

void EmptyIMMCallbackLog(const char* message) {
  LFPlatform::Logger logger("EmptyIMMCallbacks");
  logger.debug(message);
}

BOOL WINAPI ImeSetActiveContext(HIMC hIMC, BOOL fFlag) {
  LFPlatform::Logger logger("ImeSetActiveContext");
  logger.debug("hIMC = 0x%08x, flag = %s", hIMC, (fFlag ? "TRUE" : "FALSE"));
  return TRUE;
}

LRESULT WINAPI ImeEscape(HIMC hIMC, UINT uSubFunc, LPVOID lpData) {
  LFPlatform::Logger logger("ImeEscape");
  LRESULT lRet = FALSE;

  switch (uSubFunc) {
    case IME_ESC_QUERY_SUPPORT:
      logger.debug("\tIME_ESC_QUERY_SUPPORT");
      switch (*(LPUINT)lpData) {
        case IME_ESC_QUERY_SUPPORT:
          logger.debug("IME_ESC_QUERY_SUPPORT: IME_ESC_QUERY_SUPPORT");
          break;
        case IME_ESC_IME_NAME:
          logger.debug("IME_ESC_QUERY_SUPPORT: IME_ESC_IME_NAME");
          break;
        case IME_ESC_GETHELPFILENAME:
          logger.debug("IME_ESC_QUERY_SUPPORT: IME_ESC_GETHELPFILENAME");
          lRet = TRUE;
          break;
        default:
          logger.debug("IME_ESC_QUERY_SUPPORT: unknown lpData: 0x%08x",
                       *(LPUINT)lpData);
          break;
      }
      break;
    case IME_ESC_IME_NAME: {
      wstring ws = OVUTF16::FromUTF8("\xe9\x9b\x85\xe8\x99\x8e");
      logger.debug("IME_ESC_IME_NAME");
      wcscpy_s((wchar_t*)lpData, 3, ws.c_str());
      lRet = TRUE;
      break;
    }
    case IME_ESC_GETHELPFILENAME:
      logger.debug("IME_ESC_GETHELPFILENAME");
      wcscpy_s((wchar_t*)lpData, 12, L"BaseIME.hlp");
      lRet = TRUE;
      break;
    default:
      logger.debug("Other unknown func type: 0x%08x", uSubFunc);
      lRet = FALSE;
      break;
  }

  return lRet;
}

DWORD WINAPI ImeConversionList(HIMC hIMC, LPCTSTR lpSource,
                               LPCANDIDATELIST lpCandList, DWORD dwBufLen,
                               UINT uFlag) {
  EmptyIMMCallbackLog("ImeConversionList");
  return 0;
}

BOOL WINAPI ImeDestroy(UINT uForce) {
  EmptyIMMCallbackLog("ImeDestroy");
  return TRUE;
}

UINT WINAPI ImeToAsciiEx(UINT uVirtKey, UINT uScaCode, CONST LPBYTE lpbKeyState,
                         LPDWORD lpdwTransBuf, UINT fuState, HIMC) {
  EmptyIMMCallbackLog("ImeToAsciiEx");
  return 0;
}

BOOL WINAPI ImeRegisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr) {
  EmptyIMMCallbackLog("ImeRegisterWord");
  return FALSE;
}

BOOL WINAPI ImeUnregisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr) {
  EmptyIMMCallbackLog("ImeUnregisterWord");
  return FALSE;
}

UINT WINAPI ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF lp) {
  EmptyIMMCallbackLog("ImeGetRegisterWordStyle");
  return 0;
}

UINT WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfn, LPCTSTR lpRead,
                                DWORD dw, LPCTSTR lpStr, LPVOID lpData) {
  EmptyIMMCallbackLog("ImeEnumRegisterWord");
  return 0;
}

BOOL WINAPI ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPCVOID lpComp,
                                    DWORD dwComp, LPCVOID lpRead,
                                    DWORD dwRead) {
  EmptyIMMCallbackLog("ImeSetCompositionString");
  return FALSE;
}

DWORD WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType,
                                LPIMEMENUITEMINFO lpImeParentMenu,
                                LPIMEMENUITEMINFO lpImeMenu, DWORD dwSize) {
  EmptyIMMCallbackLog("ImeGetImeMenuItems");
  return 0;
}
