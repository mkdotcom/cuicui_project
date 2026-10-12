#import "ViewController.h"

#include "cuicui_core/cuicui.h"
using namespace cui;

@implementation ViewController
{
	UIStatusBarStyle currentStatusBarStyle;
}

- (void)viewDidLoad
{
	[super viewDidLoad];
	currentStatusBarStyle = UIStatusBarStyleDefault;

	cuiUiThreadId = std::this_thread::get_id();

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardDidShow:) name:UIKeyboardDidShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardDidHide:) name:UIKeyboardDidHideNotification object:nil];

	central.Init(); // calls InitMyApp() (app/main.cpp) once the size of the app is known
}

// The view cuicui draws in: the subview constrained to the safe area.
- (UIView*)cuiContainer
{
	return self.view.subviews[0];
}

- (void)viewDidLayoutSubviews
{
	[super viewDidLayoutSubviews];
	UIView* container = [self cuiContainer];
	OnWindowSizeChanged((int)container.bounds.size.width, (int)container.bounds.size.height, container);
}

// --- keyboard: the app is resized so that the keyboard doesn't hide it

- (void)keyboardResized:(CGRect)keyboardFrame showing:(bool)showing
{
	UIView* container = [self cuiContainer];
	CGFloat height = container.bounds.size.height;
	if (showing)
	{
		CGRect containerInWindow = [container convertRect:container.bounds toView:nil]; // window coordinates
		CGFloat windowHeight = self.view.window.bounds.size.height;
		CGFloat keyboardTop = windowHeight - keyboardFrame.size.height;
		height = keyboardTop - containerInWindow.origin.y;
	}
	OnWindowSizeChanged((int)container.bounds.size.width, (int)height, container);
}

- (void)keyboardDidShow:(NSNotification*)notification
{
	CGRect keyboardFrame = [notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
	[self keyboardResized:keyboardFrame showing:true];
}

- (void)keyboardDidHide:(NSNotification*)notification
{
	CGRect keyboardFrame = [notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
	[self keyboardResized:keyboardFrame showing:false];
}

// --- status bar (CuiHostViewController)

- (void)setCurrentStatusBarStyle:(UIStatusBarStyle)style
{
	currentStatusBarStyle = style;
	[self setNeedsStatusBarAppearanceUpdate];
}

- (UIStatusBarStyle)preferredStatusBarStyle
{
	return currentStatusBarStyle;
}

// --- pickers and camera (CuiHostViewController)

- (void)presentImagePicker:(UIImagePickerControllerSourceType)sourceType
{
	if (![UIImagePickerController isSourceTypeAvailable:sourceType])
	{
		LogWarning("this image source is not available on this device"); // e.g. the camera on the simulator
		return;
	}
	UIImagePickerController* picker = [[UIImagePickerController alloc] init];
	picker.delegate = self;
	picker.allowsEditing = NO;
	picker.sourceType = sourceType;
	[self presentViewController:picker animated:YES completion:nil];
}

- (void)openPickerImages
{
	[self presentImagePicker:UIImagePickerControllerSourceTypePhotoLibrary];
}

- (void)openCamera
{
	[self presentImagePicker:UIImagePickerControllerSourceTypeCamera];
}

- (void)openPickerDocuments
{
	central.MessageBox("Picking documents is not available on iOS yet.");
}

- (void)imagePickerController:(UIImagePickerController*)picker didFinishPickingMediaWithInfo:(NSDictionary*)info
{
	[picker dismissViewControllerAnimated:YES completion:nil];
	if (onFileReceived == nullptr) return;

	// the picked image is always sent as a jpeg
	UIImage* image = info[UIImagePickerControllerOriginalImage];
	NSData* data = UIImageJPEGRepresentation(image, 0.85);
	if (data == nil) return;

	auto file = std::make_shared<cFileReceived>();
	file->base64Data = NSStringToStdString([data base64EncodedStringWithOptions:0]);
	file->fileName = NSStringToStdString([[NSUUID UUID] UUIDString]) + ".jpg";
	file->mimeType = "image/jpeg";
	onFileReceived(file);
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController*)picker
{
	[picker dismissViewControllerAnimated:YES completion:nil];
}

@end
