// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../MenuUI/MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPauseMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget))
		class UButton* ResumeButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UButton* ExitButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UButton* OptionsButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UButton* ControlsButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* PauseMenuPanel = nullptr;


	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* pPauseMenuSwitcher = nullptr;


	UPROPERTY(meta = (BindWidget))
		class UOptionsMenu* pOptionsMenu = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UControlsMenu* pControlsMenu = nullptr;


	virtual void NativeOnInitialized() override;


	UFUNCTION(BlueprintCallable)
		void ResumeGame();

	UFUNCTION(BlueprintCallable)
		void ExitGame();

	UFUNCTION(BlueprintCallable)
		void SwitchToOptions();

	UFUNCTION(BlueprintCallable)
		void SwitchToMainMenu();

	UFUNCTION(BlueprintCallable)
		void SwitchToControlsMenu();

};
