// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapSelectMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMapSelectMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UScrollBox* ButtonHolder = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* BackButton = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UMapSelectButton> MapButtonTemplate = nullptr;

	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

public:

	virtual void NativeOnInitialized() override;
	
	void SetupButtons(TArray<FName> mapNames);

	UFUNCTION()
		void OnClickBack();
};
