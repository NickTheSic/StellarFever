// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_MoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:

	UTask_MoveTo(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	struct FBlackboardKeySelector WithinRangeKey;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float fMovementSpeed = 400.f;
};
