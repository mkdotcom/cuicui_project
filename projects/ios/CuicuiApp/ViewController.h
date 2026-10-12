#import <UIKit/UIKit.h>
#import "cuicui_core/ios/cuicui_ios_host.h"

// The root view controller: it starts cuicui and gives it the system services it needs
// (pickers, camera, status bar) through the CuiHostViewController protocol.
// Its view must contain one subview constrained to the safe area (see Main.storyboard):
// cuicui draws the app inside it.
@interface ViewController : UIViewController <CuiHostViewController, UIImagePickerControllerDelegate, UINavigationControllerDelegate>
@end
