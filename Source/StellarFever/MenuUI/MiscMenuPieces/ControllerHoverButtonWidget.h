// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControllerHoverButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UControllerHoverButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UPROPERTY(EditAnywhere)
		TSubclassOf<class USFButton> ButtonStyleTemplate;

	UPROPERTY(EditAnywhere)
	FButtonStyle CommonButtonStyle;
	UPROPERTY(EditAnywhere)
	FButtonStyle HoveredButtonStyle;

	UPROPERTY(VisibleAnywhere)
		TArray<class UButton*> _Buttons;
};
