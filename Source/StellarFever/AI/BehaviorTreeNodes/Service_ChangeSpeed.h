// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Service_ChangeSpeed.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UService_ChangeSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UService_ChangeSpeed();

	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bWalking;
	
	float fSpeed;

protected:
	FString GetStaticServiceDescription() const override;

private:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	//float fSpeed = 600.0f;

	
};
