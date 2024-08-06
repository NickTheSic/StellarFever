// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "HostMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UHostMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* BackButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* StartHostingButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UEditableTextBox* ServerNameText = nullptr;

	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

public:

	virtual void NativeOnInitialized() override;

	UFUNCTION()
		void OnClickBack();

	UFUNCTION()
		void HostServer();

};
