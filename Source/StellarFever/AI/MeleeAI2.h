// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseAICharacter.h"
#include "MeleeAI2.generated.h"

class AShipDoor;
class ABaseAIBoss;
class ABasePlayer;
class ABossAI;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMeleeAI2 : public ABaseAICharacter
{
	GENERATED_BODY()

public:
	AMeleeAI2();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Reset() override;


	//BOSS RANGE STUFF
	void FindBossAIRange();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutBossAI2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		AActor* pBestBossAI;

	float fNearestBossAIDistance;

	bool bFollowingBoss = false;

	ABaseAIBoss* CurrentBossAI = nullptr;

	FVector coverPosition; //FOR Boss AI
	float DistanceTarget;

	AActor* targetPlayer2;
	ABossAI* bossAI2;

	URaycastComponent* pRaycastComponent2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProtectBoss")
	bool bProtectsShieldEnabled = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* Attack1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
		UAnimMontage* Attack2;

	virtual void Fire() override;
};
