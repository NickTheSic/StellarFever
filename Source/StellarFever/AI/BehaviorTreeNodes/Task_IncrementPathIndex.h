// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Task_IncrementPathIndex.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_IncrementPathIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UTask_IncrementPathIndex(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	enum class EDirectionType
	{
		Forward, Reverse
	};

	EDirectionType direction = EDirectionType::Forward;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bidirectional = true;
};
