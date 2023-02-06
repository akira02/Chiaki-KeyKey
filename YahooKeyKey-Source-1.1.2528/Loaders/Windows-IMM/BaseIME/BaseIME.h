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

#ifndef BaseIME_H
#define BaseIME_H

#define WIN32_LEAN_AND_MEAN
#include <LFPlatform.h>
#include <OpenVanilla.h>
#include <immdev.h>
#include <tchar.h>
#include <windows.h>

#include "BIConfig.h"

#endif

// These functions need exporting, or add these into the export def file:
//
// ImeConversionList
// ImeConfigure
// ImeDestroy
// ImeEscape
// ImeInquire
// ImeProcessKey
// ImeSelect
// ImeSetActiveContext
// ImeToAsciiEx
// NotifyIME
// ImeRegisterWord
// ImeUnregisterWord
// ImeGetRegisterWordStyle
// ImeEnumRegisterWord
// ImeSetCompositionString
// ImeGetImeMenuItems
// UIWndProc
//
//
// An IME implements these DLL callbacks:
//
// BOOL WINAPI ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode, LPVOID
// pRegisterWord); BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR
// lpszUIClass, LPCTSTR lpszOptions); BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT
// uVirKey, LPARAM lParam, CONST BYTE *lpbKeyState ); BOOL WINAPI ImeSelect(HIMC
// hIMC, BOOL fSelect); BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD
// dwIndex, DWORD dwValue);
//
//
// Pracitcally empty callbacks:
//
// DWORD WINAPI ImeConversionList(HIMC hIMC, LPCTSTR lpSource, LPCANDIDATELIST
// lpCandList, DWORD dwBufLen, UINT uFlag); BOOL WINAPI ImeDestroy(UINT uForce);
// LRESULT WINAPI ImeEscape(HIMC hIMC, UINT uSubFunc, LPVOID lpData);
// BOOL WINAPI ImeSetActiveContext(HIMC hIMC, BOOL fFlag);
// UINT WINAPI ImeToAsciiEx(UINT uVirtKey, UINT uScaCode, CONST LPBYTE
// lpbKeyState, LPDWORD lpdwTransBuf, UINT fuState, HIMC); BOOL WINAPI
// ImeRegisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr); BOOL WINAPI
// ImeUnregisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr); UINT WINAPI
// ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF lp); UINT WINAPI
// ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfn, LPCTSTR lpRead, DWORD dw,
// LPCTSTR lpStr, LPVOID lpData); BOOL WINAPI ImeSetCompositionString(HIMC hIMC,
// DWORD dwIndex, LPCVOID lpComp, DWORD dwComp, LPCVOID lpRead, DWORD dwRead);
// DWORD WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType,
// LPIMEMENUITEMINFO lpImeParentMenu, LPIMEMENUITEMINFO lpImeMenu, DWORD
// dwSize);
