/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/

#ifndef YKAFOneKeyPackage_h
#define YKAFOneKeyPackage_h

#if defined(__APPLE__)
#include <OpenVanilla/OpenVanilla.h>
#else
#include "OpenVanilla.h"
#endif

#include "YKAFOneKey.h"

namespace OpenVanilla {
using namespace std;

class YKAFOneKeyPackage : public OVModulePackage {
 public:
  YKAFOneKeyPackage() {
    m_moduleVector.push_back(new OVModuleClassWrapper<YKAFOneKey>);
  }

  virtual bool initialize(OVPathInfo*, OVLoaderService* loaderService) {
    return true;
  }
};

};  // namespace OpenVanilla

#endif
