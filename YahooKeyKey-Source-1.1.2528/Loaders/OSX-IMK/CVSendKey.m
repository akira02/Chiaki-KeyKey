// [AUTO_HEADER]

#import "CVSendKey.h"

Ascii2KeyCodeTable keytable;

bool CVSKIsUpper(char c) {
  if (isupper(c)) return true;

  if (strchr("~!@#$%^&*()_+{}|:\"<>?", c)) return true;

  return false;
}

@implementation CVSendKey

static CVSendKey *_sharedSendKey = nil;

+ (CVSendKey *)sharedSendKey {
  if (_sharedSendKey == nil) _sharedSendKey = [[CVSendKey alloc] init];
  return _sharedSendKey;
}

+ (OSStatus)InitAscii2KeyCodeTable {
  unsigned char *theCurrentKCHR, *ithKeyTable;
  short count, i, j, resID;
  Handle theKCHRRsrc;
  ResType rType;
  /* set up our table to all minus ones */
  for (i = 0; i < 256; i++) keytable.transtable[i] = -1;
    /* find the current kchr resource ID */

#ifdef __x86_64__
  // always set to 0
  keytable.kchrID = 0;
#else
  keytable.kchrID = (short)GetScriptVariable(smCurrentScript, smScriptKeys);
#endif
  /* get the current KCHR resource */
  theKCHRRsrc = GetResource('KCHR', keytable.kchrID);
  if (theKCHRRsrc == NULL) return resNotFound;
  GetResInfo(theKCHRRsrc, &resID, &rType, keytable.KCHRname);
  /* dereference the resource */
  theCurrentKCHR = (unsigned char *)(*theKCHRRsrc);
  /* get the count from the resource */
  count = *(short *)(theCurrentKCHR + kTableCountOffset);
  /* build inverse table by merging all key tables */
  for (i = 0; i < count; i++) {
    ithKeyTable = theCurrentKCHR + kFirstTableOffset + (i * kTableSize);
    for (j = 0; j < kTableSize; j++) {
      if (keytable.transtable[ithKeyTable[j]] == -1)
        keytable.transtable[ithKeyTable[j]] = j;
    }
  }

  return noErr;
}

- (short)AsciiToKeyCode:(short)asciiCode {
  if (asciiCode >= 0 && asciiCode <= 255)
    return keytable.transtable[asciiCode];
  else
    return -1;
}

- (void)_typeString:(NSString *)string {
  const char *s = [string UTF8String];
  int i;
  BOOL upper;
  for (i = 0; i < strlen(s); i++) {
    CGKeyCode code = [self AsciiToKeyCode:s[i]];

    upper = CVSKIsUpper(s[i]);

    if (upper)
      CGPostKeyboardEvent((CGCharCode)0, (CGKeyCode)56, true);     // shift down
    CGPostKeyboardEvent((CGCharCode)s[i], (CGKeyCode)code, true);  // 'z' down
    CGPostKeyboardEvent((CGCharCode)s[i], (CGKeyCode)code, false);  // 'z' up
    if (upper)
      CGPostKeyboardEvent((CGCharCode)0, (CGKeyCode)56, false);  // 'shift up
  }
}

- (void)typeString:(NSString *)string {
  [self performSelector:@selector(_typeString:)
             withObject:string
             afterDelay:0.1];
}

+ (void)initialize {
  [self InitAscii2KeyCodeTable];
}

@end
