// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../StoreItem.h"
#include "AmmoBayRefills.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AAmmoBayRefills : public AStoreItem
{
	GENERATED_BODY()
	
public:
	virtual void Purchase() override;
};
