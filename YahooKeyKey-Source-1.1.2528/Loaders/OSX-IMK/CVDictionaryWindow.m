// [AUTO_HEADER]

#import "CVDictionaryWindow.h"

@implementation CVDictionaryWindow
- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(unsigned int)aStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)flag {
  if (self = [super initWithContentRect:contentRect
                              styleMask:aStyle
                                backing:NSBackingStoreBuffered
                                  defer:NO]) {
    if (LFIsRunningUnderOSX10_5AndAbove()) {
      [self setLevel:CGShieldingWindowLevel() + 1];
    } else {
      [self setLevel:NSScreenSaverWindowLevel];
    }
    [self setHasShadow:YES];
  }

  return self;
}
- (BOOL)canBecomeKeyWindow {
  return YES;
}
- (BOOL)canBecomeMainWindow {
  return NO;
}
- (NSTimeInterval)animationResizeTime:(NSRect)newWindowFrame {
  NSTimeInterval interval = 0.05;
  return interval;
}
@end
