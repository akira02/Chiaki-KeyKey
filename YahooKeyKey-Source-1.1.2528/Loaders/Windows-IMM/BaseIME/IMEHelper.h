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

#ifndef IMEHelper_h
#define IMEHelper_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <immdev.h>
#include <tchar.h>
#include <windows.h>

namespace BaseIME {

class AutoIMC {
 public:
  AutoIMC(HIMC handle) : m_handle(handle) { m_IMC = ImmLockIMC(handle); }

  ~AutoIMC() {
    if (m_IMC) ImmUnlockIMC(m_handle);
  }

  LPINPUTCONTEXT operator()() { return m_IMC; }

 protected:
  HIMC m_handle;
  LPINPUTCONTEXT m_IMC;
};

template <class T>
class AutoIMCC {
 public:
  AutoIMCC(HIMCC handle) : m_handle(handle) { m_data = (T)ImmLockIMCC(handle); }

  ~AutoIMCC() {
    if (m_data) ImmUnlockIMCC(m_handle);
  }

  T operator()() { return m_data; }

 protected:
  HIMCC m_handle;
  T m_data;
};

class IMEHelper {
 public:
  static DWORD PostMessageToClient(HIMC hIMC, UINT msg, WPARAM wParam,
                                   LPARAM lParam);

  static HIMC IMCFromWindow(HWND window) {
#pragma warning(push)
#pragma warning(disable : 4312)
    return (HIMC)(GetWindowLongPtr(window, IMMGWLP_IMC));
#pragma warning(pop)
  }
};

struct KeyInfo {
  static KeyInfo FromLPARAM(LPARAM lparam) { return *(KeyInfo*)&lparam; }

  UINT repeatCount : 16;
  UINT scanCode : 8;
  UINT isExtended : 1;
  UINT reserved : 4;
  UINT contextCode : 1;
  UINT prevKeyState : 1;
  UINT isKeyUp : 1;  // transition state
};

class KeyHelper {
 public:
  static bool IsKeyStateDown(BYTE keyState) { return !!(keyState & 0xF0); }
};

// this must be the same size as the BISMaxStringLength
#define BASEIME_COMPOSITION_STRING_SIZE 512

struct IMECompositionString {
  COMPOSITIONSTRING info;
  wchar_t readingText[BASEIME_COMPOSITION_STRING_SIZE];
  wchar_t composingText[BASEIME_COMPOSITION_STRING_SIZE];
  char composingAttribute[BASEIME_COMPOSITION_STRING_SIZE];
  wchar_t committedText[BASEIME_COMPOSITION_STRING_SIZE];
  // DWORD composingClause[BASEIME_COMPOSITION_STRING_SIZE + 1];
  // DWORD committedClause[BASEIME_COMPOSITION_STRING_SIZE + 1];
  // DWORD readingClause[2];

  IMECompositionString() {
    size_t infoSize = sizeof(info);
    memset(&info, 0, infoSize);
    wmemset(readingText, 0, BASEIME_COMPOSITION_STRING_SIZE);
    wmemset(composingText, 0, BASEIME_COMPOSITION_STRING_SIZE);
    memset(composingAttribute, 0, BASEIME_COMPOSITION_STRING_SIZE);
    wmemset(committedText, 0, BASEIME_COMPOSITION_STRING_SIZE);
    // lmemset(composingClause, 0, BASEIME_COMPOSITION_STRING_SIZE + 1);
    // lmemset(committedClause, 0, BASEIME_COMPOSITION_STRING_SIZE + 1);
    // readingClause[0] = readingClause[1] = 0;

    info.dwSize = sizeof(IMECompositionString);
    info.dwCompReadStrOffset = (DWORD)infoSize;
    info.dwCompStrOffset = (DWORD)infoSize + sizeof(readingText);
    info.dwCompAttrOffset =
        (DWORD)infoSize + sizeof(readingText) + sizeof(composingText);
    info.dwResultStrOffset = (DWORD)infoSize + sizeof(readingText) +
                             sizeof(composingText) + sizeof(composingAttribute);
    // info.dwCompClauseOffset = (DWORD)infoSize + sizeof(readingText) +
    // sizeof(composingText) + sizeof(composingAttribute) +
    // sizeof(committedText); info.dwResultClauseOffset = (DWORD)infoSize +
    // sizeof(readingText) + sizeof(composingText) + sizeof(composingAttribute)
    // + sizeof(committedText) + sizeof(composingClause);
    // info.dwCompReadClauseOffset = info.dwResultReadClauseOffset =
    //     (DWORD)infoSize + sizeof(readingText) + sizeof(composingText) +
    //     sizeof(composingAttribute) + sizeof(committedText) +
    //     sizeof(composingClause) + sizeof(committedClause);
    // info.dwCompClauseLen
    //     = info.dwCompReadClauseLen
    //     = info.dwResultClauseLen
    //     = info.dwResultReadClauseLen
    //         = 8;
  }

