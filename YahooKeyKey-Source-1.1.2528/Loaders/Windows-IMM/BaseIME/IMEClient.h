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

#ifndef IMEClient_h
#define IMEClient_h

#include <LFPlatform.h>
#include <OpenVanilla.h>

#include <set>

#include "BIClientRPCInterface.h"
#include "BIConfig.h"
#include "BIServerRPCInterface.h"

namespace BaseIME {
using namespace LFPlatform;
using namespace OpenVanilla;

class IMEClient {
 public:
  static void Start();
  static void Stop();
  static IMEClient* SharedClient();

 protected:
  static bool ServerExecutableAlreadyInProcessTable();
  static bool LaunchServerExecutable();
  static void CreateRPCConnection(Logger* logger);
  static void GetClientEndpointName(Logger* logger);
  static wchar_t* AllocString();
  static void ClearString(wchar_t* str);
  static void SetServerLocale();

 protected:
  static IMEClient* c_sharedClient;
  static RPCClientHelper::ServerHandle c_RPCServerInterfaceHandle;

 public:
  bool handleKey(unsigned int keyCode, unsigned int keyModifiers);

  void activate();
  void deactivate();

  void showStatusBar();
  void hideStatusBar();
  void setInputBufferPosition(int x, int y);
  void setInputBufferFontSize(long fontSize);
  long inputBufferFontSize();
  void showInputBuffer();
  void hideInputBuffer();
  void setCandidatePanelPosition(int x, int y);
  void showCandidatePanel();
  void hideCandidatePanel();
  void hideToolTip();

  void openPreferences();

 public:
  // clear the states
  void clear();

  // dump the states
  void dump();

  // context states
  wchar_t* actionParameter();
  wchar_t* actionVerb();
  wchar_t* candidatePageString();
  wchar_t* candidateKeyString();
  wchar_t* combinedCompositionString();
  wchar_t* combinedCompositionStringAttribute();
  wchar_t* committedString();
  wchar_t* compositionString();
  wchar_t* readingString();
  int cursorIndex();
  bool isCandidatePanelVisible();
  bool shouldUpdateCandidatePanel();
  int highlightedCandidateIndex();

  bool isCandidatePanelPreviouslyVisible();
  bool isCombinedCompositionStringPreviouslyEmpty();
  bool compositionStateDifferntFromPreviousOne();

  bool isIMEAwareMode();
  void setIMEAwareMode(bool mode);

  bool shouldSendWMCharForAsciiCommit();
  bool ignoreWMIMESetContextFalse();
  bool ignoreCPSComplete();
  bool ignoreCPSCompleteCleanUp();
  bool isPowerPoint();
  bool isGoogleChrome() { return m_isGoogleChrome; }

 protected:
  void launchApp(wchar_t* param);

 public:
  // WMIME client states
  bool isWMIMECompositionStarted();
  void startWMIMEComposition();
  void endWMIMEComposition();

 public:
  bool hasFakeKeyRequest();
  void setFakeKeyRequest(bool request);

 public:
  bool useWin32UI();

 protected:
  IMEClient();
  ~IMEClient();

 protected:
  BISContext m_RPCContext;

  bool m_contextActivated;
  bool m_WMIMECompositionStarted;

  long m_inputBufferFontSize;

  wchar_t* m_actionParameter;
  wchar_t* m_actionVerb;
  wchar_t* m_candidatePageString;
  wchar_t* m_candidateKeyString;
  wchar_t* m_combinedCompositionString;
  wchar_t* m_combinedCompositionStringAttribute;
  wchar_t* m_committedString;
  wchar_t* m_compositionString;
  wchar_t* m_readingString;
  wchar_t* m_currentInputMethodName;
  wchar_t* m_serverLocale;

  unsigned int m_cursorIndex;
  bool m_candidatePanelVisible;
  bool m_shouldUpdateCandidatePanel;
  unsigned int m_highlightedCandidateIndex;

  bool m_candidatePanelPreviouslyVisible;
  wstring m_oldCombinedComposistionString;
  wstring m_oldCombinedComposistionStringAttribute;
  unsigned int m_oldCursorIndex;

  wchar_t* m_clientRPCEndpointName;

  bool m_IMEAware;
  bool m_fakeKeyRequest;

  // app-specific fixes
  bool m_neverShowUI;
  bool m_useWin32UI;
  bool m_stopUsingServerSideUI;
  bool m_shouldSendWMCharForAsciiCommit;
  bool m_isPowerPoint;
  bool m_ignoreWMIMESetContextFalse;
  bool m_ignoreCPSComplete;
  bool m_ignoreCPSCompleteCleanUp;
  bool m_isGoogleChrome;

