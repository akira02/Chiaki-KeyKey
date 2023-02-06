// [AUTO_HEADER]

#import "TrackerSender.h"

#if (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5)
typedef unsigned int NSUInteger;
typedef int NSInteger;
#endif

#import "LFHTTPRequest.h"

TrackerSender *SharedTrackerSender = nil;

@implementation TrackerSender
+ (TrackerSender *)sharedTrackerSender {
  return SharedTrackerSender
             ? SharedTrackerSender
             : (SharedTrackerSender = [[TrackerSender alloc] init]);
}
- (void)sendTrackerWithURLString:(NSString *)urlString {
  LFHTTPRequest *request = [[LFHTTPRequest alloc] init];
  [request setDelegate:self];

  // Must use IE6
  [request setUserAgent:
               @"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; .NET "
               @"CLR 1.1.4322; .NET CLR 2.0.50727)"];
  BOOL result = [request performMethod:@"GET"
                                 onURL:[NSURL URLWithString:urlString]
                              withData:nil];
}
- (void)httpRequest:(LFHTTPRequest *)request
    didReceiveStatusCode:(NSUInteger)statusCode
                     URL:(NSURL *)url
          responseHeader:(CFHTTPMessageRef)header {
}
- (void)httpRequestDidComplete:(LFHTTPRequest *)request {
  [request autorelease];
}
- (void)httpRequest:(LFHTTPRequest *)request
    didFailWithError:(NSString *)error {
  [request autorelease];
}
@end