  IMECompositionString(LPCOMPOSITIONSTRING lpcs) {
    memcpy(this, lpcs, sizeof(IMECompositionString));
  }

  DWORD size() { return (DWORD)sizeof(IMECompositionString); }

  void setCursorIndex(size_t index) { info.dwCursorPos = (DWORD)index; }

  void setReadingText(wchar_t* text) {
    wcscpy_s(readingText, BASEIME_COMPOSITION_STRING_SIZE, text);
    info.dwCompReadStrLen = (DWORD)wcslen(readingText);
  }

  void setComposingText(wchar_t* text) {
    wcscpy_s(composingText, BASEIME_COMPOSITION_STRING_SIZE, text);
    info.dwCompStrLen = (DWORD)wcslen(composingText);

    // for (size_t i = 0; i <= info.dwCompStrLen; ++i)
    //     composingClause[i] = i;
    // info.dwCompClauseLen = 8 + 4 * (info.dwCompStrLen - 1);
  }

  void setComposingAttributeLength(size_t lengthInBlock) {
    size_t copyLength = lengthInBlock > BASEIME_COMPOSITION_STRING_SIZE
                            ? BASEIME_COMPOSITION_STRING_SIZE
                            : lengthInBlock;
    info.dwCompAttrLen = (DWORD)copyLength;
  }

  void setComposingAttributeByIndex(size_t index, char attr) {
    if (index >= BASEIME_COMPOSITION_STRING_SIZE) return;

    composingAttribute[index] = attr;
  }

  void setCommittedText(wchar_t* text) {
    wcscpy_s(committedText, BASEIME_COMPOSITION_STRING_SIZE, text);
    info.dwResultStrLen = (DWORD)wcslen(committedText);

    // for (size_t i = 0; i <= info.dwResultStrLen; ++i)
    //     committedClause[i] = i;
    // info.dwResultClauseLen = 8 + 4 * (info.dwResultStrLen - 1);
  }

  void overwrite(LPCOMPOSITIONSTRING lpcs) { memcpy(lpcs, this, info.dwSize); }

  void PASCAL lmemset(LPDWORD target, DWORD value, size_t count) {
    register size_t i;
    register DWORD b = value;
    for (i = 0; i < count; ++i) *target++ = b;
  }
};

// BICandidateInfo combines two IMM structures:
// CANDIDATEINFO and CANDIDATELIST
//
// In MS's IMM, a CANDIDATEINFO seems to be able to accommodate at most 32
// different candidate lists. Why this is needed, no one knows, perhaps for
// pagination reasons.
//
// In our implementation, the structure must contain a CANDIDATEINFO,
// then a single CANDIDATELIST. Because CANDIDATELIST already has *one
// element*, so we must extend ("follow") the structure and extend it.
//
//    typedef struct tagCANDIDATEINFO {
//        DWORD               dwSize;
//        DWORD               dwCount;
//        DWORD               dwOffset[32];
//        DWORD               dwPrivateSize;
//        DWORD               dwPrivateOffset;
//    } CANDIDATEINFO, *PCANDIDATEINFO, NEAR *NPCANDIDATEINFO, FAR
//    *LPCANDIDATEINFO;
//
//    typedef struct tagCANDIDATELIST {
//        DWORD dwSize;
//        DWORD dwStyle;
//        DWORD dwCount;
//        DWORD dwSelection;
//        DWORD dwPageStart;
//        DWORD dwPageSize;
//        DWORD dwOffset[1];
//    } CANDIDATELIST, *PCANDIDATELIST, NEAR *NPCANDIDATELIST, FAR
//    *LPCANDIDATELIST;
//

#define BASEIME_MAX_CANDIDATE_COUNT 128
#define BASEIME_CANDIDATE_STRING_LENGTH 48

struct IMECandidateInfo {
 public:
  CANDIDATEINFO candidateInfo;

