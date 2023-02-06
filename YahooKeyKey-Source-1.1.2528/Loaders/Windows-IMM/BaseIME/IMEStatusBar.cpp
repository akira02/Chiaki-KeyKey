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

#include "IMEStatusBar.h"

#include <LogKit.h>
#include <OpenVanilla.h>

#include "IMEHelper.h"
#include "resource.h"

#define BASEIME_STATUS_BAR_CLASS "IMEStatusBar"

namespace BaseIME {

using namespace OpenVanilla;
using namespace LFPlatform;

HINSTANCE IMEStatusBar::c_sharedHInstance = 0;
bool IMEStatusBar::lastPosSet = false;
int IMEStatusBar::lastPosX = 0;
int IMEStatusBar::lastPosY = 0;
bool IMEStatusBar::c_classRegistered = false;
wstring* IMEStatusBar::c_serverLocale = 0;
IMEStatusBar* IMEStatusBar::c_sharedStatusBar = 0;

wstring IMEStatusBar::ServerLocale() {
  wstring locale = L"en";
  if (!c_serverLocale) {
    c_serverLocale = new wstring(L"en");
  }

  locale = *c_serverLocale;
  return locale;
}

void IMEStatusBar::SetServerLocale(const wchar_t* locale) {
  if (!c_serverLocale) {
    c_serverLocale = new wstring;
  }

  (*c_serverLocale) = locale;
}

LRESULT IMEStatusBar::WindowProc(HWND hWnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam) {
  IMEStatusBar* bar = (IMEStatusBar*)GetWindowLongPtr(hWnd, 0);

  HBITMAP g_backgroundHead = NULL;
  HBITMAP g_backgroundBody = NULL;
  HBITMAP g_backgroundTail = NULL;

  switch (msg) {
    case WM_LBUTTONDOWN: {
      // detect if in the "message area"
      POINT p;
      p.x = (int)(short)LOWORD(lParam);  // horizontal position
      p.y = (int)(short)HIWORD(lParam);  // vertical position
      if (p.x > bar->m_buttonRect.left && p.x < bar->m_buttonRect.right &&
          p.y > bar->m_buttonRect.top && p.y < bar->m_buttonRect.bottom) {
      } else {
// http://www.pcreview.co.uk/forums/thread-2611928.php
#define HT_CAPTION 0x2
        ReleaseCapture();
        SendMessage(hWnd, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
      }

      return 1;
    }

    case WM_LBUTTONUP: {
      if (ServerLocale() == L"en") {
        MessageBox(
            NULL,
            L"Some applications are not developed to work with input methods "
            L"like Yahoo! KeyKey. This compatibility mode allows KeyKey to "
            L"work around their problem with reduced user interface.\n\nTo "
            L"change input method, switch to another app, make the change then "
            L"come back to the present app.",
            L"Yahoo! KeyKey Compatibility Mode", MB_OK);
      } else {
        wstring title = OVUTF16::FromUTF8(
            "\xE8\xBC\xB8\xE5\x85\xA5\xE6\xB3\x95\xE7\x9B\xB8\xE5\xAE\xB9\xE6"
            "\xA8\xA1\xE5\xBC\x8F\xE8\xAA\xAA\xE6\x98\x8E");

        wstring text = OVUTF16::FromUTF8(
            "\xe4\xb8\x80\xe4\xba\x9b\xe6\x87\x89\xe7\x94\xa8\xe7\xa8\x8b\xe5"
            "\xbc\x8f\xe5\x9c\xa8\xe9\x96\x8b\xe7\x99\xbc\xe7\x9a\x84\xe6\x99"
            "\x82\xe5\x80\x99\xef\xbc\x8c\xe4\xb8\xa6\xe6\x9c\xaa\xe8\x80\x83"
            "\xe6\x85\xae\xe5\x88\xb0\xe8\x88\x87\xe5\x90\x84\xe7\xa8\xae\xe8"
            "\xbc\xb8\xe5\x85\xa5\xe6\xb3\x95\xe7\x9a\x84\xe7\x9b\xb8\xe5\xae"
            "\xb9\xe6\x80\xa7\xef\xbc\x8c\xe4\xbb\xa5\xe8\x87\xb4\xe5\x9c\xa8"
            "\xe4\xbd\xbf\xe7\x94\xa8\xe6\x99\x82\xe5\x8f\xaf\xe8\x83\xbd\xe6"
            "\x9c\x83\xe7\x99\xbc\xe7\x94\x9f\xe7\x84\xa1\xe6\xb3\x95\xe8\xbc"
            "\xb8\xe5\x85\xa5\xef\xbc\x8c\xe6\x88\x96\xe5\x8a\x9f\xe8\x83\xbd"
            "\xe4\xb8\x8d\xe5\xae\x8c\xe5\x85\xa8\xe7\x9a\x84\xe5\x95\x8f\xe9"
            "\xa1\x8c\xe3\x80\x82\n\n\xe7\x82\xba\xe4\xba\x86\xe8\xa7\xa3\xe6"
            "\xb1\xba\xe6\xad\xa4\xe4\xbb\xa5\xe7\x8f\xbe\xe8\xb1\xa1\xef\xbc"
            "\x8c\xe9\x9b\x85\xe8\x99\x8e\xe5\xa5\x87\xe6\x91\xa9\xe8\xbc\xb8"
            "\xe5\x85\xa5\xe6\xb3\x95\xe5\x9c\xa8\xe9\x80\x99\xe9\xa1\x9e\xe7"
            "\x9a\x84\xe6\x87\x89\xe7\x94\xa8\xe7\xa8\x8b\xe5\xbc\x8f\xe4\xb8"
            "\xad\xef\xbc\x8c\xe6\x9c\x83\xe6\x8e\xa1\xe5\x8f\x96\xe3\x80\x8c"
            "\xe7\x9b\xb8\xe5\xae\xb9\xe6\xa8\xa1\xe5\xbc\x8f\xe3\x80\x8d("
            "Compatibility "
            "Mode)"
            "\xef\xbc\x8c\xe6\x8f\x90\xe4\xbe\x9b\xe6\x9c\x80\xe5\x9f\xba\xe6"
            "\x9c\xac\xe7\x9a\x84\xe8\xbc\xb8\xe5\x85\xa5\xe6\xb3\x95\xe5\x8a"
            "\x9f\xe8\x83\xbd\xe5\x92\x8c\xe4\xbb\x8b\xe9\x9d\xa2\xe3\x80\x82\n"
            "\n\xe5\x9c\xa8\xe7\x9b\xb8\xe5\xae\xb9\xe6\xa8\xa1\xe5\xbc\x8f\xe4"
            "\xb8\xad\xef\xbc\x8c\xe6\x9f\x90\xe4\xba\x9b\xe7\x89\xb9\xe5\xae"
            "\x9a\xe5\x8a\x9f\xe8\x83\xbd\xe5\xb0\x87\xe4\xb8\x8d\xe6\x8f\x90"
            "\xe4\xbe\x9b\xe3\x80\x82\xe5\x90\x8c\xe6\x99\x82\xe4\xbb\x8b\xe9"
            "\x9d\xa2\xe4\xb9\x9f\xe7\xb2\xbe\xe7\xb0\xa1\xe5\x8c\x96\xef\xbc"
            "\x8c\xe4\xbb\xa5\xe9\x81\x94\xe5\x88\xb0\xe6\xbb\xbf\xe8\xb6\xb3"
            "\xe8\xbc\xb8\xe5\x85\xa5\xe6\x96\x87\xe5\xad\x97\xe7\x9a\x84\xe6"
            "\x9c\x80\xe5\x9f\xba\xe6\x9c\xac\xe9\x9c\x80\xe6\xb1\x82\xe3\x80"
            "\x82\n\n\xe5\xa6\x82\xe6\x9e\x9c\xe6\x82\xa8\xe9\x9c\x80\xe8\xa6"
            "\x81\xe6\x9b\xb4\xe6\x8f\x9b\xe7\x9b\xae\xe5\x89\x8d\xe4\xbd\xbf"
            "\xe7\x94\xa8\xe7\x9a\x84\xe8\xbc\xb8\xe5\x85\xa5\xe6\xb3\x95\xe6"
            "\x88\x96\xe6\x98\xaf\xe6\x94\xb9\xe8\xae\x8a\xe5\x85\xa8\xe5\x9e"
            "\x8b/"
            "\xe5\x8d\x8a\xe5\x9e\x8b\xe6\xa8\xa1\xe5\xbc\x8f\xef\xbc\x8c\xe8"
            "\xab\x8b\xe5\x88\x87\xe6\x8f\x9b\xe8\x87\xb3\xe5\x85\xb6\xe4\xbb"
            "\x96\xe6\x87\x89\xe7\x94\xa8\xe7\xa8\x8b\xe5\xbc\x8f\xef\xbc\x8c"
            "\xe6\x9b\xb4\xe6\x8f\x9b\xe5\xbe\x8c\xe5\x86\x8d\xe5\x9b\x9e\xe5"
            "\x88\xb0\xe7\x9b\xae\xe5\x89\x8d\xe7\x9a\x84\xe6\x87\x89\xe7\x94"
            "\xa8\xe7\xa8\x8b\xe5\xbc\x8f\xe5\x8d\xb3\xe5\x8f\xaf\xe3\x80\x82");

        MessageBox(NULL, text.c_str(), title.c_str(), MB_OK);
      }
      return 1;
    }

    case WM_MOVING: {
      RECT* r = (RECT*)lParam;
      /*
                      p.x = (int)(short) LOWORD(lParam);   // horizontal
         position p.y = (int)(short) HIWORD(lParam);   // vertical position
              // ClientToScreen(hWnd, &p);

              lastPosX = p.x;
              lastPosY = p.y;
                      lastPosSet = true; */
      lastPosX = r->left;
      lastPosY = r->top;

      return 1;
    }

    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);

      HDC hDC = ps.hdc;

      RECT rect;
      GetClientRect(hWnd, &rect);

      FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

      if (g_backgroundBody == NULL)
        g_backgroundBody = LoadBitmap(IMEStatusBar::c_sharedHInstance,
                                      MAKEINTRESOURCE(IDB_BITMAP_BG));
      HDC hdcMem = CreateCompatibleDC(hDC);
      SelectObject(hdcMem, g_backgroundBody);
      BITMAP bm;
      GetObject(g_backgroundBody, sizeof(bm), &bm);

      StretchBlt(hDC, 0, 0, rect.right, 23, hdcMem, 0, 0, bm.bmWidth,
                 bm.bmHeight, SRCCOPY);

      if (g_backgroundHead == NULL)
        g_backgroundHead = LoadBitmap(IMEStatusBar::c_sharedHInstance,
                                      MAKEINTRESOURCE(IDB_BITMAP_HEAD));

      SelectObject(hdcMem, g_backgroundHead);
      GetObject(g_backgroundHead, sizeof(bm), &bm);
      BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
      RECT textRect = rect;
      textRect.left = bm.bmWidth + 10;
      textRect.top = 3;

      if (g_backgroundTail == NULL)
        g_backgroundTail = LoadBitmap(IMEStatusBar::c_sharedHInstance,
                                      MAKEINTRESOURCE(IDB_BITMAP_TAIL));

      SelectObject(hdcMem, g_backgroundTail);
      GetObject(g_backgroundTail, sizeof(bm), &bm);
      BitBlt(hDC, rect.right - bm.bmWidth, 0, bm.bmWidth, bm.bmHeight, hdcMem,
             0, 0, SRCCOPY);

      DeleteDC(hdcMem);

      long fontSize = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
      HFONT font = CreateFont(fontSize, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0,
                              0, 0, 0, L"Arial");
      HPEN whitePen = (HPEN)GetStockObject(WHITE_PEN);

      SelectObject(hDC, whitePen);
      SelectObject(hDC, font);

      SetBkMode(hDC, TRANSPARENT);
      SetBkColor(hDC, RGB(0, 0, 0));
      SetTextColor(hDC, RGB(255, 255, 255));
      const wchar_t* imn = bar->m_currentInputMethodName.c_str();
      if (wcslen(imn)) {
        DrawText(hDC, imn, wcslen(imn), &textRect,
                 DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
      }

      long infoFontHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
      HFONT infoFont = CreateFont(infoFontHeight, 0, 0, 0, 500, FALSE, FALSE,
                                  FALSE, 0, 0, 0, 0, 0, L"Arial");

      RECT buttonRect = textRect;
      buttonRect.top = 4;
      SIZE size;
      GetTextExtentPoint32W(hDC, bar->m_currentInputMethodName.c_str(),
                            bar->m_currentInputMethodName.length(), &size);
      buttonRect.left = textRect.left + size.cx + 15;
      SelectObject(hDC, infoFont);
      GetTextExtentPoint32W(hDC, bar->m_infoString.c_str(),
                            bar->m_infoString.length(), &size);
      buttonRect.right = buttonRect.left + size.cx;
      buttonRect.bottom = buttonRect.top + size.cy;
      DrawText(hDC, bar->m_infoString.c_str(), bar->m_infoString.length(),
               &buttonRect, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
      bar->m_buttonRect = buttonRect;

      DeleteObject(font);
      DeleteObject(infoFont);

      POINT lines[2];
      lines[0].x = buttonRect.left;
      lines[0].y = buttonRect.bottom - 1;
      lines[1].x = buttonRect.right;
      lines[1].y = buttonRect.bottom - 1;

      HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      SelectObject(hDC, borderPen);
      Polyline(hDC, lines, 2);
      DeleteObject(borderPen);

      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_ERASEBKGND:
      return TRUE;
    case WM_MOUSEACTIVATE:
      return MA_NOACTIVATE;

    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }
}

bool IMEStatusBar::RegisterClass(HINSTANCE hInstance) {
  Logger logger("IMEStatusBar");
  logger.debug("RegisterClass %08x", hInstance);

  static wstring className = OVUTF16::FromUTF8(BASEIME_STATUS_BAR_CLASS);
  c_sharedHInstance = hInstance;

  if (!c_classRegistered) {
    c_classRegistered = true;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_IME;
    wc.lpfnWndProc = (WNDPROC)IMEStatusBar::WindowProc;
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

bool IMEStatusBar::Create(HWND parent) {
  if (!c_sharedStatusBar) {
    if (!c_classRegistered) return false;

    c_sharedStatusBar = new IMEStatusBar;
  }

  wstring className = OVUTF16::FromUTF8(BASEIME_STATUS_BAR_CLASS);

  c_sharedStatusBar->m_window = CreateWindowEx(
      0, className.c_str(), NULL, WS_POPUP | WS_CLIPCHILDREN | WS_EX_TOPMOST, 0,
      0, 1, 1, parent, NULL, c_sharedHInstance, NULL);

  SetWindowLongPtr(c_sharedStatusBar->m_window, 0, (LONG_PTR)c_sharedStatusBar);
  ShowWindow(c_sharedStatusBar->m_window, SW_HIDE);
  c_sharedStatusBar->moveToRememberdLocation();

  return true;
}

void IMEStatusBar::Destroy() {
  if (c_sharedStatusBar) {
    c_sharedStatusBar->m_window = 0;
  }
}

IMEStatusBar* IMEStatusBar::SharedStatusBar() { return c_sharedStatusBar; }

void IMEStatusBar::show() {
  if (!m_window) return;
  ShowWindow(m_window, SW_SHOWNA);
  moveToRememberdLocation();
}

void IMEStatusBar::hide() {
  if (!m_window) return;
  ShowWindow(m_window, SW_HIDE);
}

void IMEStatusBar::moveToRememberdLocation() {
  if (!m_window) return;
  RECT windowRect;
  GetWindowRect(m_window, &windowRect);

  if (!lastPosX && !lastPosY) {
    lastPosX = 9999;
    lastPosY = 9999;
  }

  // get screen working area dimension
  RECT workingArea;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &workingArea, 0);
  // logger.debug("working area %d %d %d %d", workingArea.left, workingArea.top,
  // workingArea.right, workingArea.bottom);

  int w = windowRect.right - windowRect.left;
  int h = windowRect.bottom - windowRect.top;

  if (lastPosX + w > workingArea.right - 30) {
    lastPosX = workingArea.right - 30 - w;
  }

  if (lastPosY + h > workingArea.bottom - 30) {
    lastPosY = workingArea.bottom - 30 - h;
  }

  MoveWindow(m_window, lastPosX, lastPosY, w, h, TRUE);
}

void IMEStatusBar::moveTo(int x, int y) {
  if (!m_window) return;

  if (!lastPosSet) {
    lastPosX = x;
    lastPosY = y;
    lastPosSet = true;
  } else {
    moveToRememberdLocation();
    return;
  }

  moveToRememberdLocation();
}

void IMEStatusBar::updateContent(wchar_t* currentInputMethodName) {
  if (!m_window) return;

  m_currentInputMethodName = currentInputMethodName;

  if (ServerLocale() == L"en") {
    m_infoString = L"Compatibility Mode";
  } else {
    m_infoString =
        OVUTF16::FromUTF8("\xE7\x9B\xB8\xE5\xAE\xB9\xE6\xA8\xA1\xE5\xBC\x8F");
  }

  HDC hDC = GetDC(m_window);
  long fontHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  HFONT font = CreateFont(fontHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0,
                          0, 0, 0, L"Arial");
  long infoFontHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  HFONT infoFont = CreateFont(infoFontHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE,
                              0, 0, 0, 0, 0, L"Arial");

  SelectObject(hDC, font);

  SIZE size;
  GetTextExtentPoint32W(hDC, m_currentInputMethodName.c_str(),
                        m_currentInputMethodName.length(), &size);
  int inputMethodNameWidth = size.cx;

  SelectObject(hDC, infoFont);
  GetTextExtentPoint32W(hDC, m_infoString.c_str(), m_infoString.length(),
                        &size);
  int infoStringWidth = size.cx;

  DeleteObject(font);
  DeleteObject(infoFont);
  ReleaseDC(m_window, hDC);

  int h = 23;
  int w = 80 + inputMethodNameWidth + infoStringWidth;

  SetWindowPos(m_window, NULL, 0, 0, w, h,
               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
  InvalidateRect(m_window, NULL, FALSE);
  moveToRememberdLocation();
}

IMEStatusBar::IMEStatusBar() : m_window(0) {}

};  // namespace BaseIME
