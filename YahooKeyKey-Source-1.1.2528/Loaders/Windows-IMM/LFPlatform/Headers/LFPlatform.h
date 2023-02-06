//
// LFPlatform.h
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef LFPlatform_h
#define LFPlatform_h

#if defined(__APPLE__)
#include <LFPlatform/LogKit.h>
#else
#include "LogKit.h"
#include "ProcessHelper.h"
#include "RPCHelper.h"
#include "RegistryHelper.h"
#endif

#endif
