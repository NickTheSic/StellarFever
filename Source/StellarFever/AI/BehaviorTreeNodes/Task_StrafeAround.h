// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_StrafeAround.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UTask_StrafeAround : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UTask_StrafeAround(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//EBTNodeResult::Type EndTaskDelay(UBehaviorTreeComponent& OwnerComp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float Distance = 1000.0;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
		FVector LeftOrRightLane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* pMeshComp;
};
