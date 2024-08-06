// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseAICharacter_AIController.generated.h"

class UBlackboardComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABaseAICharacter_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAICharacter_AIController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override; //FIX?
	virtual void OnPossess(APawn* const pawn) override; //FIX?

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
		UBlackboardComponent* BlackboardComponent;

private:

	UAISenseConfig_Sight* Sight_Config;

	UAISenseConfig_Hearing* Hearing_Config;

	UAISenseConfig_Damage* Damage_Config;

	//UFUNCTION()
		//void On_Updated(TArray<AActor*> const& updated_actors);

	UFUNCTION(BlueprintCallable)
		void On_Seen(AActor* actor, FAIStimulus const stimulus);
	UFUNCTION()
		void On_Target_Detected(AActor* actor, FAIStimulus const stimulus);

	FTimerHandle TimerHandle_ReassignTarget;

	UFUNCTION()
	void Timer_Retarget();

	bool bReassign;

	UFUNCTION()
	void DipEverySoOften();

	FTimerHandle TimerHandle_DipTimer;

	bool bDip = true;

	UAIPerceptionComponent* PerceptionComponent2;
	UAIPerceptionComponent* PerceptionComponent3;


private:
	//FGenericTeamId TeamId;
public:
	//virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	//virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
