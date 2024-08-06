// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsQuality.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQualitySetSignature, int, NewQuality);

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UOptionsQuality : public UUserWidget
{
	GENERATED_BODY()

public:

	FOnQualitySetSignature OnSetQuality;


	void NativeOnInitialized() override;


	UPROPERTY(meta = (BindWidget))
		class UComboBoxString* QualityBox = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* QualityText = nullptr;


	UFUNCTION()
		void ComboBoxSetQuality(FString SelectedItem, ESelectInfo::Type SelectionType);


	uint32 GetQualityLevel() { return uiQualityLevel; }
	void SetQualityLevel(uint32 value);

protected:
	//The current level of quality as an int
	uint32 uiQualityLevel = 4;
	
};
