//
// Developed for Yahoo! Taiwan by Lithoglyph Inc.
// Copyright (c) 2007-2010 Yahoo! Taiwan.
//

#ifndef RemoteDataFetcherDelegate_h
#define RemoteDataFetcherDelegate_h

#include <string>

namespace WindowsVanilla {
using namespace std;

class RemoteDataFetcherDelegate {
 public:
  virtual void handleFetchResult(const char* data, size_t size) {}
};
};  // namespace WindowsVanilla

#endif
