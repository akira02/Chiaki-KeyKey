#pragma once
#include "BIServerRPCInterface.h"
#include "OpenVanilla.h"

extern BISContext PMSharedContext;

namespace PhraseEditor {

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace OpenVanilla;

#ifndef SUPPRESS_TAKAO_PREFERENCE
using namespace TakaoPreference;

ref class ManagedPreferenceConnector : public PreferenceConnector {
 public:
  virtual bool importPhraseDB(String ^ filename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
    RpcTryExcept return BISImportUserPhraseDB(PMSharedContext, wchptr) == TRUE;
    RpcExcept(1) { return FALSE; }
    RpcEndExcept
  }

  virtual bool exportPhraseDB(String ^ filename) override {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(filename);
    RpcTryExcept return BISExportUserPhraseDB(PMSharedContext, wchptr) == TRUE;
    RpcExcept(1) { return FALSE; }
    RpcEndExcept
  }
};
#endif

public
ref class PhraseDBService {
 public:
  static bool CanProvideService() {
    RpcTryExcept return BISStatelessFunctionGetBool(
        BISFunctionPhraseDBCanProvideService) == TRUE;
    RpcExcept(1) { return FALSE; }
    RpcEndExcept
  }

  static int NumberOfRow() {
    RpcTryExcept return BISStatelessFunctionGetInt(
        BISFunctionPhraseDBNumberOfRow);
    RpcExcept(1) { return 0; }
    RpcEndExcept
  }

  static void InvokePhraseDBDataAtRow(wchar_t* output, int row) {
    RpcTryExcept BISStatelessFunctionGetStringWithInt(
        BISFunctionPhraseDBDataAtRow, output, row);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static KeyValuePair<String ^, String ^> DataAtRow(int row) {
    wchar_t output[BISMaxStringLength];
    wmemset(output, 0, BISMaxStringLength);

    InvokePhraseDBDataAtRow(output, row);
    if (!wcslen(output)) {
      return KeyValuePair<String ^, String ^>(String::Empty, String::Empty);
    }

    vector<string> s = OVStringHelper::Split(OVUTF8::FromUTF16(output), '\t');
    if (s.size() >= 2) {
      return KeyValuePair<String ^, String ^>(
          gcnew String(OVUTF16::FromUTF8(s[0]).c_str()),
          gcnew String(OVUTF16::FromUTF8(s[1]).c_str()));
    }

    return KeyValuePair<String ^, String ^>(String::Empty, String::Empty);
  }

  static void InvokePhraseDBReadingsForCharacter(wchar_t* output,
                                                 const wchar_t* wchptr) {
    RpcTryExcept BISStatelessFunctionGetStringWithString(
        BISFunctionPhraseDBReadingsForCharacter, output, wchptr);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static List<String ^> ^
      ReadingsForCharacter(String ^ chr) {
        List<String ^> ^ result = gcnew List<String ^>();

        pin_ptr<const wchar_t> wchptr = PtrToStringChars(chr);
        wchar_t output[BISMaxStringLength];
        wmemset(output, 0, BISMaxStringLength);

        InvokePhraseDBReadingsForCharacter(output, wchptr);

        vector<string> ovec =
            OVStringHelper::Split(OVUTF8::FromUTF16(output), '\t');
        for (vector<string>::iterator ovi = ovec.begin(); ovi != ovec.end();
             ++ovi) {
          result->Add(gcnew String(OVUTF16::FromUTF8(*ovi).c_str()));
        }

        return result;
      }

      static void Save() {
    RpcTryExcept BISStatelessFunctionVoid(BISFunctionPhraseDBSave);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static void SetNewReadingForPhraseAtRow(String ^ reading, int row) {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(reading);
    RpcTryExcept BISStatelessFunctionSetStringAndInt(
        BISFunctionPhraseDBSetNewReadingForPhraseAtRow, wchptr, row);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static void DeleteRow(int row) {
    RpcTryExcept BISStatelessFunctionSetInt(BISFunctionPhraseDBDeleteRow, row);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static void AddNewRow(String ^ phrase) {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(phrase);
    RpcTryExcept BISStatelessFunctionSetString(BISFunctionPhraseDBAddNewRow,
                                               wchptr);
    RpcExcept(1) {}
    RpcEndExcept
  }

  static void SetPhraseAtRow(String ^ phrase, int row) {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(phrase);

    RpcTryExcept BISStatelessFunctionSetStringAndInt(
        BISFunctionPhraseDBSetPhraseAtRow, wchptr, row);
    RpcExcept(1) {}
    RpcEndExcept
  }
};
};  // namespace PhraseEditor
