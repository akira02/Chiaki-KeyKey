//
// PVPropertyList.mm
//
// Copyright (c) 2007-2010 Lukhnos D. Liu (http://lukhnos.org)
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

#include "PVPropertyList.h"

#import <Cocoa/Cocoa.h>

using namespace OpenVanilla;

PVPlistValue* PVDictionaryValueFromNSDictionary(NSDictionary* plistDict);
PVPlistValue* PVArrayValueFromNSArray(NSArray* plistArray);
NSDictionary* PVNSDictionaryFromDictionaryValue(PVPlistValue* dictValue);
NSArray* PVNSArrayFromArrayValue(PVPlistValue* arrayValue);

PVPlistValue* PVArrayValueFromNSArray(NSArray* plistArray) {
  PVPlistValue* array = new PVPlistValue(PVPlistValue::Array);
  NSEnumerator* enu = [plistArray objectEnumerator];

  id value;
  while (value = [enu nextObject]) {
    if ([value isKindOfClass:[NSString class]]) {
      PVPlistValue stringValue(string([value UTF8String]));
      array->addArrayElement(&stringValue);
    } else if ([value isKindOfClass:[NSNumber class]]) {
      PVPlistValue stringValue(string([[value stringValue] UTF8String]));
      array->addArrayElement(&stringValue);
    } else if ([value isKindOfClass:[NSArray class]]) {
      PVPlistValue* arrayValue = PVArrayValueFromNSArray(value);
      array->addArrayElement(arrayValue);
      delete arrayValue;
    } else if ([value isKindOfClass:[NSDictionary class]]) {
      PVPlistValue* dictValue = PVDictionaryValueFromNSDictionary(value);
      array->addArrayElement(dictValue);
      delete dictValue;
    }
  }

  return array;
}

PVPlistValue* PVDictionaryValueFromNSDictionary(NSDictionary* plistDict) {
  PVPlistValue* dict = new PVPlistValue(PVPlistValue::Dictionary);
  NSEnumerator* enu = [plistDict keyEnumerator];

  id key;
  while (key = [enu nextObject]) {
    string keyString = string([key UTF8String]);
    id value = [plistDict objectForKey:key];

    if ([value isKindOfClass:[NSString class]]) {
      PVPlistValue stringValue(string([value UTF8String]));
      dict->setKeyValue(keyString, &stringValue);
    } else if ([value isKindOfClass:[NSNumber class]]) {
      PVPlistValue stringValue(string([[value stringValue] UTF8String]));
      dict->setKeyValue(keyString, &stringValue);
    } else if ([value isKindOfClass:[NSArray class]]) {
      PVPlistValue* arrayValue = PVArrayValueFromNSArray(value);
      dict->setKeyValue(keyString, arrayValue);
      delete arrayValue;
    } else if ([value isKindOfClass:[NSDictionary class]]) {
      PVPlistValue* dictValue = PVDictionaryValueFromNSDictionary(value);
      dict->setKeyValue(keyString, dictValue);
      delete dictValue;
    }
  }

  return dict;
}

NSArray* PVNSArrayFromArrayValue(PVPlistValue* arrayValue) {
  NSMutableArray* nsArray = [NSMutableArray new];
  if (arrayValue->type() != PVPlistValue::Array) return nsArray;

  size_t size = arrayValue->arraySize();
  for (size_t index = 0; index < size; index++) {
    PVPlistValue* value = arrayValue->arrayElementAtIndex(index);

    switch (value->type()) {
      case PVPlistValue::String:
        [nsArray
            addObject:[NSString
                          stringWithUTF8String:value->stringValue().c_str()]];
        break;
      case PVPlistValue::Array:
        [nsArray addObject:PVNSArrayFromArrayValue(value)];
        break;
      case PVPlistValue::Dictionary:
        [nsArray addObject:PVNSDictionaryFromDictionaryValue(value)];
        break;
    }
  }

  return nsArray;
}

NSDictionary* PVNSDictionaryFromDictionaryValue(PVPlistValue* dictValue) {
  NSMutableDictionary* nsDict = [NSMutableDictionary new];

  if (dictValue->type() != PVPlistValue::Dictionary) return nsDict;

  vector<string> keys = dictValue->dictionaryKeys();
  vector<string>::iterator iter;
  for (iter = keys.begin(); iter != keys.end(); iter++) {
    PVPlistValue* value = dictValue->valueForKey(*iter);
    NSString* keyString = [NSString stringWithUTF8String:(*iter).c_str()];

    switch (value->type()) {
      case PVPlistValue::String:
        [nsDict setObject:[NSString
                              stringWithUTF8String:value->stringValue().c_str()]
                   forKey:keyString];
        break;
      case PVPlistValue::Array:
        [nsDict setObject:PVNSArrayFromArrayValue(value) forKey:keyString];
        break;
      case PVPlistValue::Dictionary:
        [nsDict setObject:PVNSDictionaryFromDictionaryValue(value)
                   forKey:keyString];
        break;
    }
  }

  return nsDict;
}

PVPlistValue* PVPropertyList::ParsePlistFromString(const char* stringData) {
  NSData* data = [NSData dataWithBytes:stringData length:strlen(stringData)];

  if (!data) return 0;

  NSString* errorString = nil;
  NSPropertyListFormat format;
  id plist =
      [NSPropertyListSerialization propertyListFromData:data
                                       mutabilityOption:NSPropertyListImmutable
                                                 format:&format
                                       errorDescription:&errorString];

  if (!plist || errorString) {
    [errorString release];
    return 0;
  }

  if (![plist isKindOfClass:[NSDictionary class]]) {
    return 0;
  }

  return PVDictionaryValueFromNSDictionary(plist);
}

PVPlistValue* PVPropertyList::ParsePlist(const string& filename) {
  NSData* data = [NSData
      dataWithContentsOfFile:[NSString stringWithUTF8String:filename.c_str()]];

  if (!data) return 0;

  NSString* errorString = nil;
  NSPropertyListFormat format;
  id plist =
      [NSPropertyListSerialization propertyListFromData:data
                                       mutabilityOption:NSPropertyListImmutable
                                                 format:&format
                                       errorDescription:&errorString];

  if (!plist || errorString) {
    [errorString release];
    return 0;
  }

  if (![plist isKindOfClass:[NSDictionary class]]) {
    return 0;
  }

  return PVDictionaryValueFromNSDictionary(plist);
}

void PVPropertyList::WritePlist(const string& filename,
                                PVPlistValue* rootDictionary) {
  if (!rootDictionary) return;

  if (rootDictionary->type() != PVPlistValue::Dictionary) return;

  NSDictionary* dict = PVNSDictionaryFromDictionaryValue(rootDictionary);
  NSString* errorString = nil;
  NSData* data = [NSPropertyListSerialization
      dataFromPropertyList:dict
                    format:NSPropertyListXMLFormat_v1_0
          errorDescription:&errorString];

  if (errorString) {
    [errorString release];
  }

  if (data) {
    [data writeToFile:[NSString stringWithUTF8String:filename.c_str()]
           atomically:YES];
  }
}

void PVPropertyList::CreateNSAutoreleasePoolInMain() {
  [NSAutoreleasePool new];
}
