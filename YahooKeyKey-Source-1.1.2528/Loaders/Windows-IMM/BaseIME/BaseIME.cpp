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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "BaseIME.h"

#include <cmath>

#include "IMECandidatePanel.h"
#include "IMEClient.h"
#include "IMEHelper.h"
#include "IMEInputBuffer.h"
#include "IMEStatusBar.h"

using namespace OpenVanilla;
using namespace LFPlatform;
using namespace BaseIME;

// Ime entry points implemented here
BOOL WINAPI ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode,
                         LPVOID pRegisterWord);
BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR lpszUIClass,
                       LPCTSTR lpszOptions);
BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT uVirKey, LPARAM lParam,
                          CONST BYTE* lpbKeyState);
BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect);
BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue);

// Our own callbacks and utils
LRESULT WINAPI BIUIWindowProc(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam);
int BIDetermineCandForm(LPARAM number);
void BIUpdateInputBufferPosition(HIMC hIMC,
                                 bool alsoUpdateCandidatePanel = false);
void BIUpdateCandidatePanelPosition(HIMC hIMC, int index);
void BIUpdateCandidateWindow(HIMC hIMC);
void BIForceCleanUp(HIMC hIMC, bool clearClientState = true);

void BICRequestSendPasteKey() {
  Logger logger("BICRequestSendPasteKey");
  logger.debug("Received server request, sending the paste key");

  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.wScan = 0;
  input.ki.time = 0;
  input.ki.dwExtraInfo = GetMessageExtraInfo();

  // KEYDOWN CTRL then 'V'
  input.ki.wVk = 0x11;
  input.ki.dwFlags = 0;
  SendInput(1, &input, sizeof(input));

  input.ki.wVk = 'V';
  input.ki.dwFlags = 0;
  SendInput(1, &input, sizeof(input));

  // KEY UP 'V' then CTRL
  input.ki.wVk = 'V';
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &input, sizeof(input));

  input.ki.wVk = 0x11;
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &input, sizeof(input));
}

// the RPC function
void BICRequestSendFakeKey() {
  Logger logger("BICRequestSendFakeKey");
  logger.debug("Received server request, sending the fake key");

  IMEClient::SharedClient()->setFakeKeyRequest(true);

  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = 'A';
  input.ki.wScan = 0;
  input.ki.dwFlags = 0;
  input.ki.time = 0;
  input.ki.dwExtraInfo = GetMessageExtraInfo();

  // KEYDOWN
  input.ki.dwFlags = 0;
  SendInput(1, &input, sizeof(input));

  // KEY UP
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &input, sizeof(input));
}

void BICSendComposedText(
    /* [size_is][string][in] */ const wchar_t* text) {
  Logger logger("Client RPC");
  logger.debug("Received direct text request: %s",
               OVUTF8::FromUTF16(text).c_str());
}

void BICNotifyServerShutdown(void) {
  Logger logger("Client RPC");
  logger.debug("Client is told that service is shut down");
}

BOOL WINAPI ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode,
                         LPVOID pRegisterWord) {
  Logger logger("ImeConfigure");
  logger.debug("Input locale = %x, window = %x, mode = %x, register word = %x",
               hkl, hWnd, dwMode, pRegisterWord);

  // Configure our stuff here
  IMEClient::SharedClient()->openPreferences();
  return TRUE;
}

BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR lpszUIClass,
                       LPCTSTR lpszOptions) {
  Logger logger("ImeInquire");
  logger.debug("Setting up data");
  // set up the IME information
  lpIMEInfo->dwPrivateDataSize = 0;
  lpIMEInfo->fdwSCSCaps = 0;
  lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;
  lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
  lpIMEInfo->fdwUICaps = UI_CAP_2700;

  // IME_CMODE_NOCONVERSION must be set, because some games require that
  // (otherwise their candidate windows will not be vertical)
  lpIMEInfo->fdwConversionCaps = /* IME_CMODE_FULLSHAPE
   | */ IME_CMODE_NATIVE | IME_CMODE_NOCONVERSION;

  lpIMEInfo->fdwProperty = IME_PROP_AT_CARET | IME_PROP_CANDLIST_START_FROM_1 |
                           IME_PROP_COMPLETE_ON_UNSELECT | IME_PROP_END_UNLOAD |
                           IME_PROP_KBD_CHAR_FIRST | IME_PROP_NEED_ALTKEY |
                           IME_PROP_UNICODE;

  wstring clsName = OVUTF16::FromUTF8(BASEIME_WINDOW_CLASS);
  _tcscpy_s(lpszUIClass, clsName.size() + 1, clsName.c_str());
  return TRUE;
}

BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT uVirKey, LPARAM lParam,
                          CONST BYTE* lpbKeyState) {
  IMEClient* client = IMEClient::SharedClient();
  KeyInfo keyInfo = KeyInfo::FromLPARAM(lParam);

  Logger logger("ImeProcessKey");
  logger.debug(
      "%s, hIMC = 0x%08x, uVirKey = 0x%04x (%d), lParam = 0x%08x, keyState = "
      "(%02x %02x)",
      ((keyInfo.isKeyUp) ? "KEY UP" : "KEY DOWN"), hIMC, uVirKey, uVirKey,
      lParam, (char)lpbKeyState[0], (char)lpbKeyState[1]);

  bool keyTranslated = false;
  unsigned int keyCode = 0;
  unsigned int keyModifiers = 0;
  static DWORD shiftPressedTime = 0;
  const DWORD SHIFT_PRESS_TIMEOUT = 300;

  if (client->hasFakeKeyRequest()) {
    logger.debug("Forging the fake key");
    keyCode = BASEIME_FAKEKEY_CODE;
    keyModifiers = BASEIME_FAKEKEY_MODIFIER;
    keyTranslated = true;
    client->setFakeKeyRequest(false);
  }

  // ignores CAPS LOCK key; we need it as a state, not a key itself
  if (uVirKey == VK_CAPITAL) return FALSE;

  if (uVirKey == VK_LWIN || uVirKey == VK_RWIN) return FALSE;

  // process SHIFT and SHIFT-SPACE
  if (keyInfo.isKeyUp) {
    logger.debug(
        "KEY IS UP, shiftPressedTime = %ld, tick count = %ld, diff = %ld",
        shiftPressedTime, GetTickCount(), (GetTickCount() - shiftPressedTime));

    if (shiftPressedTime > 0 && uVirKey == VK_SHIFT &&
        (GetTickCount() - shiftPressedTime) < SHIFT_PRESS_TIMEOUT) {
      // we make it a key
      keyCode = OVKeyCode::LeftShift;
      keyTranslated = true;
      shiftPressedTime = 0;
    } else {
      shiftPressedTime = 0;
      return FALSE;
    }
  }

  if (!keyInfo.isKeyUp && uVirKey == VK_SHIFT) {
    logger.debug("SHIFT keydown, setting shiftPressedTime");

    if (!KeyHelper::IsKeyStateDown(lpbKeyState[VK_CONTROL]) &&
        !shiftPressedTime) {
      shiftPressedTime = GetTickCount();
      logger.debug("SHIFT shiftPressedTime = %d", (int)shiftPressedTime);
    }
    return FALSE;
  } else {
    shiftPressedTime = 0;
  }

  // handles SHIFT-SPACE
  if (LOWORD(uVirKey) == VK_SPACE &&
      !KeyHelper::IsKeyStateDown(lpbKeyState[VK_CONTROL]) &&
      KeyHelper::IsKeyStateDown(lpbKeyState[VK_SHIFT])) {
    keyCode = OVKeyCode::Space;
    keyModifiers = OVKeyMask::Shift;
    keyTranslated = true;
  }

  // ignores CTRL-SPACE
  if (LOWORD(uVirKey) == VK_SPACE && (lpbKeyState[VK_CONTROL] & 0x80)) {
    logger.debug("CTRL-SPACE pressed, ImmGetOpenStatus: %d",
                 ImmGetOpenStatus(hIMC));
    return FALSE;
  }

  if (!keyTranslated) {
    // translate the key modifiers
    WORD translatedWords[3];
    int result = ToAscii(uVirKey, MapVirtualKey(uVirKey, 0), lpbKeyState,
                         (LPWORD)&translatedWords, 0);

    // if shift is pressed
    if (lpbKeyState[VK_CONTROL] & 0x80) keyModifiers |= OVKeyMask::Ctrl;
    if (lpbKeyState[VK_MENU] & 0x80) keyModifiers |= OVKeyMask::Alt;
    if (LOWORD(lpbKeyState[VK_CAPITAL])) keyModifiers |= OVKeyMask::CapsLock;

    if ((uVirKey >= VK_NUMPAD0) && (uVirKey <= VK_DIVIDE))
      keyModifiers |= OVKeyMask::NumLock;

    if (result == 1)
      keyCode = (unsigned int)(translatedWords[0] & 0x7f);
    else {
      keyCode = uVirKey;
      bool shiftOn = !!(lpbKeyState[VK_SHIFT] & 0x80);

      // @todo: translates some known virutal key codes
      // switch (keyCode) {
      // default:
      //  break;
      // }
    }

    if (lpbKeyState[VK_SHIFT] & 0x80 || LOWORD(uVirKey) == VK_SHIFT) {
      keyModifiers |= OVKeyMask::Shift;
    }

    // and translate some strange chars
    if (keyModifiers & OVKeyMask::Ctrl) {
      switch ((char)keyCode) {
        case 27:
          keyCode = '[';
          break;
        case 28:
          keyCode = '\\';
          break;
        case 29:
          keyCode = ']';
          break;
        case -34:
          keyCode = (keyModifiers & OVKeyMask::Shift) ? '\"' : '\'';
          break;
        case -65:
          keyCode = (keyModifiers & OVKeyMask::Shift) ? '?' : '/';
          break;
        case -66:
          keyCode = (keyModifiers & OVKeyMask::Shift) ? '>' : '.';
          break;
        case -68:
          keyCode = (keyModifiers & OVKeyMask::Shift) ? '<' : ',';
          break;
        case -70:
          keyCode = (keyModifiers & OVKeyMask::Shift) ? ':' : ';';
          break;
      }
    }

    logger.debug("result %d translated keycode = %d (%04x)", result, keyCode,
                 keyCode);

    if (keyCode && !(keyModifiers & OVKeyMask::Shift))
      keyCode = tolower((char)keyCode);
    else if (keyCode && (keyModifiers & OVKeyMask::Shift))
      keyCode = toupper((char)keyCode);

    if (keyModifiers & OVKeyMask::CapsLock) {
      if (keyCode && !(keyModifiers & OVKeyMask::Shift))
        keyCode = toupper((char)keyCode);
      else
        keyCode = tolower((char)keyCode);
    }

    if (LOWORD(uVirKey) >= VK_F1 && LOWORD(uVirKey) <= VK_F24) return FALSE;

    switch (LOWORD(uVirKey)) {
      case VK_PRIOR:
        keyCode = OVKeyCode::PageUp;
        break;
      case VK_NEXT:
        keyCode = OVKeyCode::PageDown;
        break;
      case VK_END:
        keyCode = OVKeyCode::End;
        break;
      case VK_HOME:
        keyCode = OVKeyCode::Home;
        break;
      case VK_LEFT:
        keyCode = OVKeyCode::Left;
        break;
      case VK_UP:
        keyCode = OVKeyCode::Up;
        break;
      case VK_RIGHT:
        keyCode = OVKeyCode::Right;
        break;
      case VK_DOWN:
        keyCode = OVKeyCode::Down;
        break;
      case VK_DELETE:
        keyCode = OVKeyCode::Delete;
        break;

      case VK_CONTROL:
      case VK_INSERT:
        keyCode = 0;
        return FALSE;
        break;
    }
  }

  logger.debug("translated keycode OV : 0x%08x, mask : 0x%08x", keyCode,
               keyModifiers);
  if (client->shouldSendWMCharForAsciiCommit()) {
    // currently, only Y! Instant Messenger has this flag, so the hack!
    // HACK: ALT-S (send the whole string) requires clearing the attribute
    // buffer
    if (keyCode == 18 && keyModifiers == OVKeyMask::Alt) {
      logger.debug("ALT-S hit");

      AutoIMC imc(hIMC);

      if (!imc()) {
        logger.debug("Fails to lock IMC");
        return FALSE;
      }

      AutoIMCC<LPCOMPOSITIONSTRING> compstr(imc()->hCompStr);

      IMECompositionString result;
      size_t aslen = wcslen(client->compositionString());
      for (size_t ai = 0; ai < aslen; ++ai) {
        result.setComposingAttributeByIndex(ai, ATTR_FIXEDCONVERTED);
      }
      result.setComposingAttributeLength(aslen);
      result.setComposingText(client->compositionString());
      result.overwrite(compstr());
      LPARAM compositionParam = GCS_COMPSTR | GCS_COMPATTR |
                                GCS_COMPCLAUSE  // composition str, attr, clause
                                | GCS_COMPREADSTR | GCS_COMPREADATTR |
                                GCS_COMPREADCLAUSE  // reading str, attr, clause
                                | GCS_CURSORPOS | GCS_DELTASTART;
      IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
                                     compositionParam);
      return FALSE;
    }
  }

  AutoIMC imc(hIMC);

  if (!imc()) {
    logger.debug("Fails to lock IMC");
    return FALSE;
  }

  AutoIMCC<LPCOMPOSITIONSTRING> compstr(imc()->hCompStr);
  LPARAM compositionParam = 0;

  if (!compstr()) {
    logger.debug("Fails to lock IMCC");
    return FALSE;
  }

  // determine if the composition string is already empty but we are not (status
  // cleared by client)
  IMECompositionString clientIMCC(compstr());
  logger.debug("reading client state: %d vs. %d",
               wcslen(clientIMCC.composingText),
               wcslen(client->combinedCompositionString()));
  if (!wcslen(clientIMCC.composingText) &&
      wcslen(client->combinedCompositionString())) {
    logger.debug("Context state reset by the client application");
    client->deactivate();
    client->clear();
    client->activate();
  }

  bool handled;
  handled = client->handleKey(keyCode, keyModifiers);
  client->dump();

  if (!handled) {
    logger.debug(
        "key not handled, composition started: %s, combined composition "
        "string: %s",
        (client->isWMIMECompositionStarted() ? "true" : "false"),
        OVUTF8::FromUTF16(client->combinedCompositionString()).c_str());

    // bypass the single SHIFT key, to fix the associated phrase + empty space +
    // other key regression we're doing so because we can't flush the client's
    // candidate state, lest the candidate window will stick while the server
    // side's context's candidate is already changed (i.e. state discrepencies)
    if (keyCode == OVKeyCode::LeftShift) {
      // not possible to have key mask
      return FALSE;
    }

    // end the composition state
    if (client->isWMIMECompositionStarted() &&
        !wcslen(client->combinedCompositionString())) {
      // flush the composition buffer
      // IMEHelper::PostMessageToClient(hIMC, WM_IME_ENDCOMPOSITION, 0, 0);
      // client->endWMIMEComposition();

      // we can't clear client state, otherwise BIUpdateCandidateWindow won't
      // work (bug fix for associated phrase window)
      BIForceCleanUp(hIMC, false);
    }

    BIUpdateCandidateWindow(hIMC);
    client->clear();
    return FALSE;
  }

  // app-specific fixes, especially targeting Yahoo Messenger
  wchar_t wch = *(client->committedString());
  if (wcslen(client->committedString()) == 1 &&
      ((wch >= 'A' && wch <= 'Z') || (wch >= 'a' && wch <= 'z')) &&
      !wcslen(client->combinedCompositionString())) {
    if (client->shouldSendWMCharForAsciiCommit()) {
      // 0x00ff0001 = masks only key scan code, and set repeat count to 1
      // this doesn't work in MS Office, so needs more investigation, but
      // anyway now works with Yahoo Messenger, so we makes it an
      // app-specific fix
      if (client->isWMIMECompositionStarted()) {
        BIForceCleanUp(hIMC);
      }

      IMEHelper::PostMessageToClient(hIMC, WM_CHAR, wch, lParam & 0x00ff0001);
      return true;
    }
  }

  if (wcslen(client->committedString())) {
    IMECompositionString result;

    if (!client->isWMIMECompositionStarted()) {
      IMEHelper::PostMessageToClient(hIMC, WM_IME_STARTCOMPOSITION, 0, 0);
      client->startWMIMEComposition();
    } else {
      // there might be some program that need this, if not, forget about this
      // part (MSN Live Messenger?)

      // result.overwrite(compstr());
      // IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
      // compositionParam); IMEHelper::PostMessageToClient(hIMC,
      // WM_IME_ENDCOMPOSITION, 0, 0); IMEHelper::PostMessageToClient(hIMC,
      // WM_IME_STARTCOMPOSITION, 0, 0); client->startWMIMEComposition();
    }

    result.setCommittedText(client->committedString());
    result.overwrite(compstr());

    compositionParam = GCS_COMPSTR | GCS_COMPATTR |
                       GCS_COMPCLAUSE  // composition str, attr, clause
                       | GCS_COMPREADSTR | GCS_COMPREADATTR |
                       GCS_COMPREADCLAUSE  // reading str, attr, clause
                       | GCS_CURSORPOS | GCS_DELTASTART | GCS_RESULTSTR;

    // compositionParam = GCS_RESULTSTR;
    IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
                                   compositionParam);

    // end composition if we don't have other things following
    if (!wcslen(client->combinedCompositionString())) {
      // no one loves duplicated code, but some games require us to update
      // candidate info HERE, otherwise associated phrases won't show up!
      AutoIMCC<LPCANDIDATEINFO> candinfo(imc()->hCandInfo);
      IMECandidateInfo filler;
      filler.setCandidatesFromContinuousString(client->candidatePageString());
      filler.setHighlightedIndex(client->highlightedCandidateIndex());
      filler.overwrite(candinfo());
      BIUpdateCandidateWindow(hIMC);

      // send WM_IME_ENDCOMPOSITION if no candidate panel is visible--no
      // associated phrase NOTE 2008-09-10: if we don't send W_I_E, MSN
      // auto-emoticon and probably Excel become buggy
      if (!client->isCandidatePanelVisible() /* && client->isCandidatePanelPreviouslyVisible() */) {
        IMEHelper::PostMessageToClient(hIMC, WM_IME_ENDCOMPOSITION, 0, 0);
        client->endWMIMEComposition();
      }
      return handled;
    }
  }

  if (wcslen(client->combinedCompositionString())) {
    IMECompositionString composition;

    // translate the attribute
    wchar_t* attrstr = client->combinedCompositionStringAttribute();
    size_t aslen = wcslen(attrstr);
    for (size_t ai = 0; ai < aslen; ++ai) {
      switch (attrstr[ai]) {
        case 'r':
          composition.setComposingAttributeByIndex(ai,
                                                   ATTR_TARGET_NOTCONVERTED);
          break;
        case 'h':
          composition.setComposingAttributeByIndex(ai, ATTR_TARGET_CONVERTED);
          break;
        case 'n':
        default:
          composition.setComposingAttributeByIndex(ai, ATTR_INPUT);
      }
    }
    composition.setComposingAttributeLength(aslen);
    composition.setComposingText(client->combinedCompositionString());
    composition.setCursorIndex(client->cursorIndex());
    composition.overwrite(compstr());

    compositionParam = GCS_COMPSTR | GCS_COMPATTR |
                       GCS_COMPCLAUSE  // composition str, attr, clause
                       | GCS_COMPREADSTR | GCS_COMPREADATTR |
                       GCS_COMPREADCLAUSE  // reading str, attr, clause
                       | GCS_CURSORPOS | GCS_DELTASTART;

    // we update the IME client (the desktop app)'s composition state only if it
    // is changed addition: we need to do this too if we've got
    // committedString(), because there's this border-line case where you type
    // e.g. ",,,,,,,,,,,,," and this part will never be entered!
    if (client->compositionStateDifferntFromPreviousOne() ||
        wcslen(client->committedString())) {
      if (!client->isWMIMECompositionStarted()) {
        IMEHelper::PostMessageToClient(hIMC, WM_IME_STARTCOMPOSITION, 0, 0);
        client->startWMIMEComposition();
      }

      IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
                                     compositionParam);
    }
  } else {
    // of if (wcslen(client->combinedCompositionString())) ==> combinedCS is
    // empty

    // because commit-string section already has updated the candidate window,
    // we don't do this again if (!wcslen(client->committedString()))
    //     BIUpdateCandidateWindow(hIMC);

    if (client->isWMIMECompositionStarted()) {
      if (client->isGoogleChrome()) {
        BIForceCleanUp(hIMC);
      } else {
        // flush the composition buffer
        IMECompositionString composition;
        composition.overwrite(compstr());

        // we commented out this line to ensure compatibility of some games,
        // however, we the real behavior should be that, if the previous
        // composing string is not empty, and now is, then we should tell the
        // client app that it's now cleared
        if (!client->isCombinedCompositionStringPreviouslyEmpty())
          IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
                                         compositionParam);
      }
    } else {
      IMEHelper::PostMessageToClient(hIMC, WM_IME_STARTCOMPOSITION, 0, 0);
      client->startWMIMEComposition();
    }
  }
  AutoIMCC<LPCANDIDATEINFO> candinfo(imc()->hCandInfo);
  IMECandidateInfo filler;
  filler.setCandidatesFromContinuousString(client->candidatePageString());
  filler.setHighlightedIndex(client->highlightedCandidateIndex());
  filler.overwrite(candinfo());

  BIUpdateCandidateWindow(hIMC);
  return handled;
}

BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect) {
  Logger logger("ImeSelect");
  logger.debug("hIMC = 0x%08x, select = %s", hIMC,
               (fSelect ? "TRUE" : "FALSE"));

  if (!hIMC) return FALSE;

  AutoIMC imc(hIMC);
  if (!imc()) return TRUE;

  if (fSelect) {
    // Init the general member of IMC.
    if (!(imc()->fdwInit & INIT_LOGFONT)) {
      // imc()->lfFont.A.lfCharSet = CHINESEBIG5_CHARSET;
      // imc()->lfFont.W.lfCharSet = CHINESEBIG5_CHARSET;
      imc()->fdwInit |= INIT_LOGFONT;
    }

    if (!(imc()->fdwInit & INIT_CONVERSION)) {
      imc()->fdwConversion = /* IME_CMODE_FULLSHAPE |*/ IME_CMODE_NATIVE;
      imc()->fdwInit |= INIT_CONVERSION;
    }

    imc()->fdwInit |= INIT_COMPFORM;

    // resize the stuff
    IMECompositionString stubCS;
    imc()->hCompStr = ImmReSizeIMCC(imc()->hCompStr, stubCS.size());

    AutoIMCC<LPCOMPOSITIONSTRING> compstr(imc()->hCompStr);
    if (compstr()) stubCS.overwrite(compstr());

    IMECandidateInfo stubCI;
    imc()->hCandInfo = ImmReSizeIMCC(imc()->hCandInfo, stubCI.size());
    AutoIMCC<LPCANDIDATEINFO> candinfo(imc()->hCandInfo);
    if (candinfo()) stubCI.overwrite(candinfo());
  }

  imc()->fOpen = fSelect;

  return TRUE;
}

