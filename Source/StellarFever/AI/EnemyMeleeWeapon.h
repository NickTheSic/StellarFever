// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponAI.h"
#include "EnemyMeleeWeapon.generated.h"

class USoundBase;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEnemyMeleeWeapon : public AWeaponAI
{
	GENERATED_BODY()

public:

	AEnemyMeleeWeapon();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//virtual void FireAuto(float val) override;
	virtual void FireSingleAI() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fAttackRange;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* pMeleeHitSound;

	FTimerHandle TimerHandle_SwingDuration;
	FTimerHandle TimerHandle_AttackReset;

	bool bMeleeAttackRange; //melee attack duration (collider shown)
	bool bAttackReset; //cooldown before next shot

	float fMeleeAttackActive;

	bool bMeleeHit; //Can only hit once per swing

	UFUNCTION()
	void StopSwing();
	UFUNCTION()
	void ResetAttack();
	UFUNCTION()
	void ActuallyAttack(); //meant for Anim

	void MeleeSocketCollision();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastFireExtras();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSwing();
	UFUNCTION(NetMulticast, Reliable)
		void MulticastSwingSound();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastHitSound();
	
};
