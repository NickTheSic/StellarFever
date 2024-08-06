// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Drops/DropsBase.h"
#include "MedicDrops.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMedicDrops : public ADropsBase
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
};
