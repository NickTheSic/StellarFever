// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../StoreItem.h"
#include "VaccineRefills.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AVaccineRefills : public AStoreItem
{
	GENERATED_BODY()

public:
	virtual void Purchase() override;
	
};
