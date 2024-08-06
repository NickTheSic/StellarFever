// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSwitcher.generated.h"

/**
 * 
 */

UCLASS()
class STELLARFEVER_API UMainMenuSwitcher : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UWidgetSwitcher* pMenuSwitcher = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UMainMenu* pMainMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UPlayerSelectMenu* pPlayerSelectMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UOptionsMenu* pOptionsMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UHostMenu* pHostMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UJoinMenu* pJoinMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UMapSelectMenu* pMapSelectMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UControlsMenu* pControlsMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UCanvasPanel* pLoadingScreen = nullptr;

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	//Wrapper functions for switching to a specific menu
	UFUNCTION(BlueprintCallable)
		void SwitchToMainMenu();
	UFUNCTION(BlueprintCallable)
		void SwitchToPlayerSelect();
	UFUNCTION(BlueprintCallable)
		void SwitchToOptionsMenu();
	UFUNCTION(BlueprintCallable)
		void SwitchToHostMenu();
	UFUNCTION(BlueprintCallable)
		void SwitchToJoinMenu();
	UFUNCTION(BlueprintCallable)
		void SwitchToMapSelect();
	UFUNCTION(BlueprintCallable)
		void SwitchToControlsMenu();
	UFUNCTION(BlueprintCallable)
		void SwitchToLoadingScreen();
};
