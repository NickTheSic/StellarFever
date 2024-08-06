// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_FindPatrolPathPointExploder.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_FindPatrolPathPointExploder : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UTask_FindPatrolPathPointExploder(FObjectInitializer const& object_initializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
