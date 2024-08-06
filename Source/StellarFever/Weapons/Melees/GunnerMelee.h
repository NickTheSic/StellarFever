// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MeleeBase.h"
#include "GunnerMelee.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGunnerMelee : public AMeleeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class USphereComponent* pSphere;

	UPROPERTY(EditAnywhere)
	class URadialForceComponent* pRadialForce;

	UPROPERTY(EditAnywhere, Category = "Gunner Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fExplosionDamage;

	UPROPERTY(EditAnywhere, Category = "Gunner Melee Stats")
		class USoundCue* pExplodeSound;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCameraShake> cameraShake;

public:
	AGunnerMelee();

	virtual void BeginPlay() override;

	virtual void AttackStart() override;
	virtual void Attack() override;
	virtual void AttackStop() override;
	virtual void AltAttackStart() override;
	virtual void AltAttack() override;
	virtual void AltAttackStop() override;

	void ExplosionHammer();

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayWorldCameraShake();
};
