/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/
// [AUTO_HEADER]

#import <Cocoa/Cocoa.h>

#import "TakaoSettings.h"

@interface TakaoSearch : NSObject {
  IBOutlet id u_shortcutKey;

  NSMutableDictionary *_searchDictionary;
  NSString *_preferenceFilePath;
}

/*!
 @method writePreference:
 @abstract Writes the settings into the plist file.
 @param sender The sender object.
 */
- (IBAction)writePreference:(id)sender;

@end
