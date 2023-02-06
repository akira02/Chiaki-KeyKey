//
// Developed for Yahoo! Taiwan by Lithoglyph Inc.
// Copyright (c) 2007-2010 Yahoo! Taiwan.
//

#ifndef RemoteDataFetcher_h
#define RemoteDataFetcher_h

#include <vcclr.h>

#include <map>

#include "OpenVanilla.h"
#include "RemoteDataFetcherDelegate.h"

#pragma managed(push, on)

namespace WindowsVanilla {
using namespace std;

using namespace System;
using namespace System::Globalization;
using namespace System::Threading;
using namespace System::Net;
using namespace System::Net::Cache;
using namespace System::Text;
using namespace System::IO;
using namespace OpenVanilla;

ref class RemoteDataFetcher {
 public:
  static RemoteDataFetcher ^ SharedInstance = nullptr;

  RemoteDataFetcher() {
    m_delegateMap = new map<string, RemoteDataFetcherDelegate*>();
  }

  void fetch(const string& URLString, RemoteDataFetcherDelegate* delegate) {
    m_delegateMap->operator[](URLString) = delegate;
    String ^ ustr = gcnew String(OVUTF16::FromUTF8(URLString).c_str());

    Thread ^ newThread = gcnew Thread(
        gcnew ParameterizedThreadStart(&RemoteDataFetcher::startFetch));
    array<Object ^> ^ ar = gcnew array<Object ^>(2);
    ar[0] = this;
    ar[1] = ustr;

    newThread->Start(ar);
  }

 protected:
  map<string, RemoteDataFetcherDelegate*>* m_delegateMap;

  void handleFetchResult(String ^ url, String ^ result) {
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(url);
    string cppURL = OVUTF8::FromUTF16(wchptr);
    string cppResult;

    if (result != nullptr) {
      pin_ptr<const wchar_t> wchptr = PtrToStringChars(result);
      cppResult = OVUTF8::FromUTF16(wchptr);
    }

    if (m_delegateMap->find(cppURL) == m_delegateMap->end()) {
      // exception here?
    } else {
      m_delegateMap->operator[](cppURL)->handleFetchResult(cppResult.c_str(),
                                                           cppResult.length());
    }
  }

  static void startFetch(Object ^ data) {
    String ^ result = nullptr;

    array<Object ^> ^ ar = (array<Object ^> ^) data;
    RemoteDataFetcher ^ callback = (RemoteDataFetcher ^) ar[0];
    String ^ URLString = (String ^) ar[1];

    Diagnostics::Debug::WriteLine(gcnew String(L"Starting to fetch"));

    WebRequest ^ request = nullptr;

    try {
      Encoding ^ encode = Encoding::GetEncoding(gcnew String(L"utf-8"));
      request = WebRequest::Create(URLString);
      request->CachePolicy =
          gcnew RequestCachePolicy(RequestCacheLevel::BypassCache);
      request->Timeout = 1000 * 5;
      request->ContentType = gcnew String(L"application/x-www-form-urlencoded");
      request->Method = gcnew String(L"GET");

      WebResponse ^ response = request->GetResponse();
      StreamReader ^ reader = gcnew StreamReader(response->GetResponseStream());
      result = reader->ReadToEnd();
      reader->Close();
    } catch (...) {
    }

    callback->handleFetchResult(URLString, result);
  }
};

};  // namespace WindowsVanilla

#endif
