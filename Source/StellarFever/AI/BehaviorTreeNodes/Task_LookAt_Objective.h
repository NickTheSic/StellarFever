// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_LookAt_Objective.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_LookAt_Objective : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	
	UTask_LookAt_Objective(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};
