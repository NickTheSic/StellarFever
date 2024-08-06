// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MapSelectButton.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMapSelectButton : public UButton
{
	GENERATED_BODY()

public:

	FName MapName;

	void Setup(FName mapName, class UMainMenuSwitcher* menuSwitcher);

	UFUNCTION()
		void OnClick();
	
	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

};
