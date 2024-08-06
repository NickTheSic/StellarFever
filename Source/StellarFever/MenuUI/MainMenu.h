// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMainMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()
public:

	void NativeOnInitialized() override;	//Used like BeginPlay

	void NativeConstruct() override;		//Any time it appears on screen


	//Bind Dynamic functions
	UFUNCTION()
		void OnClickExit();
	UFUNCTION()
		void OnClickPlay();
	UFUNCTION()
		void OnClickTutorial();
	UFUNCTION()
		void OnClickOptions();
	UFUNCTION()
		void OnClickControls();
	
	UFUNCTION()
		void OnClickHost();
	UFUNCTION()
		void OnClickJoin();

	UFUNCTION()
		void OnClickMapSelect();

	//The UI Elements I want displayed on screen
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TitleText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* PlayButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* HostButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* JoinButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* TutorialButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* OptionsButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ExitButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ControlsButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* MapSelectButton = nullptr;

	UPROPERTY()
	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;


	void SetEnablePlay();

};
