// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "JanitorAI_AIController.generated.h"

class UBlackboardComponent;


/**
 * 
 */
UCLASS()
class STELLARFEVER_API AJanitorAI_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AJanitorAI_AIController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* const pawn) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated)
	UBlackboardComponent* BlackboardComponent;
	
};