 public:
  set<intptr_t> reentranceBarrier;
};

inline IMEClient* IMEClient::SharedClient() { return c_sharedClient; }

inline wchar_t* IMEClient::AllocString() {
  return new wchar_t[BISMaxStringLength];
}

inline void IMEClient::ClearString(wchar_t* str) {
  wmemset(str, 0, BISMaxStringLength);
}

inline void IMEClient::clear() {
  m_contextActivated = false;
  m_WMIMECompositionStarted = false;
  ClearString(m_actionParameter);
  ClearString(m_actionVerb);
  ClearString(m_candidatePageString);
  ClearString(m_candidateKeyString);
  ClearString(m_combinedCompositionString);
  ClearString(m_combinedCompositionStringAttribute);
  ClearString(m_committedString);
  ClearString(m_compositionString);
  ClearString(m_readingString);

  m_cursorIndex = 0;
  m_candidatePanelVisible = false;
  m_shouldUpdateCandidatePanel = false;
  m_highlightedCandidateIndex = 0;

  m_candidatePanelPreviouslyVisible = false;

  m_oldCursorIndex = 0;
  m_oldCombinedComposistionString = L"";
  m_oldCombinedComposistionStringAttribute = L"";

  m_IMEAware = true;

  m_fakeKeyRequest = false;
}

inline void IMEClient::dump() {
#define WS(x) OVUTF8::FromUTF16(x).c_str()
  Logger logger("IMEClient");
  logger.debug("action verb: %s, parameter: %s", WS(m_actionVerb),
               WS(m_actionParameter));
  logger.debug("combined composition: %s, old composition: %s",
               WS(m_combinedCompositionString),
               WS(m_oldCombinedComposistionString));
  logger.debug("cursor: %d, old cursor: %d", m_cursorIndex, m_oldCursorIndex);
  logger.debug(
      "candidate panel visible: %d, previously: %d, should update: %d, "
      "highlight index; %d",
      m_candidatePanelVisible, m_candidatePanelPreviouslyVisible,
      m_shouldUpdateCandidatePanel, m_highlightedCandidateIndex);
#undef WS
}

inline wchar_t* IMEClient::actionParameter() { return m_actionParameter; }

inline wchar_t* IMEClient::actionVerb() { return m_actionVerb; }

inline wchar_t* IMEClient::candidatePageString() {
  return m_candidatePageString;
}

inline wchar_t* IMEClient::candidateKeyString() { return m_candidateKeyString; }

inline wchar_t* IMEClient::combinedCompositionString() {
  return m_combinedCompositionString;
}

inline wchar_t* IMEClient::combinedCompositionStringAttribute() {
  return m_combinedCompositionStringAttribute;
}

inline wchar_t* IMEClient::committedString() { return m_committedString; }

inline wchar_t* IMEClient::compositionString() { return m_compositionString; }

inline wchar_t* IMEClient::readingString() { return m_readingString; }

inline int IMEClient::cursorIndex() { return m_cursorIndex; }

inline bool IMEClient::isCandidatePanelVisible() {
  return m_candidatePanelVisible;
}

inline bool IMEClient::shouldUpdateCandidatePanel() {
  return m_shouldUpdateCandidatePanel;
}

inline int IMEClient::highlightedCandidateIndex() {
  return m_highlightedCandidateIndex;
}

inline bool IMEClient::isCandidatePanelPreviouslyVisible() {
  return m_candidatePanelPreviouslyVisible;
}

inline bool IMEClient::isCombinedCompositionStringPreviouslyEmpty() {
  return m_oldCombinedComposistionString.size() == 0;
}

inline bool IMEClient::compositionStateDifferntFromPreviousOne() {
  return m_oldCursorIndex != m_cursorIndex ||
         m_oldCombinedComposistionString != m_combinedCompositionString ||
         m_oldCombinedComposistionStringAttribute !=
             m_combinedCompositionStringAttribute;
}

inline bool IMEClient::isWMIMECompositionStarted() {
  return m_WMIMECompositionStarted;
}

inline void IMEClient::startWMIMEComposition() {
  m_WMIMECompositionStarted = true;
}

inline void IMEClient::endWMIMEComposition() {
  m_WMIMECompositionStarted = false;

  hideInputBuffer();
}

inline bool IMEClient::hasFakeKeyRequest() { return m_fakeKeyRequest; }

inline void IMEClient::setFakeKeyRequest(bool request) {
  m_fakeKeyRequest = request;
}

inline long IMEClient::inputBufferFontSize() { return m_inputBufferFontSize; }

inline bool IMEClient::isIMEAwareMode() { return m_IMEAware; }

inline void IMEClient::setIMEAwareMode(bool mode) { m_IMEAware = mode; }

inline IMEClient::IMEClient()
    : m_RPCContext(0),
      m_contextActivated(false),
      m_inputBufferFontSize(14),
      m_neverShowUI(false),
      m_useWin32UI(false),
      m_shouldSendWMCharForAsciiCommit(false),
      m_isPowerPoint(false),
      m_ignoreWMIMESetContextFalse(false),
      m_ignoreCPSComplete(false),
      m_ignoreCPSCompleteCleanUp(false),
      m_stopUsingServerSideUI(false),
      m_isGoogleChrome(false) {
  Logger logger("IMEClient");

  m_actionParameter = AllocString();
  m_actionVerb = AllocString();
  m_candidatePageString = AllocString();
  m_candidateKeyString = AllocString();
  m_combinedCompositionString = AllocString();
  m_combinedCompositionStringAttribute = AllocString();
  m_committedString = AllocString();
  m_compositionString = AllocString();
  m_readingString = AllocString();
  m_clientRPCEndpointName = AllocString();
  m_currentInputMethodName = AllocString();
  m_serverLocale = AllocString();

  clear();

  string process = ProcessHelper::CurrentProcessImageName();
  logger.debug("Attaching to process %s", process.c_str());

  bool incompatibles =
      OVWildcard::Match(process, "*wow.exe") ||         // World of Warcraft
      OVWildcard::Match(process, "*lin.bin") ||         // Lineage 1
      OVWildcard::Match(process, "*GoogleDesktop*") ||  // Google Desktop
      OVWildcard::Match(process, "*GoogleQuick*") ||  // Google Quick Searchbar
      OVWildcard::Match(process,
                        "*Empires*.exe") ||  // Age of Empires (all versions)
      OVWildcard::Match(process,
                        "*age2_x1*.exe") ||  // Age of Empires Data Disk
      OVWildcard::Match(process,
                        "*elementclient.exe");  // ElementClient game engine

  if (incompatibles) {
    m_neverShowUI = true;
    m_useWin32UI = true;
    m_stopUsingServerSideUI = true;
  }

  // fix Maple Story, PowerPoint and Skype
  if (OVWildcard::Match(process, "*MapleStory.exe")) {
    // TO DO: Maybe *lin.bin too?
    m_ignoreCPSComplete = true;
  }

  if (OVWildcard::Match(process, "*POWERPNT.exe")) {
    m_ignoreCPSComplete = true;
    m_isPowerPoint = true;
  }

  if (OVWildcard::Match(process, "*Skype.exe")) {
    m_ignoreCPSCompleteCleanUp = true;
  }

  if (OVWildcard::Match(process, "*YahooMessenger.exe")) {
    m_shouldSendWMCharForAsciiCommit = true;
  }

  if (OVWildcard::Match(process, "*excel.exe")) {
    m_ignoreWMIMESetContextFalse = true;
  }

  if (OVWildcard::Match(process, "*chrome.exe")) {
    m_isGoogleChrome = true;
  }
}

inline IMEClient::~IMEClient() {
  delete m_clientRPCEndpointName;
  delete m_readingString;
  delete m_compositionString;
  delete m_committedString;
  delete m_combinedCompositionStringAttribute;
  delete m_combinedCompositionString;
  delete m_candidatePageString;
  delete m_candidateKeyString;
  delete m_actionVerb;
  delete m_actionParameter;
}

inline bool IMEClient::shouldSendWMCharForAsciiCommit() {
  return m_shouldSendWMCharForAsciiCommit;
}

inline bool IMEClient::ignoreWMIMESetContextFalse() {
  return m_ignoreWMIMESetContextFalse;
}

inline bool IMEClient::ignoreCPSComplete() { return m_ignoreCPSComplete; }

inline bool IMEClient::ignoreCPSCompleteCleanUp() {
  return m_ignoreCPSCompleteCleanUp;
}

inline bool IMEClient::isPowerPoint() { return m_isPowerPoint; }

inline bool IMEClient::useWin32UI() { return m_useWin32UI; }
};  // namespace BaseIME

#endif