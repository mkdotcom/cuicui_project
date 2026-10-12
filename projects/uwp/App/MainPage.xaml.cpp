//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//


#include "MainPage.xaml.h"
#include <vector>
#include <assert.h>
#include <string> 
#include "cuicui_core/platform.h"
#include "cuicui_core/cuicui.h"
using namespace cui;
//#include "../../app/app.h"



using namespace CuicuiApp;

using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;



//bool firstResizedEvent = true;

// TODO: it works at first, then sometimes we no longer receive the event, even after restarting the app
void OnBackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args)
{
	// TODO: DUPLICATE_5668 (one in each platform code), cleaner
	if (central.hardwareBackButtonLinked.size() == 0) return; 
	wWidget *w = central.hardwareBackButtonLinked.back();	
	//central.hardwareBackButtonLinked.pop_back();  NO! it will be pop_back when the widget is destroyed TODO: check we don't make this mistake on the other platforms!
	w->TriggerClickable();		
}



bool IsMouseLeftButtonPressed(Windows::UI::Input::PointerPoint ^pp)
{
	if (pp->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
		if (pp->Properties->IsLeftButtonPressed == true)
		{
			return true;
		}
	return false;
}

#include "App.xaml.h"
Platform::Object^ GetAppStyle(const std::string &styleName)
{
	auto style = App::Current->Resources->Lookup(StdStringToUwpString(styleName));
	return style;
}


 void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^e)
{	 
	// cuicui: command lines arguments are passed as a Payload in initialPayload	
	if (e != nullptr)
	{
		auto parameters = e->Parameter->ToString();		
		UwpCommandLineArguments(parameters);
	}
}

