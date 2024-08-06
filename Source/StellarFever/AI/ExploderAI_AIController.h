// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ExploderAI_AIController.generated.h"

class UBlackboardComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AExploderAI_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AExploderAI_AIController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* const pawn) override;

	virtual void Tick(float DeltaSeconds) override;


	//UAIPerceptionComponent* PerceptionComponent2;

	UFUNCTION()
		void On_Target_Detected(AActor* actor, FAIStimulus const stimulus);

	UAISenseConfig_Sight* Sight_Config;

	UAISenseConfig_Hearing* Hearing_Config;

	UAISenseConfig_Damage* Damage_Config;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
	
};
