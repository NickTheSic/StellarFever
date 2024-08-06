// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../StoreItem.h"
#include "FuelRefills.generated.h"

/**
 *
 */
UCLASS()
class STELLARFEVER_API AFuelRefills : public AStoreItem
{
	GENERATED_BODY()

public:
	virtual void Purchase() override;

};