LRESULT WINAPI BIUIWindowProc(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam) {
  static int candidateFormIndex = 0;

  Logger logger("BIUIWindowProc");
  logger.nop("hWnd = 0x%08x, wParam = 0x%08x, lParam = 0x%08x", hWnd, wParam,
             lParam);

  if (!hWnd) {
    switch (msg) {
      case WM_IME_SETCONTEXT:
      case WM_IME_CONTROL:
      case WM_IME_COMPOSITION:
      case WM_IME_COMPOSITIONFULL:
      case WM_IME_ENDCOMPOSITION:
      case WM_IME_SELECT:
      case WM_IME_STARTCOMPOSITION:
      case WM_IME_NOTIFY:
      case WM_IME_KEYDOWN:
      case WM_IME_KEYUP:
      case WM_IME_CHAR:
        logger.debug("Empty hWnd, return 0 for WM_IME_.+");
        return 0;
      case WM_PAINT:
        logger.debug("Empty hWnd, WM_PAINT");
        return DefWindowProc(hWnd, msg, wParam, lParam);
      case WM_TIMER:
        logger.debug("Empty hWnd, WM_TIMER");
        return DefWindowProc(hWnd, msg, wParam, lParam);
      default:
        logger.debug("Empty hWnd, use DefWindowProc.");
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
  }

  HIMC hIMC = IMEHelper::IMCFromWindow(hWnd);
  IMEClient* client = IMEClient::SharedClient();

  switch (msg) {
    case WM_CREATE:
      logger.debug("WM_CREATE");
      break;
    case WM_DESTROY:
      logger.debug("WM_DESTROY");
      IMEClient::SharedClient()->hideStatusBar();
      IMEClient::SharedClient()->hideInputBuffer();
      IMEClient::SharedClient()->hideCandidatePanel();
      IMEClient::SharedClient()->hideToolTip();
      IMEClient::SharedClient()->deactivate();

      if (IMEClient::SharedClient()->useWin32UI()) {
        IMECandidatePanel::Destroy();
        IMEInputBuffer::Destroy();
        IMEStatusBar::Destroy();
      }

      break;
    case WM_ACTIVATEAPP:
      logger.debug("WM_ACTIVATEAPP, wParam = %s", (wParam ? "TRUE" : "FALSE"));

      // Workaround for PowerPoint
      if (!wParam) {
        // hide UI components here
        IMEClient::SharedClient()->hideStatusBar();

        if (IMEClient::SharedClient()->useWin32UI()) {
          IMEStatusBar::Destroy();
        }
      }
      break;

    case WM_IME_SETCONTEXT: {
      string wislp;

      if ((BOOL)wParam) {
        // Excel doesn't go thru this
        if (!IMEClient::SharedClient()->ignoreWMIMESetContextFalse())
          IMEClient::SharedClient()->activate();

        IMEClient::SharedClient()->setIMEAwareMode(true);

        if (lParam & ISC_SHOWUIGUIDELINE) {
          wislp += "ISC_SHOWUIGUIDELINE";
          if (IMEClient::SharedClient()->useWin32UI()) {
            IMEStatusBar::Create(hWnd);
          }

          IMEClient::SharedClient()->showStatusBar();

          // we need to tell the client to let us open the status window again
          IMEHelper::PostMessageToClient(hIMC, WM_IME_NOTIFY,
                                         IMN_OPENSTATUSWINDOW, 0x1);
        }
        if (lParam & ISC_SHOWUICOMPOSITIONWINDOW) {
          wislp += " | ISC_SHOWUICOMPOSITIONWINDOW";
        }
        if (lParam & ISC_SHOWUICANDIDATEWINDOW) {
          wislp += " | ISC_SHOWUICANDIDATEWINDOW";
        }

        //@warning The following ISC_* are considered as redundants.
        if (lParam & ISC_SHOWUIALLCANDIDATEWINDOW) {
          wislp += " | ISC_SHOWUIALLCANDIDATEWINDOW";
        }
        if (lParam & ISC_SHOWUIALL) {
          wislp = " | ISC_SHOWUIALL";
        }
      } else {
        bool hideInputBufferAndToolTipAndCleanUp = false;
        bool hideCandidatePanel = false;

        if (lParam & ISC_SHOWUIGUIDELINE) {
          wislp += "ISC_SHOWUIGUIDELINE";
        }

        if (lParam & ISC_SHOWUICOMPOSITIONWINDOW) {
          wislp += " | ISC_SHOWUICOMPOSITIONWINDOW";
          hideInputBufferAndToolTipAndCleanUp = true;
        }

        if (lParam & ISC_SHOWUICANDIDATEWINDOW) {
          wislp += " | ISC_SHOWUICANDIDATEWINDOW";
          hideCandidatePanel = true;
        }

        if (lParam & ISC_SHOWUIALL) {
          wislp += " | ISC_SHOWUIALL";
          hideCandidatePanel = true;
        }

        if (hideCandidatePanel) IMEClient::SharedClient()->hideCandidatePanel();

        if (hideInputBufferAndToolTipAndCleanUp &&
            !IMEClient::SharedClient()->ignoreWMIMESetContextFalse()) {
          IMEClient::SharedClient()->hideInputBuffer();
          IMEClient::SharedClient()->hideToolTip();
          IMEClient::SharedClient()->deactivate();
          BIForceCleanUp(hIMC);
        }
      }

      logger.debug("WM_IME_SETCONTEXT, wParam = %s, lParam = %s",
                   (wParam ? "TRUE" : "FALSE"), wislp.c_str());
      break;
    }
    case WM_IME_CONTROL:
      switch (wParam) {
        case IMC_GETCANDIDATEPOS: {
          logger.debug("WM_IME_CONTROL: wParam = IMC_GETCANDIDATEPOS");
          LPINPUTCONTEXT lpIMC = ImmLockIMC(hIMC);
          *(LPCANDIDATEFORM)lParam = lpIMC->cfCandForm[0];
          ImmUnlockIMC(hIMC);
          return 0;
        }
        case IMC_GETCOMPOSITIONWINDOW: {
          logger.debug("WM_IME_CONTROL: wParam = IMC_GETCOMPOSITIONWINDOW");
          LPINPUTCONTEXT lpIMC = ImmLockIMC(hIMC);
          *(LPCOMPOSITIONFORM)lParam = lpIMC->cfCompForm;
          ImmUnlockIMC(hIMC);
          return 0;
        }
        default:
          logger.debug("WM_IME_CONTROL: other wParam, returns 1");
          return 1;
      }
    case WM_IME_STARTCOMPOSITION:
      logger.debug("WM_IME_STARTCOMPOSITION");
      break;
    case WM_IME_COMPOSITION:
      logger.debug(
          "WM_IME_COMPOSITION: shows composition window (input buffer)");

      IMEClient::SharedClient()->setIMEAwareMode(false);

      if (client->isWMIMECompositionStarted()) {
        logger.debug("composition started, now show the window");
        BIUpdateInputBufferPosition(hIMC);
        if (wcslen(client->combinedCompositionString())) {
          logger.debug("show input buffer");
          client->showInputBuffer();
        }
      } else {
        logger.debug("not started??");
      }
      break;

      break;
    case WM_IME_COMPOSITIONFULL:
      logger.debug("WM_IME_COMPOSITIONFULL");
      break;
    case WM_IME_ENDCOMPOSITION:
      logger.debug("WM_IME_ENDCOMPOSITION (now we hide input buffer)");
      client->hideInputBuffer();
      break;
    case WM_IME_SELECT:
      logger.debug(
          "WM_IME_SELECT, wParam = %s, lParam = 0x%08x (keyboard layout "
          "handle).",
          (wParam ? "TRUE" : "FALSE"), lParam);

      if ((BOOL)wParam) {
        IMEClient::SharedClient()->activate();
        IMEClient::SharedClient()->setIMEAwareMode(true);
        // BIForceCleanUp(hIMC);
      } else {
        IMEClient::SharedClient()->deactivate();
        BIForceCleanUp(hIMC);
        IMEClient::SharedClient()->hideInputBuffer();
        IMEClient::SharedClient()->hideCandidatePanel();
        IMEClient::SharedClient()->hideToolTip();
        IMEClient::SharedClient()->hideStatusBar();

        if (IMEClient::SharedClient()->useWin32UI()) {
          IMEStatusBar::Destroy();
        }
      }

      {
        AutoIMC imc(hIMC);
        if (imc()) {
          imc()->fOpen = !!wParam;
        }
      }

      break;
    case WM_IME_NOTIFY:
      switch (wParam) {
        case IMN_OPENSTATUSWINDOW:
          logger.debug("WM_IME_NOTIFY: IMN_OPENSTATUSWINDOW: %d",
                       ImmGetOpenStatus(hIMC));
          if (IMEClient::SharedClient()->useWin32UI()) {
            IMEInputBuffer::Create(hWnd);
            IMEStatusBar::Create(hWnd);
          }
          IMEClient::SharedClient()->showStatusBar();
          break;
        case IMN_CLOSESTATUSWINDOW:
          logger.debug("WM_IME_NOTIFY: IMN_CLOSESTATUSWINDOW: %d",
                       ImmGetOpenStatus(hIMC));
          IMEClient::SharedClient()->hideInputBuffer();
          IMEClient::SharedClient()->hideCandidatePanel();
          IMEClient::SharedClient()->hideToolTip();
          IMEClient::SharedClient()->hideStatusBar();

          if (IMEClient::SharedClient()->useWin32UI()) {
            IMECandidatePanel::Destroy();
            IMEInputBuffer::Destroy();
            IMEStatusBar::Destroy();
          }

          break;
        case IMN_OPENCANDIDATE:
          if (IMEClient::SharedClient()->useWin32UI()) {
            IMECandidatePanel::Create(hWnd);
          }

          logger.debug("WM_IME_NOTIFY: IMN_OPENCANDIDATE");
          break;
        case IMN_CHANGECANDIDATE:
          logger.debug("WM_IME_NOTIFY: IMN_CHANGECANDIDATE");
          BIUpdateCandidatePanelPosition(hIMC, candidateFormIndex);
          client->showCandidatePanel();
          break;
        case IMN_CLOSECANDIDATE:
          logger.debug("WM_IME_NOTIFY: IMN_CLOSECANDIDATE");
          client->hideCandidatePanel();

          if (IMEClient::SharedClient()->useWin32UI()) {
            IMECandidatePanel::Destroy();
          }
          break;
        case IMN_SETCANDIDATEPOS:
          logger.debug("WM_IME_NOTIFY: IMN_SETCANDIDATEPOS, lPararm=0x%08x",
                       lParam);
          candidateFormIndex = BIDetermineCandForm(lParam);
          break;
        case IMN_SETCONVERSIONMODE:
          logger.debug("WM_IME_NOTIFY: IMN_SETCONVERSIONMODE");
          break;
        case IMN_SETSENTENCEMODE:
          logger.debug("WM_IME_NOTIFY: IMN_SETSENTENCEMODE");
          break;
        case IMN_SETOPENSTATUS:
          logger.debug("WM_IME_NOTIFY: IMN_SETOPENSTATUS: %d",
                       ImmGetOpenStatus(hIMC));
          break;

        case IMN_SETCOMPOSITIONFONT:
          logger.debug("WM_IME_NOTIFY: IMN_SETCOMPOSITIONFONT");
          BIUpdateInputBufferPosition(hIMC);
          break;
        case IMN_SETCOMPOSITIONWINDOW:
          logger.debug("WM_IME_NOTIFY: IMN_SETCOMPOSITIONWINDOW");
          IMEClient::SharedClient()->setIMEAwareMode(false);
          BIUpdateInputBufferPosition(hIMC);
          break;
        case IMN_PRIVATE:
          logger.debug("WM_IME_NOTIFY: IMN_PRIVATE, lParam = 0x%08x", lParam);
          break;
        case IMN_GUIDELINE:
          logger.debug("IMN_GUIDELINE");
          break;
        case IMN_SOFTKBDDESTROYED:
          logger.debug("IMN_SOFTKBDDESTROYED");
          break;
      }
      break;
    case WM_IME_KEYDOWN:
      logger.debug("WM_IME_KEYDOWN");
      break;
    case WM_IME_KEYUP:
      logger.debug("WM_IME_KEYUP");
      break;
    case WM_IME_CHAR:
      logger.debug("WM_IME_CHAR");
      break;
    default:
      logger.nop("Unknown msg (%08x), returns DefWindowProc", msg);
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return 0;
}

BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue) {
  LFPlatform::Logger logger("NotifyIME");
  logger.nop("IMC = %08x, action = %08x, index = %08x, value = %08x", hIMC,
             dwAction, dwIndex, dwValue);

  IMEClient* client = IMEClient::SharedClient();

  switch (dwAction) {
    case NI_OPENCANDIDATE:
      logger.nop("NI_OPENCANDIDATE");
      return TRUE;
    case NI_CLOSECANDIDATE:
      logger.nop("NI_CLOSECANDIDATE");
      return TRUE;
    case NI_SELECTCANDIDATESTR:
      logger.nop("NI_SELECTCANDIDATESTR");
      return TRUE;
    case NI_CHANGECANDIDATELIST:
      logger.nop("NI_CHANGECANDIDATELIST");
      return TRUE;
    case NI_SETCANDIDATE_PAGESTART:
      logger.nop("NI_SETCANDIDATE_PAGESTART");
      return TRUE;
    case NI_SETCANDIDATE_PAGESIZE:
      logger.nop("NI_SETCANDIDATE_PAGESIZE");
      return TRUE;
    case NI_CONTEXTUPDATED:
      switch (dwValue) {
        case IMC_SETCOMPOSITIONWINDOW:
          logger.nop("NI_CONTEXTUPDATED: IMC_SETCOMPOSITIONWINDOW");
          return TRUE;
        case IMC_SETCONVERSIONMODE:
          logger.nop(
              "NI_CONTEXTUPDATED: IMC_SETCONVERSIONMODE, previous mode=%08x",
              dwIndex);
          return TRUE;
        case IMC_SETSENTENCEMODE:
          logger.nop(
              "NI_CONTEXTUPDATED: IMC_SETSENTENCEMODE, previous mode=%08x",
              dwIndex);
          return TRUE;
        case IMC_SETCANDIDATEPOS:
          // Firefox usually sends this when composing
          logger.nop("NI_CONTEXTUPDATED: IMC_SETCANDIDATEPOS");
          return TRUE;
        case IMC_SETCOMPOSITIONFONT:
          logger.nop("NI_CONTEXTUPDATED: IMC_SETCOMPOSITIONFONT");
          return TRUE;
        case IMC_SETOPENSTATUS:
          logger.debug("NI_CONTEXTUPDATED: IMC_SETOPENSTATUS %d %d", dwIndex,
                       dwValue);
          return TRUE;
      }
      break;
    case NI_COMPOSITIONSTR:
      switch (dwIndex) {
        case CPS_COMPLETE:
          logger.debug(
              "NI_COMPOSITIONSTR: CPS_COMPLETE (setting as result string)");

          if (client->ignoreCPSCompleteCleanUp()) {
            // Skype instant message

            IMEClient::SharedClient()->hideInputBuffer();
            IMEClient::SharedClient()->hideCandidatePanel();
            IMEClient::SharedClient()->hideToolTip();
          } else {
            client->deactivate();

            if (!client->ignoreCPSComplete()) {
              // mouse event triggers, also MS Word relies on this for
              // maximizing windows
              BIForceCleanUp(hIMC);
              IMEClient::SharedClient()->hideInputBuffer();
              IMEClient::SharedClient()->hideCandidatePanel();
              IMEClient::SharedClient()->hideToolTip();
            } else if (client->isPowerPoint()) {
              BIForceCleanUp(hIMC);
            }

            client->activate();
          }

          logger.debug("Done force clear");
          return TRUE;

        case CPS_CONVERT:
          logger.debug("NI_COMPOSITIONSTR:CPS_CONVERT (composing)");
          return TRUE;
        case CPS_REVERT:
          logger.debug("NI_COMPOSITIONSTR:CPS_REVERT (composing)");
          return TRUE;
        case CPS_CANCEL:
          logger.debug("NI_COMPOSITIONSTR:CPS_CANCEL (clearing)");
          return TRUE;
      }
  }

  return TRUE;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
  string appDataDir = OVDirectoryHelper::UserApplicationSupportDataDirectory(
      BASEIME_USER_APPLICATION_DATA_DIR);
  OVDirectoryHelper::CheckDirectory(appDataDir);

  string logFilename = OVPathHelper::PathCat(appDataDir, BASEIME_LOG_FILENAME);
  Logger::UseLogFile(logFilename);

  // make it static, because the window class's lpszClassName points to here
  static wstring className = OVUTF16::FromUTF8(BASEIME_WINDOW_CLASS);

  Logger logger("DllMain");

  switch (reason) {
    case DLL_PROCESS_ATTACH:
      logger.debug("DLL_PROCESS_ATTACH, module: %08x", module);

      // disable subsequent call of DLL_THREAD_ATTACH and DLL_THREAD_DETACH
      DisableThreadLibraryCalls((HMODULE)module);

      // starts the thing
      IMEClient::Start();

      // register the UI Window's class, note that cbWndExtra must be at least 8
      // bytes long
      WNDCLASSEX cls;
      cls.cbClsExtra = 0;
      cls.cbSize = sizeof(WNDCLASSEX);
      cls.cbWndExtra = sizeof(LONG) * 2;
      cls.hCursor = LoadCursor(NULL, IDC_ARROW);
      cls.hIcon = NULL;
      cls.hIconSm = NULL;
      cls.hInstance = (HINSTANCE)module;
      cls.hbrBackground = NULL;
      cls.lpfnWndProc = (WNDPROC)BIUIWindowProc;
      cls.lpszClassName = className.c_str();
      cls.lpszMenuName = (LPTSTR)NULL;
      cls.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_IME;

      if (!RegisterClassEx((LPWNDCLASSEX)&cls)) {
        logger.error("Cannot register window class: %s", BASEIME_WINDOW_CLASS);
        return FALSE;
      }

      if (!IMECandidatePanel::RegisterClass((HMODULE)module)) {
        logger.error("Cannot register candidate panel class");
        return FALSE;
      }

      if (!IMEInputBuffer::RegisterClass((HMODULE)module)) {
        logger.error("Cannot register input buffer class");
        return FALSE;
      }

      if (!IMEStatusBar::RegisterClass((HMODULE)module)) {
        logger.error("Cannot register staus bar class");
        return FALSE;
      }

      break;

    case DLL_PROCESS_DETACH:
      logger.debug("DLL_PROCESS_DETACH");

      // deactivate and hide all stuff
      IMEClient::SharedClient()->deactivate();
      IMEClient::SharedClient()->hideInputBuffer();
      IMEClient::SharedClient()->hideCandidatePanel();
      IMEClient::SharedClient()->hideStatusBar();

      if (IMEClient::SharedClient()->useWin32UI()) {
        IMEStatusBar::Destroy();
      }

      // unregister UI classes
      UnregisterClass(OVUTF16::FromUTF8(BASEIME_WINDOW_CLASS).c_str(),
                      (HINSTANCE)module);

      // stops
      IMEClient::Stop();

      break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;

    default:
      logger.error("DllMain in a strange point. What's the reason? %d (0x%08x)",
                   reason, reason);
  }
  return TRUE;
}

void BIUpdateCandidateWindow(HIMC hIMC) {
  bool candidateMsgSent = false;

  Logger logger("UpdateCandidateWindow");
  IMEClient* client = IMEClient::SharedClient();
  if (client->isCandidatePanelVisible() &&
      !client->isCandidatePanelPreviouslyVisible()) {
    logger.debug("sending WM_IME_NOTIFY: IMN_OPENCANDIDATE");
    IMEHelper::PostMessageToClient(hIMC, WM_IME_NOTIFY, IMN_OPENCANDIDATE,
                                   0x01);
    candidateMsgSent = true;
    BIUpdateCandidatePanelPosition(hIMC, 0);
  } else if (!client->isCandidatePanelVisible() &&
             client->isCandidatePanelPreviouslyVisible()) {
    logger.debug("sending WM_IME_NOTIFY: IMN_CLOSECANDIDATE");

    // never hurts to do it ourselves, fixes the bug for associated phrases
    client->hideCandidatePanel();

    IMEHelper::PostMessageToClient(hIMC, WM_IME_NOTIFY, IMN_CLOSECANDIDATE,
                                   0x01);

    // we still ask the app for the latest position to let our fake input buffer
    // show in correct pos at IME-aware mode candidateMsgSent = true;
  }

  if (client->isCandidatePanelVisible() &&
      client->shouldUpdateCandidatePanel()) {
    logger.debug("sending WM_IME_NOTIFY: IMN_CHANGECANDIDATE");
    IMEHelper::PostMessageToClient(hIMC, WM_IME_NOTIFY, IMN_CHANGECANDIDATE,
                                   0x01);
    candidateMsgSent = true;
  }

  if (!candidateMsgSent && IMEClient::SharedClient()->isIMEAwareMode())
    BIUpdateCandidatePanelPosition(hIMC, 0);
}

void BIUpdateInputBufferPosition(HIMC hIMC, bool alsoUpdateCandidatePanel) {
  IMEClient* client = IMEClient::SharedClient();

  Logger logger("BIUpdateInputBufferPosition");
  AutoIMC imc(hIMC);
  if (!imc()) return;

  POINT caretPoint;
  if (GetCaretPos(&caretPoint)) {
    logger.debug("Caret pos info: %d, %d", caretPoint.x, caretPoint.y);
  } else {
    logger.debug("No caret pos info avail.");
  }

  RECT windowRect;
  GetWindowRect(imc()->hWnd, &windowRect);
  logger.debug("Window rectangle: (%d, %d, %d, %d)", windowRect.left,
               windowRect.top, windowRect.right, windowRect.bottom);

  LOGFONT* lfFont = (LOGFONT*)&imc()->lfFont;

  logger.debug("lfHeight: %i", lfFont->lfHeight);
  client->setInputBufferFontSize(lfFont->lfHeight);

  logger.debug("cfCompForm.ptCurrent: (%d, %d)",
               imc()->cfCompForm.ptCurrentPos.x,
               imc()->cfCompForm.ptCurrentPos.y);
  logger.debug("cfCompForm.rcArea=(%d, %d, %d, %d)",
               imc()->cfCompForm.rcArea.left, imc()->cfCompForm.rcArea.top,
               imc()->cfCompForm.rcArea.right, imc()->cfCompForm.rcArea.bottom);

  POINT pos;
  pos.x = pos.y = 0;

  switch (imc()->cfCompForm.dwStyle) {
    case CFS_DEFAULT:
      logger.debug("imc()::cfCompForm.dwStyle = CFS_DEFAULT");
      break;
    case CFS_POINT:
      logger.debug("imc()::cfCompForm.dwStyle = CFS_POINT");
      pos = imc()->cfCompForm.ptCurrentPos;
      break;
    case CFS_FORCE_POSITION:
      logger.debug("imc()::cfCompForm.dwStyle = CFS_FORCE_POSITION");
      pos = imc()->cfCompForm.ptCurrentPos;
      break;
    case CFS_RECT:
      logger.debug("imc()::cfCompForm.dwStyle = CFS_RECT");
      pos = imc()->cfCompForm.ptCurrentPos;
      break;
    default:
      logger.debug("imc()::cfCompForm.dwStyle unown (0x%08x)",
                   imc()->cfCompForm.dwStyle);
  }

  // judge if we need to recourse to using the caret position
  if (!pos.x && !pos.y && !imc()->cfCompForm.rcArea.left &&
      !imc()->cfCompForm.rcArea.top && !imc()->cfCompForm.rcArea.right &&
      !imc()->cfCompForm.rcArea.bottom) {
    logger.debug("recourse to using caret position");
    pos = caretPoint;
  }

  logger.debug("Got caret position (%d,%d) by composition", pos.x, pos.y);
  ClientToScreen(imc()->hWnd, &pos);

  if (!client->isIMEAwareMode()) client->setInputBufferPosition(pos.x, pos.y);

  logger.debug("Translated caret position (%d,%d) by composition", pos.x,
               pos.y);

  if (alsoUpdateCandidatePanel) {
    client->setCandidatePanelPosition(
        pos.x, pos.y + (int)client->inputBufferFontSize() + 7);
  }
}

int BIDetermineCandForm(LPARAM number) {
  static int LookUpTable[] = {0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0};

  int bit = 0;
  int shift = -1;
  for (; number != 0; number >>= 4, shift++) bit = LookUpTable[number & 0x0f];

  return bit + shift * 4;
}

void BIUpdateCandidatePanelPosition(HIMC hIMC, int index) {
  IMEClient* client = IMEClient::SharedClient();
  Logger logger("BIUpdateCandidatePanelPosition");
  AutoIMC imc(hIMC);
  if (!imc()) return;

  LOGFONT* lfFont = (LOGFONT*)&imc()->lfFont;
  logger.debug("imc()->hWnd: 0x%08x, lfHeight: %i", imc()->hWnd,
               lfFont->lfHeight);

  POINT pos;
  pos.x = pos.y = 0;

  HDC hDC = GetDC(imc()->hWnd);
  TEXTMETRIC tm;
  GetTextMetrics(hDC, &tm);
  int localDpiY = GetDeviceCaps(hDC, LOGPIXELSY);
  ReleaseDC(imc()->hWnd, hDC);

  logger.debug("DC aspsects, localDpiY: %d, tmDigitizedAspectY: %d", localDpiY,
               tm.tmDigitizedAspectY);
  int fontPoints = (int)abs((LONG)ceilf((float)lfFont->lfHeight *
                                        (localDpiY / tm.tmDigitizedAspectY)));

  logger.debug("cfCompForm.ptCurrentPos: (%d, %d)",
               imc()->cfCompForm.ptCurrentPos.x,
               imc()->cfCompForm.ptCurrentPos.y);
  logger.debug("cfCandForm[%d].ptCurrentPos: (%d, %d)", index,
               imc()->cfCandForm[index].ptCurrentPos.x,
               imc()->cfCandForm[index].ptCurrentPos.y);

  POINT pt = imc()->cfCandForm[index].ptCurrentPos;
  logger.debug(
      "cfCandForm[%d].rcArea(left, top, right, bottom): (%d, %d, %d, %d)",
      index, imc()->cfCandForm[index].rcArea.left,
      imc()->cfCandForm[index].rcArea.top,
      imc()->cfCandForm[index].rcArea.right,
      imc()->cfCandForm[index].rcArea.bottom);

  logger.debug("Determines imc()::cfCandForm[%d].dwStyle", index);
  switch (imc()->cfCandForm[index].dwStyle) {
    case CFS_CANDIDATEPOS:
      logger.debug("CFS_CANDIDATEPOS");
      pos = imc()->cfCandForm[index].ptCurrentPos;
      ClientToScreen(imc()->hWnd, &pos);
      logger.debug("Set candidate pos to (%d, %d)", pos.x, pos.y);
      client->setCandidatePanelPosition(pos.x, pos.y);

      if (client->isIMEAwareMode()) {
        client->setInputBufferPosition(
            pos.x, pos.y - (int)client->inputBufferFontSize());
      }

      break;

    case CFS_EXCLUDE:
      logger.debug("CFS_EXCLUDE");
      pos.x = imc()->cfCandForm[index].ptCurrentPos.x;

      // @warning A workaround for PowerPoint, but not good enough for Excel.
      if (pos.x == imc()->cfCompForm.ptCurrentPos.x)
        pos.x = imc()->cfCandForm[index].rcArea.right;

      pos.y = imc()->cfCandForm[index].rcArea.bottom;
      ClientToScreen(imc()->hWnd, &pos);
      logger.debug("Set candidate pos to (%d, %d)", pos.x, pos.y);
      client->setCandidatePanelPosition(pos.x, pos.y);

      if (client->isIMEAwareMode()) {
        client->setInputBufferPosition(
            pos.x, pos.y - (int)client->inputBufferFontSize());
      }

      break;

    default:
      logger.debug("unknown: 0x%08x", imc()->cfCandForm[index].dwStyle);
      BIUpdateInputBufferPosition(hIMC, true);
      break;
  }
}

void BIForceCleanUp(HIMC hIMC, bool clearClientState) {
  static bool reentranceBarrier = false;

  IMEClient* client = IMEClient::SharedClient();

  if (client->reentranceBarrier.find((intptr_t)hIMC) !=
      client->reentranceBarrier.end()) {
    return;
  }

  client->reentranceBarrier.insert((intptr_t)hIMC);

  reentranceBarrier = true;

  Logger logger("ForceCleanUp");
  AutoIMC imc(hIMC);

  if (!imc()) {
    logger.debug("Fails to lock IMC");
    return;
  }

  AutoIMCC<LPCOMPOSITIONSTRING> compstr(imc()->hCompStr);
  LPARAM compositionParam = 0;

  if (client->isWMIMECompositionStarted()) {
    IMECompositionString result;

    logger.debug("Residue clean up: %s",
                 OVUTF8::FromUTF16(client->compositionString()).c_str());
    result.setCommittedText(client->compositionString());
    result.overwrite(compstr());
    compositionParam = GCS_RESULTSTR;

    IMEHelper::PostMessageToClient(hIMC, WM_IME_COMPOSITION, 0,
                                   compositionParam);
    IMEHelper::PostMessageToClient(hIMC, WM_IME_ENDCOMPOSITION, 0, 0);
    client->endWMIMEComposition();
  }

  if (clearClientState) client->clear();

  client->reentranceBarrier.erase((intptr_t)hIMC);
}
