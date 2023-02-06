#pragma once
#include <vcclr.h>

#include "EditorForm.h"
#include "OpenVanilla.h"
#include "PhraseDBService.h"

namespace PhraseEditor {

using namespace System;
using namespace System::Text;
using namespace System::Web;

ref class URLHandler {
 public:
  static void addPhraseFromURL(System::String ^ url) {
    if (!url->StartsWith(L"ykeykey://")) return;

    String ^ decodedString =
        HttpUtility::UrlDecode(url, Encoding::GetEncoding(L"utf-8"));

    decodedString = decodedString->Substring(10);
    decodedString = decodedString->Replace(L"/", String::Empty);

    array<Char> ^ chars = {'_'};
    array<String ^> ^ anArray = decodedString->Split(chars);
    if (anArray->Length < 2) {
      String ^ phrase = EditorForm::ValidatedString(decodedString);
      if (!phrase->Length) {
        // alert
        return;
      }
      PhraseDBService::AddNewRow(phrase);
      PhraseDBService::Save();
      MessageBox::Show("Add new phrase: " + phrase);
    } else if (anArray->Length == 2) {
      String ^ phrase = EditorForm::ValidatedString(anArray[0]);
      String ^ reading = anArray[1];
      array<Char> ^ common = {','};
      array<String ^> ^ readingArray = reading->Split(common);
      if (phrase->Length != readingArray->Length) {
        // alert
        return;
      }
      PhraseDBService::AddNewRow(phrase);
      int lastRow = PhraseDBService::NumberOfRow() - 1;
      PhraseDBService::SetPhraseAtRow(phrase, lastRow);
      PhraseDBService::SetNewReadingForPhraseAtRow(reading, lastRow);
      PhraseDBService::Save();
    }
  }
};
}  // namespace PhraseEditor