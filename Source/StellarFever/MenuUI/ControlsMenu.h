// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "ControlsMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UControlsMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	class UPauseMenu* pPauseMenuSwitcher = nullptr;

	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackButton = nullptr;

	UFUNCTION()
		void OnClickBack();
	
};
