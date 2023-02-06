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

#include "IMEHelper.h"

#include <LFPlatform.h>

namespace BaseIME {

using namespace LFPlatform;

DWORD IMEHelper::PostMessageToClient(HIMC hIMC, UINT msg, WPARAM wParam,
                                     LPARAM lParam) {
  Logger logger("IMEHelper");
  logger.debug(
      "PostIMEMessageToClient, imc = %08x, msg = %08x, wP = %08x, lP = %08x; "
      "IMMLock count = %d",
      hIMC, msg, wParam, lParam, ImmGetIMCLockCount(hIMC));
  DWORD dwNumMsgBuf = 0;

  if (!hIMC) return dwNumMsgBuf;

  INPUTCONTEXT *lpIMC = ImmLockIMC(hIMC);
  if (!lpIMC) return dwNumMsgBuf;

  HIMCC hBuf = ImmReSizeIMCC(lpIMC->hMsgBuf,
                             (lpIMC->dwNumMsgBuf + 1) * sizeof(TRANSMSG));
  if (hBuf) {
    lpIMC->hMsgBuf = hBuf;
    TRANSMSG *pBuf = (TRANSMSG *)ImmLockIMCC(hBuf);
    if (pBuf) {
      pBuf[lpIMC->dwNumMsgBuf].message = msg;
      pBuf[lpIMC->dwNumMsgBuf].wParam = wParam;
      pBuf[lpIMC->dwNumMsgBuf].lParam = lParam;
      lpIMC->dwNumMsgBuf++;
      dwNumMsgBuf = lpIMC->dwNumMsgBuf;
      ImmUnlockIMCC(hBuf);
    }
  }
  ImmUnlockIMC(hIMC);

  if (dwNumMsgBuf != 0) ImmGenerateMessage(hIMC);

  return dwNumMsgBuf;
}

};  // namespace BaseIME
