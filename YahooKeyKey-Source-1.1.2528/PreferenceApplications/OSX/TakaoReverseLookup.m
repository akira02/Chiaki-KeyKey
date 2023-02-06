/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/
// [AUTO_HEADER]

#import "TakaoReverseLookup.h"

#import "TakaoHelper.h"

@implementation TakaoReverseLookup

- (void)dealloc {
  [_reverseLookupArray release];
  [_preferenceFilePath release];
  [super dealloc];
}

- (id)init {
  if (self = [super init]) {
    NSLog(@"init");
    BOOL loaded = [NSBundle loadNibNamed:@"TakaoReverseLookup" owner:self];
    NSAssert((loaded == YES), @"NIB did not load");
    NSLog(@"init2");
  }
  return self;
}

- (void)awakeFromNib {
  LFRetainAssign(_preferenceFilePath,
                 [TakaoHelper plistFilePath:PLIST_GLOBAL_FILENAME]);
  _reverseLookupArray = [[NSArray alloc]
      initWithObjects:@"ReverseLookup-Generic-cj-cin",
                      @"ReverseLookup-Mandarin-bpmf-cin",
                      @"ReverseLookup-Mandarin-bpmf-cin-HanyuPinyin", nil];

  NSData *data = [NSData dataWithContentsOfFile:_preferenceFilePath
                                        options:0
                                          error:nil];
  if (data) {
    NSPropertyListFormat format;
    NSString *errorString = nil;
    NSMutableDictionary *d = [NSPropertyListSerialization
        propertyListFromData:data
            mutabilityOption:NSPropertyListImmutable
                      format:&format
            errorDescription:&errorString];
    if (d) {
      NSArray *activatedAroundFilters =
          [d valueForKey:@"ActivatedAroundFilters"];
      NSEnumerator *enumerator = [activatedAroundFilters objectEnumerator];
      NSString *aroundFilter = nil;
      while (aroundFilter = [enumerator nextObject]) {
        if ([_reverseLookupArray containsObject:aroundFilter]) {
          if ([aroundFilter isEqualToString:@"ReverseLookup-Generic-cj-cin"]) {
            [_popUpButton selectItemAtIndex:1];
          } else if ([aroundFilter
                         isEqualToString:@"ReverseLookup-Mandarin-bpmf-cin"]) {
            [_popUpButton selectItemAtIndex:2];
          } else if ([aroundFilter
                         isEqualToString:
                             @"ReverseLookup-Mandarin-bpmf-cin-HanyuPinyin"]) {
            [_popUpButton selectItemAtIndex:3];
          }
          return;
        }
      }
    }
  }
  [_popUpButton selectItemAtIndex:0];
}

- (NSView *)view {
  return _view;
}
- (IBAction)changeReverseLookupSetting:(id)sender {
  int selected = [sender indexOfSelectedItem];
  NSString *reverseLookupModuleName = @"";
  if (selected > 0 && selected < 4) {
    reverseLookupModuleName = [_reverseLookupArray objectAtIndex:selected - 1];
  }
  NSMutableDictionary *newDictionary = [NSMutableDictionary dictionary];
  NSData *data = [NSData dataWithContentsOfFile:_preferenceFilePath
                                        options:0
                                          error:nil];
  if (data) {
    NSPropertyListFormat format;
    NSString *errorString = nil;
    NSMutableDictionary *d = [NSPropertyListSerialization
        propertyListFromData:data
            mutabilityOption:NSPropertyListImmutable
                      format:&format
            errorDescription:&errorString];
    if (d) {
      NSMutableArray *newArray = [NSMutableArray array];
      NSArray *activatedAroundFilters =
          [d valueForKey:@"ActivatedAroundFilters"];
      NSEnumerator *enumerator = [activatedAroundFilters objectEnumerator];
      NSString *aroundFilter = nil;
      while (aroundFilter = [enumerator nextObject]) {
        if (![aroundFilter hasPrefix:@"ReverseLookup-"]) {
          [newArray addObject:aroundFilter];
        }
      }
      if ([reverseLookupModuleName length]) {
        [newArray addObject:reverseLookupModuleName];
      }
      [d setValue:newArray forKey:@"ActivatedAroundFilters"];
    }
    [newDictionary addEntriesFromDictionary:d];
  } else {
    if ([reverseLookupModuleName length]) {
      [newDictionary setValue:[NSArray arrayWithObject:reverseLookupModuleName]
                       forKey:@"ActivatedAroundFilters"];
    }
  }
  NSString *errorString = nil;
  data = [NSPropertyListSerialization
      dataFromPropertyList:newDictionary
                    format:NSPropertyListXMLFormat_v1_0
          errorDescription:&errorString];

  if (errorString) {
    [errorString release];
  }

  if (data) {
    [data writeToFile:_preferenceFilePath atomically:YES];
  }
}

@end
