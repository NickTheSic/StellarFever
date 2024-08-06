// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Drops/DropsBase.h"
#include "CurrencyDrop.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ACurrencyDrop : public ADropsBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;	
};
