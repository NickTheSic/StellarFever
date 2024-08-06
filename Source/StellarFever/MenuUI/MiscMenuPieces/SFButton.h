// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Styling/SlateTypes.h"
#include "SFButton.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API USFButton : public UButton
{
	GENERATED_BODY()

public:

	USFButton();

	UFUNCTION()
		void OnHoveredFunction();

};
