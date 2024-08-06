// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MeleeAI2.h"
#include "ShieldAI.generated.h"

class USceneComponent;
class ASmokeGrenadeAI;
class AScavengerAI;
class UBehaviorTree;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API AShieldAI : public AMeleeAI2
{
	GENERATED_BODY()
	
public:
	AShieldAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		bool bShieldEnemy;*/

	//class AEnemyShield* pCurrentShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		TSubclassOf<AEnemyShield> StarterShieldClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* BehaviorTreeShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowPoint")
		USceneComponent* HideBehindPosition;

	bool bIsUsedAsCover = false;

	UFUNCTION()
	void ShotGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grenades")
		TSubclassOf<ASmokeGrenadeAI> SmokeGrenadeClass;

		bool bThrownGrenade = false;

		//Target Actor
		ABasePlayer* basePlayer;
		//Chosen Scavenger
		AScavengerAI* scavenger;

		virtual void Activate() override;
		virtual void Reset() override;
};
