// A minimal cuicui app: a title, an image, a text input with a button, and a scrollable list.
// Start here and replace it with your own UI.

#include "cuicui_core/cuicui.h"
using namespace cui;

static wTextInput* nameInput = nullptr;
static wText* greeting = nullptr;

static void SayHello()
{
	std::string name = nameInput->GetText();
	greeting->SetText(name.empty() ? "Type your name first :)" : "Hello, " + name + "!");
}

// The only function a cuicui app must provide: build the UI in the root widget "master".
void InitMyApp()
{
	master = new wEmpty();
	master->Xem(FILL)->Yem(FILL)->BgColor(0xffffff);

	master->Content([](Children& dc)
	{
		// wSafeZone keeps the content away from the notch and the system bars
		// (its parameters are the colors of the left/top/right/bottom system areas)
		dc += (new wSafeZone(0xffffff, 0xffffff, 0xffffff, 0xffffff))->BgColor(0xffffff)->Content([](Children& dc)
		{
			dc += wPadding::Create(FILL, FILL, ems(0.5))->Content([](Children& dc)
			{
				dc += wRows::Create(FILL, FILL)->Content([](Children& dc)
				{
					// a title
					dc += wText::Create(FILL, MIN)->SetText("Hello cuicui!", 0x000000, true)->ChangeEmRatio(1.5);
					dc += wVGap::Create(ems(0.5));

					// an image (from app/assets/)
					dc += wImage::Create(FILL, ems(5), wImage::AutoResize::Contain)->Source("cuicui.png");
					dc += wVGap::Create(ems(0.5));

					// a text input and a button, side by side
					dc += wColumns::Create(FILL, MIN)->CenterAllChildrenVertically()->Content([](Children& dc)
					{
						nameInput = wTextInput::Create(FILL)->PlaceholderText("Your name")->TextColor(0x000000);
						dc += nameInput->Yem(ems(1.6))->BgColor(0xeeeeee);
						nameInput->onEnterKeyPressed = []() { SayHello(); };
						dc += wHGap::Create(ems(0.5));
						dc += wButton::Create(MIN, ems(1.6), "Say hello", ems(0.6))->OnClick([](wButton*) { SayHello(); });
					});
					dc += wVGap::Create(ems(0.3));
					// fixed height: the text changes later, its size doesn't (otherwise call RefreshUI_Everything())
					dc += greeting = (wText*)wText::Create(FILL, ems(1.3))->SetText("");
					dc += wVGap::Create(ems(0.5));

					// a scrollable list that takes the remaining space
					dc += wScrollView::Create(FILL, FILL)->BgColor(0xf2f2f2)->Content([](Children& dc)
					{
						dc += wRows::Create(FILL, MIN)->Content([](Children& dc)
						{
							for (int i = 1; i <= 50; i++)
								dc += wText::Create(FILL, ems(1.5))->SetText("Item " + std::to_string(i));
						});
					});
				});
			});
		});
	});
}