  // these two members should be treated as one
  CANDIDATELIST candidateList;
  DWORD more_candidateList_dwOffset[BASEIME_MAX_CANDIDATE_COUNT - 1];

  wchar_t candidateStrings[BASEIME_MAX_CANDIDATE_COUNT]
                          [BASEIME_CANDIDATE_STRING_LENGTH];

  void overwrite(LPCANDIDATEINFO lpci) {
    memcpy(lpci, this, sizeof(IMECandidateInfo));
  }

  DWORD size() { return (DWORD)sizeof(IMECandidateInfo); }

  void setHighlightedIndex(DWORD index) {
    if (index >= candidateList.dwCount)
      candidateList.dwSelection = 0;
    else
      candidateList.dwSelection = index;
  }

  IMECandidateInfo() {
    char* candiInfoPtr = (char*)&candidateInfo;
    char* candiListPtr = (char*)&candidateList;

    candidateInfo.dwSize = sizeof(IMECandidateInfo);
    candidateInfo.dwCount = 1;  // one candidate list
    candidateInfo.dwOffset[0] = (DWORD)(candiListPtr - candiInfoPtr);
    candidateInfo.dwPrivateSize = 0;
    candidateInfo.dwPrivateOffset = 0;

    candidateList.dwSize = sizeof(candidateList) +
                           sizeof(more_candidateList_dwOffset) +
                           sizeof(candidateStrings);
    candidateList.dwStyle = IME_CAND_READ;
    candidateList.dwCount = 0;
    candidateList.dwSelection = 0;
    candidateList.dwPageStart = 0;

    for (size_t index = 0; index < BASEIME_MAX_CANDIDATE_COUNT; ++index) {
      candidateList.dwOffset[index] =
          (DWORD)((char*)candidateStrings[index] - candiListPtr);
      wmemset(candidateStrings[index], 0, BASEIME_CANDIDATE_STRING_LENGTH);
    }
  }

  // this expects the continuous string to end in two \0, e.g.:
  //   "a\0b\0\c\0\0" yields candidates a, b, c
  void setCandidatesFromContinuousString(wchar_t* wstr) {
#define MAX_SKIP_LENGTH 1024
    size_t index, length = wcsnlen(wstr, 1024);
    for (index = 0; index < length; index++) {
      if (wstr[index]) {
        // debug info here
      }

      if (wstr[index] == 0xffff) wstr[index] = 0;
    }

    for (index = 0; index < BASEIME_MAX_CANDIDATE_COUNT; ++index) {
      size_t skiplen = wcsnlen(wstr, MAX_SKIP_LENGTH);
      if (!skiplen) break;

      wcsncpy_s(candidateStrings[index], BASEIME_CANDIDATE_STRING_LENGTH, wstr,
                _TRUNCATE);

      // BIDebugLog("Adding candidate %s, index = %d, skiplen = %d",
      // OVUTF8::FromUTF16(wstring(wstr)).c_str(), index, skiplen);

      wstr += (skiplen + 1);  // skip the skiplen, plus the next \0
    }
#undef MAX_SKIP_LENGTH

    candidateList.dwCount = (DWORD)index;
    candidateList.dwSelection = 0;
    candidateList.dwPageStart = 0;
    candidateList.dwPageSize = (DWORD)index;
  }
};

};  // namespace BaseIME

#endif