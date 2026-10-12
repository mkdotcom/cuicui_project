//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"



namespace CuicuiApp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>	

	public ref class MainPage sealed
	{
	public:
		//property Windows::UI::Xaml::Controls::Canvas ^masterCanvas;
		//property  Windows::UI::Xaml::Controls::TextBlock ^test;
		//property void* master;

	public:
		MainPage();
		void MyEventHandler(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^e); // this one is used to know the size of "master"
		//void WindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e); // this one is for detecting mouse<->touch change	

		//void OnEnteredBackground(Platform::Object ^sender, Windows::ApplicationModel::EnteredBackgroundEventArgs ^e);
		//void OnLeavingBackground(Platform::Object ^sender, Windows::ApplicationModel::LeavingBackgroundEventArgs ^e);
		

		void OnTouchHitTesting(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::TouchHitTestingEventArgs^ touchHitTestingArgs);
		//void OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e);
		//void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args);
		void OnActivated(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowActivatedEventArgs ^args);

	protected:
		virtual void  OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^e) override;
	};
}
