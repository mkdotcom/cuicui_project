#import "AppDelegate.h"

#include "cuicui_core/cuicui.h"
using namespace cui;

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
	// The UI is created by ViewController (see Main.storyboard), which starts cuicui.
	return YES;
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
	if (central.onAppStateChanged != nullptr)
		central.onAppStateChanged(cCentral::eAppState::Background);
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
	if (central.onAppStateChanged != nullptr)
		central.onAppStateChanged(cCentral::eAppState::Foreground);
}

@end
