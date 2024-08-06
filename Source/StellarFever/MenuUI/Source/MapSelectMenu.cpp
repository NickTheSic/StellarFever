// Copyright Quarantine Games 2020


#include "../MapSelectMenu.h"
#include "../MainMenuGameMode.h"
#include "../MainMenuSwitcher.h"
#include "../MiscMenuPieces/MapSelectButton.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"


void UMapSelectMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton != nullptr)
	{
		BackButton->OnClicked.AddDynamic(this, &UMapSelectMenu::OnClickBack);
	}

	if (AMainMenuGameMode* mmGamemode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode()))
	{
		TArray<FName> maps = mmGamemode->GetAllMaps();
		SetupButtons(maps);
	}

}

void UMapSelectMenu::SetupButtons(TArray<FName> mapNames)
{
	if (MapButtonTemplate == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MapButtonTemplate was null"));
		return;
	}

	for (FName name : mapNames)
	{
		UMapSelectButton* button = WidgetTree->ConstructWidget<UMapSelectButton>(MapButtonTemplate);
		UTextBlock* text = WidgetTree->ConstructWidget<UTextBlock>();

		FText textText = FText::FromString(name.ToString());
		text->SetText(textText);
		button->AddChild(text);

		button->Setup(name, pMainMenuSwitcher);

		if (ButtonHolder != nullptr)
			ButtonHolder->AddChild(button);
	}
}

void UMapSelectMenu::OnClickBack()
{
	if (pMainMenuSwitcher != nullptr)
	{
		pMainMenuSwitcher->SwitchToMainMenu();
	}
}
