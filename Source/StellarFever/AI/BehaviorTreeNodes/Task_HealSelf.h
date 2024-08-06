// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_HealSelf.generated.h"

class ABaseAICharacter_AIController;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_HealSelf : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UTask_HealSelf(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
