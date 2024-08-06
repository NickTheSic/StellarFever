// Copyright Quarantine Games 2020

#include "ControllerHoverButtonWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "SFButton.h"


void UControllerHoverButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ButtonStyleTemplate)
	{
		USFButton* button = Cast<USFButton>(ButtonStyleTemplate->GetDefaultObject());
		if (button)
		{
			CommonButtonStyle = button->WidgetStyle;
			HoveredButtonStyle = CommonButtonStyle;
			HoveredButtonStyle.Normal = CommonButtonStyle.Hovered;
		}
	}

}

void UControllerHoverButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);

	//Loop through all the buttons
	//Check if Focused and set to hovered 
	//Else set to normal

	for (UButton* button : _Buttons)
	{
		if (button == nullptr) continue;

		if (button->HasKeyboardFocus())
		{
			button->SetStyle(HoveredButtonStyle);
		}
		else
		{
			button->SetStyle(CommonButtonStyle);
		}
	}
}
