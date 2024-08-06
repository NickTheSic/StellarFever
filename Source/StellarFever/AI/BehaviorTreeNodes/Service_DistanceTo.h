// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Service_DistanceTo.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UService_DistanceTo : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UService_DistanceTo();
	void OnBecomeRelevant(UBehaviorTreeComponent& owner_comp, uint8* node_memory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float fMaxDistance = 550.f;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (AllowPrivateAccess = "true"))
	struct FBlackboardKeySelector bIsWithinDistance;
	
};
