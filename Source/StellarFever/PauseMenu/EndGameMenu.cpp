// Copyright Quarantine Games 2020

#include "EndGameMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../Player/SF_PlayerController.h"
#include "Kismet/GameplayStatics.h"


void UEndGameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PlayAgainButton)
	{
		PlayAgainButton->OnClicked.AddDynamic(this, &UEndGameMenu::OnClickPlayButton);
		_Buttons.Add(PlayAgainButton);
	}

	if (ReturnToMenuButton)
	{
		ReturnToMenuButton->OnClicked.AddDynamic(this, &UEndGameMenu::OnClickReturnToMenu);
		_Buttons.Add(PlayAgainButton);
	}
}

void UEndGameMenu::OnClickPlayButton()
{
	if (ASF_PlayerController* controller = GetOwningPlayer<ASF_PlayerController>())
	{
		controller->PlayerPlayAgain();
	}

	if (PlayAgainButton != nullptr)
	{
		PlayAgainButton->SetIsEnabled(false);
	}
}

void UEndGameMenu::OnClickReturnToMenu()
{
	if (ASF_PlayerController* controller = GetOwningPlayer<ASF_PlayerController>())
	{
		controller->PlayerReturnToMenu();
	}

	UGameplayStatics::OpenLevel(GetOwningPlayer(), FName("MainMenuMap"));
}

void UEndGameMenu::SetReasonForEndingGame(const EEndGameReason& Reason)
{
	if (Reason > EEndGameReason::GameWon)
	{
		EndGameText->SetText(FText::FromString(GameOverTitleText));
	}
	else
	{
		EndGameText->SetText(FText::FromString(GameWonTitleText));
	}
	
	SetReasonText(Reason);
}


void UEndGameMenu::SetReasonText(const EEndGameReason& EReason)
{
	FString reasonTextValue= FString();

	switch (EReason)
	{
	case EEndGameReason::GameWon :
		reasonTextValue = WinGameText;
		break;
	
	case EEndGameReason::AllPlayersDied :
		reasonTextValue = PlayersDeadText;
		break;

	case EEndGameReason::EnemiesStoleVaccines :
		reasonTextValue = VaccinesLostText;
		break;

	default:
		reasonTextValue = DefaultGameOverText;
		break;
	}

	ReasonText->SetText(FText::FromString(reasonTextValue));
}
