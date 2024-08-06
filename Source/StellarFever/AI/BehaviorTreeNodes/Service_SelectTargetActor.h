// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Templates/SubclassOf.h"
#include "Service_SelectTargetActor.generated.h"

class UAISense;
class AActor;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UService_SelectTargetActor : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UService_SelectTargetActor();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
	TArray<AActor*> pOutActors;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector BB_TargetMechanism;

private:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<UAISense> AISense = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
	AActor* pBestTargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
	float fNearestTargetDistance = 1000000000.f;
};
