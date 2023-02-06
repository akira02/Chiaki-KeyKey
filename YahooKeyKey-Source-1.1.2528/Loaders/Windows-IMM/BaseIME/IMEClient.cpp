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

#include "IMEClient.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <shellapi.h>
#include <windows.h>

#include <cmath>

#include "IMECandidatePanel.h"
#include "IMEInputBuffer.h"
#include "IMEStatusBar.h"

namespace BaseIME {

using namespace OpenVanilla;

IMEClient* IMEClient::c_sharedClient = 0;
RPCClientHelper::ServerHandle IMEClient::c_RPCServerInterfaceHandle = 0;

#define RPC_BEGIN            \
  if (!m_RPCContext) return; \
  RpcTryExcept
#define RPC_END                      \
  RpcExcept(1) { m_RPCContext = 0; } \
  RpcEndExcept
// #define RPC_MSGEND(msg)     RpcExcept(1) { m_RPCContext = 0; MessageBox(0,
// msg, L"IME Client", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL); }
// RpcEndExcept
#define RPC_MSGEND(msg)              \
  RpcExcept(1) { m_RPCContext = 0; } \
  RpcEndExcept

void IMEClient::Start() {
  Logger logger("IMEClient");

  if (c_sharedClient) return;

  c_sharedClient = new IMEClient;

  // init the RPC server service interface
  Logger sublogger("EstablishConnection");

  string currentProcessImageName = ProcessHelper::CurrentProcessImageName();

  if (ProcessHelper::IsSystemUser()) {
    sublogger.debug("No connection to server if run under SYSTEM user");
  } else if (OVWildcard::Match(currentProcessImageName, "*winlogon.exe") ||
             OVWildcard::Match(currentProcessImageName,
                               "*" BASEIME_SERVER_EXECUTABLE)) {
    sublogger.debug("No connection to process named: %s",
                    currentProcessImageName.c_str());
  } else {
    if (!(c_RPCServerInterfaceHandle = RPCClientHelper::ConnectServer(
              BASEIME_SERVER_IDENTIFIER, &BIServerRPCInterfaceGlobalHandle))) {
      logger.error("Cannot init the RPC service interface");
      return;
    }

    CreateRPCConnection(&sublogger);
    GetClientEndpointName(&sublogger);

    logger.debug(
        "Have we obtained client RPC endpoint name? '%s'",
        OVUTF8::FromUTF16(c_sharedClient->m_clientRPCEndpointName).c_str());

    if (wcslen(c_sharedClient->m_clientRPCEndpointName)) {
      logger.debug("Creating client's RPC service");

      if (!RPCServerHelper::StartService(
              OVUTF8::FromUTF16(c_sharedClient->m_clientRPCEndpointName),
              BIClientRPCInterface_v1_0_s_ifspec)) {
        logger.error("Cannot create client's RPC service, ignoring");
        ClearString(c_sharedClient->m_clientRPCEndpointName);
      }
    }
  }
}

void IMEClient::Stop() {
  if (c_sharedClient) {
    if (wcslen(c_sharedClient->m_clientRPCEndpointName)) {
      RPCServerHelper::StopService();
    }

    if (c_sharedClient->m_RPCContext) {
      RpcTryExcept { BISCloseConnection(&c_sharedClient->m_RPCContext); }
      RpcExcept(1) {
        // silently ignores the problem since it's near the end
        // MessageBox(0, L"Stop", L"IME Client", MB_OK | MB_ICONINFORMATION |
        // MB_SYSTEMMODAL);
      }
      RpcEndExcept;
    }

    delete c_sharedClient;
  }

  // finalize the RPC server service interface
  if (c_RPCServerInterfaceHandle)
    RPCClientHelper::Disconnect(c_RPCServerInterfaceHandle,
                                &BIServerRPCInterfaceGlobalHandle);
}

void IMEClient::SetServerLocale() {
  RpcTryExcept {
    BISStatelessFunctionGetString(BISFunctionGetServerLocale,
                                  c_sharedClient->m_serverLocale);
    IMEStatusBar::SetServerLocale(c_sharedClient->m_serverLocale);
  }
  RpcExcept(1) {}
  RpcEndExcept;
}

void IMEClient::CreateRPCConnection(Logger* logger) {
  if (!c_sharedClient) return;

  BISContext& context = c_sharedClient->m_RPCContext;
  bool connected = false;
  bool launched = false;

  DWORD tick = 0;
  DWORD interval = 250;  // 250 ms
  DWORD timeout =
      BASEIME_SERVER_STARTUP_TIMEOUT * 1000;  // timeout in milliseconds

  while (!connected && tick < timeout) {
    RpcTryExcept {
      boolean result = BISOpenConnection(&context);
      if (!result) {
        // we recount the stuff until the server's UI is ready
        tick = 0;
        while (tick < timeout) {
          Sleep(interval);
          tick += interval;
          logger->debug(
              "Trying to wait for UI to be ready, tick = %d (each of %d ms)",
              tick / interval, interval);
          Logger::FlushLogger();

          result = BISOpenConnection(&context);
          if (result) {
            SetServerLocale();
            break;
          }
        }

        if (!result) {
          logger->error("Server returns error");
          return;
        }
      } else {
        SetServerLocale();
      }

      connected = true;
    }
    RpcExcept(1) {
      if (!launched) {
        if (ServerExecutableAlreadyInProcessTable()) {
          logger->error("Already in process table, not launching");
        } else {
          logger->debug("Launching the server");
          if (!LaunchServerExecutable()) {
            logger->error("Can't even find the server, ended");
            return;
          }
        }

        launched = true;
      }

      Sleep(interval);
      tick += interval;
      logger->debug("Trying to connect, tick = %d (each of %d ms)",
                    tick / interval, interval);
      Logger::FlushLogger();
    }

    RpcEndExcept;
  }
}

bool IMEClient::ServerExecutableAlreadyInProcessTable() {
  return ProcessHelper::ProcessNameExistsForCurrentUser(
      "*" BASEIME_SERVER_EXECUTABLE);
}

bool IMEClient::LaunchServerExecutable() {
  Logger logger("Launcher");
  logger.debug("Preparing to launch the server");
  logger.debug("Finding the registry key HKEY_LOCAL_MACHINE\\%s",
               BASEIME_SERVER_REGKEY);

  string launchPath = BASEIME_SERVER_DEFAULT_LOCATION;
  string executable = BASEIME_SERVER_EXECUTABLE;

  if (!RegistryHelper::KeyExists(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY)) {
    logger.error("cannot find the registry key, use default location: %s",
                 launchPath.c_str());
  } else {
    HKEY key =
        RegistryHelper::OpenKey(HKEY_LOCAL_MACHINE, BASEIME_SERVER_REGKEY);
    logger.debug("Querying value named: %s",
                 BASEIME_SERVER_LOCATION_VALUE_NAME);
    string location = RegistryHelper::QueryStringValue(
        key, BASEIME_SERVER_LOCATION_VALUE_NAME);

    if (location.size()) {
      launchPath = location;
      logger.debug("Found server location: %s", launchPath.c_str());
    }

    RegCloseKey(key);
  }

  string execPath = OVPathHelper::PathCat(launchPath, executable);
  logger.debug("Launching %s", execPath.c_str());

  if (!ProcessHelper::LaunchProgram(execPath, launchPath)) {
    logger.error("Cannot launch server");
    return false;
  } else {
    logger.debug("Server launched");
  }

  return true;
}

void IMEClient::GetClientEndpointName(Logger* logger) {
  if (!c_sharedClient) return;

  ClearString(c_sharedClient->m_clientRPCEndpointName);

  if (!c_sharedClient->m_RPCContext) return;

  RpcTryExcept {
    BISNextAvailableClientRPCEndpointName(
        c_sharedClient->m_RPCContext, c_sharedClient->m_clientRPCEndpointName);
  }
  RpcExcept(1) {
    MessageBox(0, L"GetClientEndpointName", L"IME Client",
               MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);

    c_sharedClient->m_RPCContext = 0;
  }
  RpcEndExcept;
}

bool IMEClient::handleKey(unsigned int keyCode, unsigned int keyModifiers) {
  boolean handled = FALSE;

  if (!m_RPCContext) return false;

  RpcTryExcept {
    m_candidatePanelPreviouslyVisible = m_candidatePanelVisible;
    m_oldCursorIndex = m_cursorIndex;
    m_oldCombinedComposistionString = m_combinedCompositionString;
    m_oldCombinedComposistionStringAttribute =
        m_combinedCompositionStringAttribute;

    boolean candidatePanelVisible = FALSE;
    boolean shouldUpdateCandidatePanel = FALSE;

    handled = BISHandleKey(
        m_RPCContext, keyCode, keyModifiers, m_committedString,
        m_combinedCompositionString, m_combinedCompositionStringAttribute,
        m_compositionString, m_readingString, m_candidatePageString,
        m_candidateKeyString, m_actionVerb, m_actionParameter, &m_cursorIndex,
        &candidatePanelVisible, &shouldUpdateCandidatePanel,
        &m_highlightedCandidateIndex);

    m_candidatePanelVisible = (candidatePanelVisible == TRUE);
    m_shouldUpdateCandidatePanel = (shouldUpdateCandidatePanel == TRUE);

    if (IMECandidatePanel::SharedPanel()) {
      IMECandidatePanel::SharedPanel()->updateContent(
          m_candidatePageString, m_candidateKeyString,
          m_highlightedCandidateIndex);
    }

    if (IMEInputBuffer::SharedInputBuffer()) {
      IMEInputBuffer::SharedInputBuffer()->updateContent(
          m_combinedCompositionString, m_combinedCompositionStringAttribute,
          m_cursorIndex);

      if (!wcslen(m_combinedCompositionString)) {
        IMEInputBuffer::SharedInputBuffer()->hide();
      }
    }

    if (!wcscmp(m_actionVerb, L"open")) {
      ShellExecute(0, L"open", m_actionParameter, NULL, L".\\", SW_SHOWNORMAL);
    } else if (!wcscmp(m_actionVerb, L"launchApp")) {
      launchApp(m_actionParameter);
    }
  }
  RPC_MSGEND(L"Handle Key");

  return handled == TRUE;
}

void IMEClient::launchApp(wchar_t* param) {
  vector<string> p = OVStringHelper::Split(OVUTF8::FromUTF16(param), '\t');
  if (!p.size()) {
    return;
  }

  wstring wa = OVUTF16::FromUTF8(p[0]).c_str();
  wstring wp = (p.size() > 1) ? OVUTF16::FromUTF8(p[1]).c_str() : L"";

  const wchar_t* app = wa.c_str();
  const wchar_t* params = wp.length() ? wp.c_str() : NULL;
  ShellExecute(0, L"open", app, params, L".\\", SW_SHOWNORMAL);
}

void IMEClient::activate() {
  if (m_contextActivated) return;
  m_contextActivated = true;

  RPC_BEGIN {
    int x = 0, y = 0;
    BISActivate(m_RPCContext, m_stopUsingServerSideUI, m_currentInputMethodName,
                &x, &y);
    BISSetCurrentClientRPCEndpoint(m_RPCContext, m_clientRPCEndpointName);

    if (IMEStatusBar::SharedStatusBar() && m_useWin32UI) {
      //          IMEStatusBar::SharedStatusBar()->moveTo(x, y);
      IMEStatusBar::SharedStatusBar()->updateContent(m_currentInputMethodName);
    }
  }
  RPC_MSGEND(L"Activate");
}

void IMEClient::deactivate() {
  if (!m_contextActivated) return;
  m_contextActivated = false;

  RPC_BEGIN {
    // Deactivate also does clean-up
    // BISClearContextState(m_RPCContext);
    BISEndCurrentClientRPCEndpoint(m_RPCContext);
    BISDeactivate(m_RPCContext, m_compositionString);
  }
  RPC_MSGEND(L"Deactivate");
}

void IMEClient::showStatusBar() {
  RPC_BEGIN {
    if (!m_neverShowUI) BISShowStatusBar(m_RPCContext);

    if (IMEStatusBar::SharedStatusBar() && m_useWin32UI) {
      IMEStatusBar::SharedStatusBar()->moveToRememberdLocation();
      IMEStatusBar::SharedStatusBar()->show();
      IMEStatusBar::SharedStatusBar()->updateContent(m_currentInputMethodName);
    }
  }
  RPC_MSGEND(L"Show Status Bar");
}

void IMEClient::hideStatusBar() {
  RPC_BEGIN {
    BISHideStatusBar(m_RPCContext);

    if (IMEStatusBar::SharedStatusBar() && m_useWin32UI) {
      IMEStatusBar::SharedStatusBar()->hide();
    }
  }
  RPC_MSGEND(L"Hide Status Bar");
}

void IMEClient::showInputBuffer() {
  RPC_BEGIN {
    if (!m_neverShowUI) BISShowInputBuffer(m_RPCContext);

    if (IMEInputBuffer::SharedInputBuffer() && m_useWin32UI) {
      IMEInputBuffer::SharedInputBuffer()->show();
      IMEInputBuffer::SharedInputBuffer()->updateContent(
          m_combinedCompositionString, m_combinedCompositionStringAttribute,
          m_cursorIndex);
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"showInputBuffer", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::hideInputBuffer() {
  RPC_BEGIN {
    BISHideInputBuffer(m_RPCContext);
    if (IMEInputBuffer::SharedInputBuffer() && m_useWin32UI) {
      IMEInputBuffer::SharedInputBuffer()->hide();
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"hideInputBuffer", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::setInputBufferPosition(int x, int y) {
  RPC_BEGIN {
    BISSetInputBufferPosition(m_RPCContext, x, y);

    if (IMEInputBuffer::SharedInputBuffer() && m_useWin32UI) {
      IMEInputBuffer::SharedInputBuffer()->moveTo(x, y);
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"setInputBufferPosition", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::setInputBufferFontSize(long fontSize) {
  m_inputBufferFontSize = abs(fontSize);

  if (m_inputBufferFontSize < 14) {
    m_inputBufferFontSize = 14;
  }

  RPC_BEGIN { BISSetInputBufferTraits(m_RPCContext, m_inputBufferFontSize); }
  RpcExcept(1) {
    // MessageBox(0, L"setInputBufferFontSize", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::setCandidatePanelPosition(int x, int y) {
  RPC_BEGIN {
    BISSetCandidatePanelPosition(m_RPCContext, x, y);

    if (IMECandidatePanel::SharedPanel() && m_useWin32UI) {
      IMECandidatePanel::SharedPanel()->moveTo(x, y);
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"setCandidatePanelPosition", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::showCandidatePanel() {
  RPC_BEGIN {
    if (!m_neverShowUI) BISShowCandidatePanel(m_RPCContext);

    if (IMECandidatePanel::SharedPanel() && m_useWin32UI) {
      IMECandidatePanel::SharedPanel()->show();
      IMECandidatePanel::SharedPanel()->updateContent(
          m_candidatePageString, m_candidateKeyString,
          m_highlightedCandidateIndex);
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"showCandidatePanel", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::hideCandidatePanel() {
  RPC_BEGIN {
    BISHideCandidatePanel(m_RPCContext);

    if (IMECandidatePanel::SharedPanel() && m_useWin32UI) {
      IMECandidatePanel::SharedPanel()->hide();
    }
  }
  RpcExcept(1) {
    // MessageBox(0, L"hideCandidatePanel", L"IME Client", MB_OK |
    // MB_ICONINFORMATION | MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::hideToolTip() {
  RPC_BEGIN { BISHideToolTip(m_RPCContext); }
  RpcExcept(1) {
    // MessageBox(0, L"hideToolTip", L"IME Client", MB_OK | MB_ICONINFORMATION |
    // MB_SYSTEMMODAL);

    m_RPCContext = 0;
  }
  RpcEndExcept
  // RPC_END;
}

void IMEClient::openPreferences() {
  RPC_BEGIN { BISOpenPreferences(m_RPCContext); }
  RPC_MSGEND(L"Open Prefences");
}

};  // namespace BaseIME
