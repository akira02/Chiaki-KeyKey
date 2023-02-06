/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/
// [AUTO_HEADER]

#import "TakaoSearch.h"

#import "TakaoHelper.h"

@implementation TakaoSearch

- (void)dealloc {
  [_preferenceFilePath release];
  [super dealloc];
}

- (void)setUI {
  NSString *shortcutKey = [_searchDictionary valueForKey:@"ShortcutKey"];

  if ([shortcutKey isEqualToString:@"~"]) {
    [u_shortcutKey selectItemAtIndex:1];
  } else {
    [u_shortcutKey selectItemAtIndex:0];
  }
}

- (void)awakeFromNib {
  _searchDictionary = [[NSMutableDictionary alloc] init];
  [_searchDictionary setValue:@"`" forKey:@"ShortcutKey"];
  LFRetainAssign(_preferenceFilePath,
                 [TakaoHelper plistFilePath:PLIST_SEARCH_FILENAME]);

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
      [_searchDictionary addEntriesFromDictionary:d];
    }
  }
  [self setUI];
  [self writePreference:self];
}

- (void)updateDictionary {
  int selection = [u_shortcutKey indexOfSelectedItem];
  switch (selection) {
    case 0:
      [_searchDictionary setValue:@"`" forKey:@"ShortcutKey"];
      break;
    case 1:
      [_searchDictionary setValue:@"~" forKey:@"ShortcutKey"];
      break;
    default:
      [_searchDictionary setValue:@"`" forKey:@"ShortcutKey"];
      break;
  }
}

- (IBAction)writePreference:(id)sender {
  NSString *errorString = nil;
  [self updateDictionary];
  NSData *data = [NSPropertyListSerialization
      dataFromPropertyList:_searchDictionary
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
