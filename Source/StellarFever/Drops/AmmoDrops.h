// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Drops/DropsBase.h"
#include "AmmoDrops.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AAmmoDrops : public ADropsBase
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;	
};
