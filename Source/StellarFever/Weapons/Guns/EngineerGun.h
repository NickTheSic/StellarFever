// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GunBase.h"
#include "EngineerGun.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEngineerGun : public AGunBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		class UAnimSequence* pAnimation; //For shooting/attacking/throwing animations
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		TSubclassOf<class UWeaponAnimInstance> pAnimationInstance; //For shooting/attacking/throwing animations

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		class UEngineerBulletPoolComponent* BulletPool;
	uint8 ucMaxActiveBullets;

	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnDuration;
	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnsPerSecond;
	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fExplosionDamage;

	bool bCanShoot;

	FTimerHandle CooldownTimer;

	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShake> BulletCameraShake;

	UPROPERTY(EditAnywhere, Category = "Engineer Gun Stats", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* pExplosionEffect;

public:
	AEngineerGun();

	//When we left click
	virtual void AttackStart() override;
	//Attack code
	virtual void Attack() override;
	//When we release left click
	virtual void AttackStop() override;

	//When we right click
	virtual void AltAttackStart() override;
	//AltAttack code
	virtual void AltAttack() override;
	//When we release right click
	virtual void AltAttackStop() override;

	UFUNCTION(Server, Reliable)
	virtual void Server_AttackCooldown();

	TSubclassOf<class UCameraShake> GetBulletCameraShake() { return BulletCameraShake; }

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
