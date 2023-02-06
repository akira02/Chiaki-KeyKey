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

#include "IMECandidatePanel.h"

#include <LogKit.h>
#include <OpenVanilla.h>

#include "IMEHelper.h"

#define BASEIME_CANDIDATE_PANEL_CLASS "IMECandidatePanel"

namespace BaseIME {

using namespace OpenVanilla;
using namespace LFPlatform;

HINSTANCE IMECandidatePanel::c_sharedHInstance = 0;
bool IMECandidatePanel::c_classRegistered = false;
IMECandidatePanel* IMECandidatePanel::c_sharedCandidatePanel = 0;

LRESULT IMECandidatePanel::WindowProc(HWND hWnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
  // Logger logger("IMECandidatePanel");
  // logger.debug("WindowProc %d", msg);

  IMECandidatePanel* panel = (IMECandidatePanel*)GetWindowLongPtr(hWnd, 0);

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      RECT rect;
      GetClientRect(hWnd, &rect);
      // logger.debug("WM_PAINT %d,%d,%d,%d", rect.left, rect.top, rect.right,
      // rect.bottom);

      HDC hDC = ps.hdc;

      // FillRect(hDC, &rect, (HBRUSH)(COLOR_BACKGROUND + 1));
      FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

      long fontHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
      long keyHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
      HFONT font = CreateFont(fontHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0,
                              0, 0, 0, 0, L"MingLiU");
      HFONT keyFont = CreateFont(keyHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE,
                                 0, 0, 0, 0, 0, L"Arial");
      HPEN whitePen = (HPEN)GetStockObject(WHITE_PEN);
      SelectObject(hDC, whitePen);

      DWORD i = 0;

      SetBkMode(hDC, TRANSPARENT);
      SetBkColor(hDC, RGB(0, 0, 0));

      for (i = 0; i < panel->m_candidateInfo.candidateList.dwCount; i++) {
        // logger.debug("drawing: %d", i);

        RECT keyBackgoundRect;
        keyBackgoundRect.top = i * 18 + 12;
        keyBackgoundRect.bottom = i * 18 + 26;
        keyBackgoundRect.left = 6;
        keyBackgoundRect.right = 18;

        if (i == panel->m_candidateInfo.candidateList.dwSelection) {
          RECT highlightRect;
          highlightRect.top = i * 18 + 10;
          highlightRect.bottom = i * 18 + 28;
          highlightRect.left = 0;
          highlightRect.right = rect.right;
          HBRUSH purpleBrush = CreateSolidBrush(RGB(140, 91, 156));
          FillRect(hDC, &highlightRect, purpleBrush);
          DeleteObject(purpleBrush);
          HBRUSH lightPurpleBrush = CreateSolidBrush(RGB(234, 209, 239));
          FillRect(hDC, &keyBackgoundRect, lightPurpleBrush);
          DeleteObject(lightPurpleBrush);
        } else {
          HBRUSH grayBrush = CreateSolidBrush(RGB(154, 154, 154));
          FillRect(hDC, &keyBackgoundRect, grayBrush);
          DeleteObject(grayBrush);
        }

        RECT keyRect;
        keyRect.top = i * 18 + 13;
        keyRect.bottom = i * 18 + 28;
        keyRect.left = 0;
        keyRect.right = 25;

        string ck = "X";
        if (i < panel->m_candidateKeys.size()) {
          ck = panel->m_candidateKeys[i];
        }
        wstring wck = OVUTF16::FromUTF8(ck);
        const wchar_t* keyString = wck.c_str();
        SetTextColor(hDC, RGB(80, 80, 80));
        SelectObject(hDC, keyFont);
        DrawText(hDC, keyString, 1, &keyRect,
                 DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

        RECT textRect;
        textRect.top = i * 18 + 10;
        textRect.bottom = i * 18 + 28;
        textRect.left = 25;
        textRect.right = rect.right;

        wchar_t* s = panel->m_candidateInfo.candidateStrings[i];

        // SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(255, 255, 255));
        SelectObject(hDC, font);
        DrawText(hDC, s, wcslen(s), &textRect,
                 DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
      }

      DeleteObject(font);
      DeleteObject(keyFont);

      POINT lines[5];
      lines[0].x = rect.left;
      lines[0].y = rect.top;
      lines[1].x = rect.right - 1;
      lines[1].y = rect.top;
      lines[2].x = rect.right - 1;
      lines[2].y = rect.bottom - 1;
      lines[3].x = rect.left;
      lines[3].y = rect.bottom - 1;
      lines[4].x = rect.left;
      lines[4].y = rect.top;

      HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
      SelectObject(hDC, borderPen);
      Polyline(hDC, lines, 5);
      SelectObject(hDC, GetStockObject(BLACK_PEN));
      DeleteObject(borderPen);

      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_ERASEBKGND:
      // logger.debug("WM_ERASEBKGND");
      return TRUE;
    case WM_MOUSEACTIVATE:
      // logger.debug("WM_MOUSEACTIVATE");
      return MA_NOACTIVATE;
  }

  if ((msg >= WM_IME_STARTCOMPOSITION && msg <= WM_IME_KEYLAST) ||
      (msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP)) {
    // logger.debug("IME messages");
    return 0;
  }

  if (msg == WM_DESTROY) {
    // logger.debug("!!! WM_DESTROY!!!");
  }

  // logger.debug("Default messages");
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool IMECandidatePanel::RegisterClass(HINSTANCE hInstance) {
  Logger logger("IMECandidatePanel");
  logger.debug("RegisterClass %08x", hInstance);

  static wstring className = OVUTF16::FromUTF8(BASEIME_CANDIDATE_PANEL_CLASS);
  c_sharedHInstance = hInstance;

  if (!c_classRegistered) {
    c_classRegistered = true;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_IME;
    wc.lpfnWndProc = (WNDPROC)IMECandidatePanel::WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LONG_PTR);
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL;
    wc.lpszMenuName = (LPTSTR)NULL;
    wc.lpszClassName = className.c_str();
    wc.hbrBackground = NULL;
    wc.hIconSm = NULL;

    return !!RegisterClassEx((LPWNDCLASSEX)&wc);
  }

  return false;
}

bool IMECandidatePanel::Create(HWND parent) {
  if (!c_sharedCandidatePanel) {
    if (!c_classRegistered) return false;

    c_sharedCandidatePanel = new IMECandidatePanel;
  }

  wstring className = OVUTF16::FromUTF8(BASEIME_CANDIDATE_PANEL_CLASS);

  c_sharedCandidatePanel->m_window = CreateWindowEx(
      0, className.c_str(), NULL,
      WS_POPUP | WS_CLIPCHILDREN | WS_EX_TOPMOST | WS_EX_NOACTIVATE, 0, 0, 450,
      150, parent, NULL, c_sharedHInstance, NULL);

  SetWindowLongPtr(c_sharedCandidatePanel->m_window, 0,
                   (LONG_PTR)c_sharedCandidatePanel);
  ShowWindow(c_sharedCandidatePanel->m_window, SW_HIDE);
  return true;
}

void IMECandidatePanel::Destroy() {
  if (c_sharedCandidatePanel) {
    c_sharedCandidatePanel->m_window = 0;
  }
}

IMECandidatePanel* IMECandidatePanel::SharedPanel() {
  return c_sharedCandidatePanel;
}

void IMECandidatePanel::show() {
  if (!m_window) return;
  ShowWindow(m_window, SW_SHOWNA);
}

void IMECandidatePanel::hide() {
  if (!m_window) return;
  ShowWindow(m_window, SW_HIDE);
}

void IMECandidatePanel::moveTo(int x, int y) {
  if (!m_window) return;

  RECT windowRect;
  GetWindowRect(m_window, &windowRect);
  MoveWindow(m_window, x, y, windowRect.right - windowRect.left,
             windowRect.bottom - windowRect.top, TRUE);
}

void IMECandidatePanel::updateContent(wchar_t* candidatePageString,
                                      wchar_t* candidateKeyString,
                                      size_t highlightIndex) {
  if (!m_window) return;

  if (candidatePageString) {
    m_candidateInfo.setCandidatesFromContinuousString(candidatePageString);
  }
  int count = c_sharedCandidatePanel->m_candidateInfo.candidateList.dwCount;

  if (!count) {
    hide();
  }

  // SIZE_T candidateLength = 0;

  HDC hDC = GetDC(m_window);
  long fontHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  HFONT font = CreateFont(fontHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0,
                          0, 0, 0, L"MingLiU");
  SelectObject(hDC, font);
  SIZE size;
  long textWidth = 0;

  for (int i = 0; i < count; i++) {
    wchar_t* s = c_sharedCandidatePanel->m_candidateInfo.candidateStrings[i];
    GetTextExtentPoint32W(hDC, s, wcslen(s), &size);
    if (size.cx > textWidth) {
      textWidth = size.cx;
    }
  }
  DeleteObject(font);
  ReleaseDC(m_window, hDC);

  int h = 18 * count + 20;
  int w = 40 + textWidth;

  if (candidateKeyString) {
    string ks = OVUTF8::FromUTF16(candidateKeyString);
    m_candidateKeys = OVUTF8Helper::SplitStringByCodePoint(ks);
  } else {
    m_candidateKeys.clear();
  }

  m_candidateInfo.setHighlightedIndex(highlightIndex);
  SetWindowPos(m_window, NULL, 0, 0, w, h,
               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
  InvalidateRect(m_window, NULL, FALSE);

  RECT windowRect;
  GetWindowRect(m_window, &windowRect);

  HDC screenDC = GetDC(NULL);
  int screenWidth = GetDeviceCaps(screenDC, HORZRES);
  int screenHeight = GetDeviceCaps(screenDC, VERTRES);
  ReleaseDC(NULL, screenDC);

  int oldX = windowRect.left;
  int oldY = windowRect.top;

  if (windowRect.right > screenWidth) {
    windowRect.left -= windowRect.right - screenWidth;
  }

  if (windowRect.bottom > (screenHeight - 30)) {
    windowRect.top -= windowRect.bottom - (screenHeight - 30);
  }

  if (windowRect.left != oldX || windowRect.top != oldY) {
    moveTo(windowRect.left, windowRect.top);
  }
}

IMECandidatePanel::IMECandidatePanel() : m_window(0) {}

};  // namespace BaseIME
