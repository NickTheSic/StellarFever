// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseAICharacter.h"
#include "RangeAI.generated.h"

class AActor;
class AShieldAI;
class ABasePlayer;
class URaycastComponent;
class ABossAI;
class ABaseAIBoss;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API ARangeAI : public ABaseAICharacter
{
	GENERATED_BODY()

public:
	ARangeAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;



	AShieldAI* pShieldAI;

	void FindShieldAI();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutShieldAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		AActor* pBestShieldAI;

	float fNearestShieldAIDistance;

	FVector hidePosition; //FOR Shield AI
	FVector coverPosition; //FOR Boss AI

	//When behind cover, pull out to attack so often
	UFUNCTION()
	void CoverAttack();

	FTimerHandle TimerHandle_CoverAttack;

	bool bCoverAttackMode = false;

	ABasePlayer* targetPlayer;
	float DistanceTarget;


	//More then 1 boss can happen
	void FindBossAIMelee();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutBossAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		AActor* pBestBossAI;

	float fNearestBossAIDistance;

	//bool bWithinBossRange = false;

	bool bFollowingBoss = false;

	ABaseAIBoss* CurrentBossAI = nullptr;

	ABaseAIBoss* bossAI2;
	URaycastComponent* pRaycastComponent2 = nullptr;


};
