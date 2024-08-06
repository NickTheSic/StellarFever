// Copyright Quarantine Games 2020


#include "CaptainTeleportButton.h"
#include "../CaptainPlayer.h"
#include "Blueprint/WidgetTree.h"
#include "CaptainSpecialScreen.h"
#include "Components/TextBlock.h"


UCaptainTeleportButton::UCaptainTeleportButton()
{
	OnClicked.AddDynamic(this, &UCaptainTeleportButton::SetPlayerToTeleport);

	FSlateBrush hovered;
	hovered.TintColor = FSlateColor(FLinearColor::Blue);
	FButtonStyle RegularStyle;
	RegularStyle.SetHovered(hovered);
	SetStyle(RegularStyle);

	SetPressMethod(EButtonPressMethod::ButtonPress);

}

void UCaptainTeleportButton::CreateTextSubWidget(UWidgetTree* widgetTree)
{
	pText = widgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	AddChild(pText);
}

void UCaptainTeleportButton::SetPlayerToTeleport()
{
	if (pCaptain != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Telling the ccaptain to teleport Directly");
		pCaptain->SetTeleportInformation(iTeleportIndex);
		pCaptain->ClassSpecial();
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "The captain was null");
}

void UCaptainTeleportButton::SetCaptain(class ACaptainPlayer* captain)
{
	if (captain != nullptr)
	{
		pCaptain = captain;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Setting the captain on the Teleport button was a null captain"));
}

void UCaptainTeleportButton::SetCaptainScreen(UCaptainScreen* screen)
{
	pScreen = screen;
}

void UCaptainTeleportButton::SetButtonText(FString string)
{
	if (pText != nullptr)
	{
		pText->SetText(FText::FromString(string));

		FSlateColor slateColor(FLinearColor::Black);
		pText->SetColorAndOpacity(slateColor);

		FSlateFontInfo slateFontInfo = pText->Font;
		slateFontInfo.Size = 12;
		pText->SetFont(slateFontInfo);
	}
}

void UCaptainTeleportButton::SetTeleportIndex(uint32 idx)
{
	iTeleportIndex = idx;
}
