//
// WVLoaderPolicy.h
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

#ifndef WVLoaderPolicy_h
#define WVLoaderPolicy_h

#include <PlainVanilla.h>

namespace OpenVanilla {
using namespace std;

class WVLoaderPolicy : public PVLoaderPolicy {
 public:
  WVLoaderPolicy(const vector<string> loadPaths) : PVLoaderPolicy(loadPaths) {}

  virtual const vector<string> modulePackageFilePatterns() {
    vector<string> result;
    // result.push_back("OV*.dll");
    // result.push_back("YK*.dll");

    // Takao-only
    result.push_back("*.ykmodule");

    return result;
  }

  virtual const OVPathInfo modulePackagePathInfoFromPath(const string& path) {
    OVPathInfo info;
    string identifier = modulePackageIdentifierFromPath(path);
    info.loadedPath = OVPathHelper::DirectoryFromPath(path);

    if (OVWildcard::Match(path, "*.ykmodule"))
      info.resourcePath = path;
    else
      info.resourcePath = info.loadedPath;

    info.writablePath = OVPathHelper::PathCat(
        OVDirectoryHelper::UserApplicationSupportDataDirectory(loaderName()),
        identifier);
    return info;
  }
};
};  // namespace OpenVanilla

#endif