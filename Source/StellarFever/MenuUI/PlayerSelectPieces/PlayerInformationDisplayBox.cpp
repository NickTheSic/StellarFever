// Copyright Quarantine Games 2020


#include "PlayerInformationDisplayBox.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UPlayerInformationDisplayBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	if (FlavourText)
	{
		FlavourText->SetAutoWrapText(true);
		FlavourText->SetText(FText::FromString(""));
	}

	if (CharacterNameText)
	{
		CharacterNameText->SetAutoWrapText(true);
		CharacterNameText->SetText(FText::FromString(""));
	}

	if (ClassNameText)
	{
		ClassNameText->SetAutoWrapText(true);
		ClassNameText->SetText(FText::FromString(""));
	}

	if (WeaponInformationText)
	{
		WeaponInformationText->SetAutoWrapText(true);
		WeaponInformationText->SetText(FText::FromString(""));
	}

	if (SpecialInformationText)
	{
		SpecialInformationText->SetAutoWrapText(true);
		SpecialInformationText->SetText(FText::FromString(""));
	}
}

void UPlayerInformationDisplayBox::SetPlayerClass(const EPlayerClass& playerClass)
{
	//Set the class name text
	if (ClassNameText->IsValidLowLevel())
	{
		FText classText;

		switch (playerClass)
		{
		case EPlayerClass::Captain:
			classText = FText::FromString("Captain");
			break;
		case EPlayerClass::Engineer:
			classText = FText::FromString("Engineer");
			break;
		case EPlayerClass::Gunner:
			classText = FText::FromString("Gunner");
			break;
		case EPlayerClass::Medic:
			classText = FText::FromString("Medic");
			break;
		default:
			classText = FText::FromString("Not a Player");
			break;
		}

		ClassNameText->SetText(classText);
	}
}

void UPlayerInformationDisplayBox::SetPlayerName(const FText& playerName)
{
	if (CharacterNameText != nullptr)
	{
		CharacterNameText->SetText(playerName);
	}
}

void UPlayerInformationDisplayBox::SetPlayerFlavourText(const FText& flavourText)
{
	if (FlavourText->IsValidLowLevel())
		FlavourText->SetText(flavourText);
}
void UPlayerInformationDisplayBox::SetWeaponInformationText(const FText& weaponInfoText)
{
	if (WeaponInformationText->IsValidLowLevel())
		WeaponInformationText->SetText(weaponInfoText);
}
void UPlayerInformationDisplayBox::SetSpecialInformationText(const FText& specialInfoText)
{
	if (SpecialInformationText->IsValidLowLevel())
		SpecialInformationText->SetText(specialInfoText);
}
