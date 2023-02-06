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

#include "IMEInputBuffer.h"

#include <LogKit.h>
#include <OpenVanilla.h>

#include "IMEHelper.h"

#define BASEIME_INPUT_BUFFER_CLASS "IMEInputBuffer"

namespace BaseIME {

using namespace OpenVanilla;
using namespace LFPlatform;

HINSTANCE IMEInputBuffer::c_sharedHInstance = 0;
bool IMEInputBuffer::c_classRegistered = false;
IMEInputBuffer* IMEInputBuffer::c_sharedInputBuffer = 0;

LRESULT IMEInputBuffer::WindowProc(HWND hWnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
  // Logger logger("IMEInputBuffer");
  // logger.debug("WindowProc %d", msg);

  IMEInputBuffer* inputBuffer = (IMEInputBuffer*)GetWindowLongPtr(hWnd, 0);

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      RECT rect;
      GetClientRect(hWnd, &rect);
      // logger.debug("WM_PAINT %d,%d,%d,%d", rect.left, rect.top, rect.right,
      // rect.bottom);

      HDC hDC = ps.hdc;

      FillRect(hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

      long fontSize = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
      HFONT font = CreateFont(fontSize, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0,
                              0, 0, 0, L"MingLiU");
      HPEN whitePen = (HPEN)GetStockObject(WHITE_PEN);

      SelectObject(hDC, whitePen);
      SelectObject(hDC, font);

      SetBkMode(hDC, TRANSPARENT);
      SetBkColor(hDC, RGB(255, 255, 255));
      SetTextColor(hDC, RGB(0, 0, 0));
      DrawText(hDC, inputBuffer->m_combinedCompositionString.c_str(),
               wcslen(inputBuffer->m_combinedCompositionString.c_str()), &rect,
               DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

      SIZE size;
      RECT cursorRect;
      cursorRect.top = 0;
      cursorRect.bottom = rect.bottom - 2;

      wstring fullString(inputBuffer->m_combinedCompositionString.c_str());
      size_t index = inputBuffer->m_cursorIndex;
      wstring stringBeforeCursor;
      if (index >= fullString.length()) {
        stringBeforeCursor = fullString;
      } else {
        stringBeforeCursor = fullString.substr(0, index);
      }
      GetTextExtentPoint32W(hDC, stringBeforeCursor.c_str(),
                            stringBeforeCursor.length(), &size);
      cursorRect.left = size.cx;

      wstring cursorString = L"";
      if (inputBuffer->m_cursorIndex >= fullString.length()) {
        cursorRect.right = cursorRect.left + (12 * 96 / 72);
      } else {
        cursorString = fullString.substr(index, 1);
        GetTextExtentPoint32W(hDC, cursorString.c_str(), cursorString.length(),
                              &size);
        cursorRect.right = cursorRect.left + size.cx;
      }

      HBRUSH purpleBrush = CreateSolidBrush(RGB(140, 91, 156));
      FillRect(hDC, &cursorRect, purpleBrush);

      if (cursorString.length()) {
        SetTextColor(hDC, RGB(255, 255, 255));
        DrawText(hDC, cursorString.c_str(), cursorString.length(), &cursorRect,
                 DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
      }

      DeleteObject(font);
      DeleteObject(purpleBrush);

      // POINT lines[5];
      // lines[0].x = rect.left;
      // lines[0].y = rect.top;
      // lines[1].x = rect.right - 1;
      // lines[1].y = rect.top;
      // lines[2].x = rect.right - 1;
      // lines[2].y = rect.bottom - 1;
      // lines[3].x = rect.left;
      // lines[3].y = rect.bottom - 1;
      // lines[4].x = rect.left;
      // lines[4].y = rect.top;
      // HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(128,128,128));
      // SelectObject(hDC, borderPen);
      // Polyline(hDC, lines, 5);
      // SelectObject(hDC, GetStockObject(BLACK_PEN));
      // DeleteObject(borderPen);

      POINT lines[2];
      lines[0].x = rect.left;
      lines[0].y = rect.bottom - 1;
      lines[1].x = rect.right;
      lines[1].y = rect.bottom - 1;

      HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
      SelectObject(hDC, borderPen);
      Polyline(hDC, lines, 2);
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

bool IMEInputBuffer::RegisterClass(HINSTANCE hInstance) {
  Logger logger("IMEInputBuffer");
  logger.debug("RegisterClass %08x", hInstance);

  static wstring className = OVUTF16::FromUTF8(BASEIME_INPUT_BUFFER_CLASS);
  c_sharedHInstance = hInstance;

  if (!c_classRegistered) {
    c_classRegistered = true;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_IME;
    wc.lpfnWndProc = (WNDPROC)IMEInputBuffer::WindowProc;
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

bool IMEInputBuffer::Create(HWND parent) {
  // Logger logger("IMEInputBuffer");
  // logger.debug("Create %d", parent);

  if (!c_sharedInputBuffer) {
    if (!c_classRegistered) return false;

    c_sharedInputBuffer = new IMEInputBuffer;
  }

  wstring className = OVUTF16::FromUTF8(BASEIME_INPUT_BUFFER_CLASS);

  c_sharedInputBuffer->m_window = CreateWindowEx(
      0, className.c_str(), NULL,
      WS_POPUP | WS_CLIPCHILDREN | WS_EX_TOPMOST | WS_EX_NOACTIVATE, 0, 0, 450,
      150, parent, NULL, c_sharedHInstance, NULL);

  SetWindowLongPtr(c_sharedInputBuffer->m_window, 0,
                   (LONG_PTR)c_sharedInputBuffer);
  ShowWindow(c_sharedInputBuffer->m_window, SW_HIDE);
  return true;
}

void IMEInputBuffer::Destroy() {
  if (c_sharedInputBuffer) {
    c_sharedInputBuffer->m_window = 0;
  }
  //     if (c_sharedInputBuffer) {
  //         delete c_sharedInputBuffer;
  //         c_sharedInputBuffer = 0;
  //     }
}

IMEInputBuffer* IMEInputBuffer::SharedInputBuffer() {
  return c_sharedInputBuffer;
}

void IMEInputBuffer::show() {
  if (!m_window) return;
  ShowWindow(m_window, SW_SHOWNA);
}

void IMEInputBuffer::hide() {
  if (!m_window) return;
  ShowWindow(m_window, SW_HIDE);
}

void IMEInputBuffer::checkWindowPosition() {
  if (!m_window) return;

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
    MoveWindow(m_window, windowRect.left, windowRect.top,
               windowRect.right - oldX, windowRect.bottom - oldY, TRUE);
  }
}

void IMEInputBuffer::moveTo(int x, int y) {
  if (!m_window) return;

  RECT windowRect;
  GetWindowRect(m_window, &windowRect);
  MoveWindow(m_window, x, y, windowRect.right - windowRect.left,
             windowRect.bottom - windowRect.top, TRUE);

  checkWindowPosition();
}

void IMEInputBuffer::updateContent(wchar_t* combinedCompositionString,
                                   wchar_t* combinedCompositionAttributes,
                                   size_t cursorPosition) {
  if (!m_window) return;

  m_combinedCompositionString = combinedCompositionString;
  m_combinedCompositionAttributes = combinedCompositionAttributes;

  m_cursorIndex = cursorPosition;

  HDC hDC = GetDC(m_window);
  long fontHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  HFONT font = CreateFont(fontHeight, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0,
                          0, 0, 0, L"MingLiU");
  SelectObject(hDC, font);
  SIZE size;
  GetTextExtentPoint32W(hDC, m_combinedCompositionString.c_str(),
                        wcslen(m_combinedCompositionString.c_str()), &size);
  DeleteObject(font);
  ReleaseDC(m_window, hDC);

  int h = size.cy + 2;
  int w = size.cx;

  if (m_cursorIndex == wcslen(m_combinedCompositionString.c_str())) {
    w += (12 * 96 / 72);
  }

  SetWindowPos(m_window, NULL, 0, 0, w, h,
               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
  InvalidateRect(m_window, NULL, FALSE);

  checkWindowPosition();
}

IMEInputBuffer::IMEInputBuffer() : m_window(0) {}

};  // namespace BaseIME
