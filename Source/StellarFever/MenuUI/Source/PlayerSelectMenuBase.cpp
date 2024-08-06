// Copyright Quarantine Games 2020


#include "../PlayerSelectMenuBase.h"
#include "../PlayerSelectPieces/PlayerInformationDisplayBox.h"
#include "../PlayerSelectPieces/PlayerSelectButton.h"
#include "Components/Button.h"



void UPlayerSelectMenuBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UPlayerSelectMenuBase::SelectPlayerOnclick);
		_Buttons.Add(SelectButton);
	}

	//Give the buttons the player select menu
	if (CaptainButton)
	{
		CaptainButton->pPlayerSelectMenu = this;
		CaptainButton->pPlayerInformationBox = pInformationDisplayBox;
	}
	if (EngineerButton)
	{
		EngineerButton->pPlayerSelectMenu = this;
		EngineerButton->pPlayerInformationBox = pInformationDisplayBox;
	}
	if (GunnerButton)
	{
		GunnerButton->pPlayerSelectMenu = this;
		GunnerButton->pPlayerInformationBox = pInformationDisplayBox;
	}
	if (MedicButton)
	{
		MedicButton->pPlayerSelectMenu = this;
		MedicButton->pPlayerInformationBox = pInformationDisplayBox;
	}

	//Set up defualt style for the player buttons here 
	//TODO:  Change this oncce we get the actual style
	if (CaptainButton)
	{
		RegularStyle = CaptainButton->WidgetStyle;
		HoveredStyle = RegularStyle;
		HoveredStyle.Normal = RegularStyle.Hovered;
	}
}

void UPlayerSelectMenuBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	if (CaptainButton->HasKeyboardFocus())
	{
		CaptainButton->SetPlayerInformation();
		CaptainButton->SetStyle(HoveredStyle);
	}
	else
	{
		CaptainButton->SetStyle(RegularStyle);
	}

	if (EngineerButton->HasKeyboardFocus())
	{
		EngineerButton->SetPlayerInformation();
		EngineerButton->SetStyle(HoveredStyle);
	}
	else
	{
		EngineerButton->SetStyle(RegularStyle);
	}

	if (GunnerButton->HasKeyboardFocus())
	{
		GunnerButton->SetPlayerInformation();
		GunnerButton->SetStyle(HoveredStyle);
	}
	else
	{
		GunnerButton->SetStyle(RegularStyle);
	}

	if (MedicButton->HasKeyboardFocus())
	{
		MedicButton->SetPlayerInformation();
		MedicButton->SetStyle(HoveredStyle);
	}
	else
	{
		MedicButton->SetStyle(RegularStyle);
	}
}

void UPlayerSelectMenuBase::SetPlayerClass(const EPlayerClass& playerClass)
{
	//We have selected a player so we can click the button now
	SelectButton->SetIsEnabled(true);


	if (PreviouslyClickedPlayerButton != nullptr)
	{
		PreviouslyClickedPlayerButton->SetIsEnabled(true);
		PreviouslyClickedPlayerButton = nullptr;
	}


	switch (playerClass)
	{
	case EPlayerClass::Captain:
		PreviouslyClickedPlayerButton = CaptainButton;
		break;
	case EPlayerClass::Engineer:
		PreviouslyClickedPlayerButton = EngineerButton;
		break;
	case EPlayerClass::Gunner:
		PreviouslyClickedPlayerButton = GunnerButton;
		break;
	case EPlayerClass::Medic:
		PreviouslyClickedPlayerButton = MedicButton;
		break;
	}


	if (PreviouslyClickedPlayerButton != nullptr)
	{
		PreviouslyClickedPlayerButton->SetIsEnabled(false);
	}

}

void UPlayerSelectMenuBase::SelectPlayerOnclick()
{
	SelectPlayer();
}

void UPlayerSelectMenuBase::SelectPlayer()
{
	//This function is to be overridden
}

void UPlayerSelectMenuBase::OpenMenu()
{
	if (SelectButton)
		SelectButton->SetIsEnabled(false);

	if (PreviouslyClickedPlayerButton != nullptr)
		PreviouslyClickedPlayerButton->SetIsEnabled(true);

	if (CaptainButton)
		CaptainButton->SetFocus();
}
