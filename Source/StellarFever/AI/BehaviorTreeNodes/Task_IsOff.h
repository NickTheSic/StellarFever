// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_IsOff.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_IsOff : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UTask_IsOff(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
