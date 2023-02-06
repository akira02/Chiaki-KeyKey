//
// WVEncodingService.h
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

#ifndef WVEncodingService_h
#define WVEncodingService_h

#define WIN32_LEAN_AND_MEAN
#include <OpenVanilla.h>
#include <Windows.h>

#include <set>

namespace OpenVanilla {
using namespace std;

class WVEncodingService : public OVEncodingService {
 public:
  WVEncodingService();
  virtual ~WVEncodingService();
  virtual bool codepointSupportedByEncoding(const string&, const string&);
  virtual bool codepointSupportedBySystem(const string&);
  virtual const vector<string> supportedEncodings();
  virtual bool isEncodingSupported(const string& encoding);

  virtual bool isEncodingConversionSupported(const string& fromEncoding,
                                             const string& toEncoding);
  virtual const pair<bool, string> convertEncoding(const string& fromEncoding,
                                                   const string& toEncoding,
                                                   const string& text);

 protected:
  set<UINT32> m_systemFontSupportedCharacters;
};

};  // namespace OpenVanilla

#endif