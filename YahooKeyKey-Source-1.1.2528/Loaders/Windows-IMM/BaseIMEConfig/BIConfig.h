//
// BIConfig.h
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

#ifndef BIConfig_h
#define BIConfig_h

// override (define) these five following macros to customize the BaseIME system

#ifndef BASEIME_INSTALLER_VERSION
#define BASEIME_INSTALLER_VERSION "1.0.0"
#endif

#ifndef BASEIME_LOADER_NAME
#define BASEIME_LOADER_NAME "BaseIME"
#endif

#ifndef BASEIME_COMPANY_NAME
#define BASEIME_COMPANY_NAME "Lithoglyph"
#endif

#ifndef BASEIME_PRODUCT_NAME
#define BASEIME_PRODUCT_NAME "BaseIME"
#endif

#ifndef BASEIME_RESOURCE_DESCRIPTION
#define BASEIME_RESOURCE_DESCRIPTION "Lithoglyph BaseIME"
#endif

#ifndef BASEIME_RESOURCE_COPYRIGHT
#define BASEIME_RESOURCE_COPYRIGHT "Copyright (c) 2007-2008 Lithoglyph Inc."
#endif

#ifndef BASEIME_RESOURCE_ICON
#define BASEIME_RESOURCE_ICON "BaseIME.ico"
#endif

// change the following stuff only if you want to

#ifndef BASEIME_EXECUTABLE
#define BASEIME_EXECUTABLE BASEIME_PRODUCT_NAME ".ime"
#endif

#ifndef BASEIME_RESOURCE_VERSION
#define BASEIME_RESOURCE_VERSION "1, 0, 0, 0"
#endif

#ifndef BASEIME_RESOURCE_VERSION_MAJOR
#define BASEIME_RESOURCE_VERSION_MAJOR 1
#endif

#ifndef BASEIME_RESOURCE_VERSION_MINOR
#define BASEIME_RESOURCE_VERSION_MINOR 0
#endif

#ifndef BASEIME_RESOURCE_VERSION_TINY
#define BASEIME_RESOURCE_VERSION_TINY 0
#endif

#ifndef BASEIME_RESOURCE_BLOCK_CODE
#define BASEIME_RESOURCE_BLOCK_CODE "040404b0"
#endif

#ifndef BASEIME_USER_APPLICATION_DATA_DIR
#define BASEIME_USER_APPLICATION_DATA_DIR \
  BASEIME_COMPANY_NAME " " BASEIME_PRODUCT_NAME
#endif

#ifndef BASEIME_RESOURCE_CODEPAGE
#define BASEIME_RESOURCE_CODEPAGE 950
#endif

#ifndef BASEIME_RESOURCE_TRANSLATION_PARAM1
#define BASEIME_RESOURCE_TRANSLATION_PARAM1 0x404  // Traditional Chinese
#endif

#ifndef BASEIME_RESOURCE_TRANSLATION_PARAM2
#define BASEIME_RESOURCE_TRANSLATION_PARAM2 1200  // Traditional Chinese
#endif

// the following macros are derived from the above three

#ifndef BASEIME_SERVER_REGKEY
#define BASEIME_SERVER_REGKEY \
  "SOFTWARE\\" BASEIME_COMPANY_NAME "\\" BASEIME_PRODUCT_NAME
#endif

#ifndef BASEIME_SERVER_LOCATION_VALUE_NAME
#define BASEIME_SERVER_LOCATION_VALUE_NAME "ServerLocation"
#endif

#ifndef BASEIME_WINDOW_CLASS
#define BASEIME_WINDOW_CLASS BASEIME_PRODUCT_NAME "UI"
#endif

#ifndef BASEIME_SERVER_IDENTIFIER
#define BASEIME_SERVER_IDENTIFIER "BIS" BASEIME_PRODUCT_NAME
#endif

#ifndef BASEIME_CLIENT_IDENTIFIER_PREFIX
#define BASEIME_CLIENT_IDENTIFIER_PREFIX "BIC" BASEIME_PRODUCT_NAME "-"
#endif

#ifndef BASEIME_SERVER_EXECUTABLE
#define BASEIME_SERVER_EXECUTABLE BASEIME_PRODUCT_NAME "Server.exe"
#endif

#ifndef BASEIME_SERVER_DEFAULT_LOCATION
#define BASEIME_SERVER_DEFAULT_LOCATION \
  "C:\\Program Files\\" BASEIME_COMPANY_NAME "\\" BASEIME_PRODUCT_NAME "\\"
#endif

#ifndef BASEIME_SERVER_STARTUP_TIMEOUT
#define BASEIME_SERVER_STARTUP_TIMEOUT 45
#endif

#ifndef BASEIME_LOG_FILENAME
#define BASEIME_LOG_FILENAME BASEIME_PRODUCT_NAME ".log"
#endif

#ifndef BASEIME_SERVER_LOG_FILENAME
#define BASEIME_SERVER_LOG_FILENAME BASEIME_PRODUCT_NAME "Server.log"
#endif

#ifndef BASEIME_FAKEKEY_CODE
#define BASEIME_FAKEKEY_CODE 0xffff
#endif

#ifndef BASEIME_FAKEKEY_MODIFIER
#define BASEIME_FAKEKEY_MODIFIER 0xffff
#endif

#endif
