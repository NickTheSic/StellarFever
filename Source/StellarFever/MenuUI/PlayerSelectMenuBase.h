// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "../Player/PlayerInformation.h"
#include "PlayerSelectMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerSelectMenuBase : public UControllerHoverButtonWidget
{
	GENERATED_BODY()

public:
	//The object that displays the information on screen
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerInformationDisplayBox* pInformationDisplayBox = nullptr;

	//Manually adding the players to the menu
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerSelectButton* CaptainButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerSelectButton* EngineerButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerSelectButton* GunnerButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerSelectButton* MedicButton = nullptr;

	//Regular buttons with binded functions
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* SelectButton = nullptr;

	class UButton* PreviouslyClickedPlayerButton = nullptr;

	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

	UPROPERTY(EditAnywhere)
		FButtonStyle RegularStyle;
	UPROPERTY(EditAnywhere)
		FButtonStyle HoveredStyle;

public:
	virtual void NativeOnInitialized() override;  //I use as begin play
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void SetPlayerClass(const EPlayerClass& playerClass);

	UFUNCTION()  //AFunction bound to the select button
		void SelectPlayerOnclick(); //Calls Select player on click

	virtual void SelectPlayer(); // this function gets overriden

	void OpenMenu();

};