MainPage::MainPage()
{
	InitializeComponent();
	
	cuiUiThreadId = std::this_thread::get_id();

	central.Init(); 
	this->SizeChanged += ref new SizeChangedEventHandler(this, &MainPage::MyEventHandler);

	

	SystemNavigationManager::GetForCurrentView()->BackRequested += ref new Windows::Foundation::EventHandler<Windows::UI::Core::BackRequestedEventArgs ^>(&OnBackRequested);
	
	CoreWindow ^window = CoreWindow::GetForCurrentThread();

	// we don't use this
	//Application::Current->LeavingBackground += ref new Windows::UI::Xaml::LeavingBackgroundEventHandler(this, &CuicuiApp::MainPage::OnLeavingBackground);
	//Application::Current->EnteredBackground += ref new Windows::UI::Xaml::EnteredBackgroundEventHandler(this, &CuicuiApp::MainPage::OnEnteredBackground);

	window->Activated += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowActivatedEventArgs ^>(this, &CuicuiApp::MainPage::OnActivated);

	// we don't use it
	//Application::Current->Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(this, &CuicuiApp::MainPage::OnSuspending);
	
	// we don't use it
	//window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::VisibilityChangedEventArgs ^>(this, &CuicuiApp::MainPage::OnVisibilityChanged);

	// note: this doesn't work? I never receive the event, not sure what it's really for
	//window->TouchHitTesting += ref new TypedEventHandler<CoreWindow^, TouchHitTestingEventArgs^>(this, &MainPage::OnTouchHitTesting);		
		//ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::TouchHitTestingEventArgs ^>(this, &CuicuiApp::MainPage::OnTouchHitTesting);

	// another idea/article to be able to override "hittest" maybe: (but probably not)
	// https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.uielement.findsubelementsfortouchtargeting
	// https://stackoverflow.com/questions/52517310/uwp-is-it-possible-to-override-hit-testing

#ifdef USE_MIEM
	window->PointerMoved +=  ref new Windows::Foundation::TypedEventHandler<CoreWindow^, PointerEventArgs ^>([this](CoreWindow ^sender, PointerEventArgs ^args)
	{		
		Windows::UI::Input::PointerPoint ^pp = args->CurrentPoint;// GetCurrentPoint(nullptr);

		wWidget *w = MIEM::FindTouchedView(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		
		args->Handled = true;  // swallow

		if (w!=nullptr && w->isNativeControlThatNeedsTouchEvents == true) //w->coreType == eCoreType::ScrollView) // OR nativebutton or other native widget that needs to receives inputs
		{
			args->Handled = false; // letgo						
		}

		uint32_t pointerId = pp->PointerId;
		MiemBeforeMoveds();
		MIEM::eWhatToDo wtd/*dontcare*/ = miem[pointerId].Moved(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));		
		MiemAfterMoveds();
		
		return;
	}); 

	window->PointerPressed += ref new Windows::Foundation::TypedEventHandler<CoreWindow^, PointerEventArgs ^>([this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		Windows::UI::Input::PointerPoint ^pp = args->CurrentPoint;// GetCurrentPoint(nullptr);

		wWidget *w = MIEM::FindTouchedView(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		
		args->Handled = true;  // swallow

		if (w != nullptr && w->isNativeControlThatNeedsTouchEvents == true) //&& w->coreType == eCoreType::ScrollView) // OR nativebutton or other native widget that needs to receives inputs
		{
			args->Handled = false; // letgo			
		}

		uint32_t pointerId = pp->PointerId;		
		MIEM::eWhatToDo wtd/*dontcare*/ = miem[pointerId].Pressed(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));		

		
		return;
	});

	window->PointerCaptureLost += ref new Windows::Foundation::TypedEventHandler<CoreWindow^, PointerEventArgs ^>([this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		// this occurs when a nativecontrolwithtouch capture the pointer (window->PointerReleased will not be called)
		Windows::UI::Input::PointerPoint ^pp = args->CurrentPoint;
		uint32_t pointerId = pp->PointerId;
		miem[pointerId].dragging = false;
		miemManager.draggingWidget = nullptr;
		Log("miemManager.draggingWidget set to NULL");
	});

	window->PointerReleased += ref new Windows::Foundation::TypedEventHandler<CoreWindow^, PointerEventArgs ^>([this](CoreWindow ^sender, PointerEventArgs ^args)
	{	
		// WARNING: we usually receive this event BUT when a scrollview is scrolling, we do NOT receive it
		//           -> most probably because the native scrollview captured the pointer, so it receives the release
		// probably a big problem? unless an "isNativeControlThatNeedsTouchEvents" doesn't capture in onpress, maybe?

		Windows::UI::Input::PointerPoint ^pp = args->CurrentPoint;// GetCurrentPoint(nullptr);
		 
		wWidget *w = MIEM::FindTouchedView(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		
		args->Handled = true;  // swallow

		if (w != nullptr && w->isNativeControlThatNeedsTouchEvents == true) //&& w->coreType == eCoreType::ScrollView) // OR nativebutton or other native widget that needs to receives inputs
		{
			args->Handled = false; // letgo			
		}

		uint32_t pointerId = pp->PointerId;
		MIEM::eWhatToDo wtd/*dontcare*/ = miem[pointerId].Released(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		
		return;
	});

	/*
	touchGrid->PointerPressed += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
		return;

		Windows::UI::Input::PointerPoint ^pp = args->GetCurrentPoint(nullptr);
		uint32_t pointerId = pp->PointerId;

		if (!IsMouseLeftButtonPressed(pp)) { args->Handled = true; return; } // mouse left button only
		
		MIEM::eWhatToDo wtd = miem[pointerId].Pressed(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		if (wtd==MIEM::eWhatToDo::Swallow)
			args->Handled = true;
		else
			args->Handled = false;		
	});
	
	touchGrid->PointerReleased += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
		return;

		Windows::UI::Input::PointerPoint ^pp = args->GetCurrentPoint(nullptr);				
		uint32_t pointerId = pp->PointerId;

		MIEM::eWhatToDo wtd = miem[pointerId].Released(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		if (wtd == MIEM::eWhatToDo::Swallow)
			args->Handled = true;
		else
			args->Handled = false;
	});
	
	touchGrid->PointerCaptureLost += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
	});

	touchGrid->PointerCanceled += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
	});

	touchGrid->PointerEntered += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
	});

	touchGrid->PointerExited += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
	});

	touchGrid->PointerMoved += ref new PointerEventHandler([this](Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^args)
	{
		args->Handled = false; // letgo
		return;

		Windows::UI::Input::PointerPoint ^pp = args->GetCurrentPoint(nullptr);

		wWidget *w = MIEM::FindTouchedView(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		if (w == nullptr)
		{
			args->Handled = true;  // swallow
			return;
		}

		// note:
	  //  thanks to this hittest, only the widget that must receive the event will receive it
	   // and the native scrollview (or other native control) will keep receiving it too


		if (w->coreType == eCoreType::ScrollView)
		{
			args->Handled = false; // letgo
			return;
		}

		// todo

		args->Handled = true;  // swallow
		return;

		//Windows::UI::Input::PointerPoint ^pp = args->GetCurrentPoint(nullptr);
		//uint32_t pointerId = pp->PointerId;
		
		//MiemBeforeMoveds();
		//MIEM::eWhatToDo wtd = miem[pointerId].Moved(pixels::CreateFromDouble(pp->Position.X), pixels::CreateFromDouble(pp->Position.Y));
		//if (wtd == MIEM::eWhatToDo::Swallow)
		//	args->Handled = true;
		//else
	//		args->Handled = false;		
	//	MiemAfterMoveds();
	});*/
#endif
	

	// this event doesn't seem to work, see below
	//Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &MainPage::WindowSizeChanged);
}
 
//extern void UpdateCuiLayout(int w, int h);

/*
void MainPage::WindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e)
{	
	// note: according to a Microsoft code sample, this event should be called when UserInteractionMode switches from Mouse to Tablet/Touch (and vice versa)
	//       it doesn't work with the simulator, I don't know if it would work on a real device
	Windows::UI::ViewManagement::UserInteractionMode im = Windows::UI::ViewManagement::UIViewSettings::GetForCurrentView()->UserInteractionMode;
	CUICUI_NOP
}*/


void MainPage::MyEventHandler(Platform::Object^ sender, SizeChangedEventArgs ^e)
{	
	
	OutputDebugStringA("----- windows resize event\n");
	//if (e->NewSize == e->PreviousSize) return; // TODO: never happens I guess? check
	
	float w = e->NewSize.Width;
	float h = e->NewSize.Height;
	
	//touchGrid->Width = w;
	//touchGrid->Height = h;

	rootGrid->Width = w;
	rootGrid->Height = h;

	OnWindowSizeChanged((int)w, (int)h, rootGrid);
}


/*void CuicuiApp::MainPage::OnEnteredBackground(Platform::Object ^sender, Windows::ApplicationModel::EnteredBackgroundEventArgs ^e)
{
	return;
	//if (central.onAppStateChanged != nullptr)
	//	central.onAppStateChanged(cCentral::eAppState::Background);
}*/


/*void CuicuiApp::MainPage::OnLeavingBackground(Platform::Object ^sender, Windows::ApplicationModel::LeavingBackgroundEventArgs ^e)
{
	return;
	// note: also occurs when the app starts
	//if (central.onAppStateChanged != nullptr)
	//	central.onAppStateChanged(cCentral::eAppState::Foreground);
}*/



void CuicuiApp::MainPage::OnTouchHitTesting(Windows::UI::Core::CoreWindow^ /*sender*/,
	Windows::UI::Core::TouchHitTestingEventArgs^ touchHitTestingArgs)
{
	CUICUI_NOP
}



/*void CuicuiApp::MainPage::OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
{
	CUICUI_NOP
}*/


/*void CuicuiApp::MainPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args)
{
	CUICUI_NOP
}*/


void CuicuiApp::MainPage::OnActivated(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowActivatedEventArgs ^args)
{
	if (args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::Deactivated)
	{
		Log("UWP debug : onAppStateChanged : Background");
		if (central.onAppStateChanged != nullptr)
			central.onAppStateChanged(cCentral::eAppState::Background);
	}

	if (args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::PointerActivated ||
		args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::CodeActivated)
	{
		// note: also occurs when the app starts
		Log("UWP debug : onAppStateChanged : Foreground");
		if (central.onAppStateChanged != nullptr)
			central.onAppStateChanged(cCentral::eAppState::Foreground);
	}
		
}
