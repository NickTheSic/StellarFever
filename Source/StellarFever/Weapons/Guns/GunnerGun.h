// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GunBase.h"
#include "GunnerGun.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGunnerGun : public AGunBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		class UAnimSequence* pAnimation; //For shooting/attacking/throwing animations
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		TSubclassOf<class UWeaponAnimInstance> pAnimationInstance; //For shooting/attacking/throwing animations
	//spinning up properties
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fSpinTime;
	bool bIsSpinningUp, bIsSpinningDown, bHasSpunUp;
	FTimerHandle SpinTimer;

	virtual void BeginPlay() override;

	//laser properties
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fLaserDamage;
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fLaserAttackRate;
	FTimerHandle LaserTimer;

	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* pLaserEffect;
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* pLaserHitEffect;

	//passive ammo negation chance
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		uint8 ucGunnerPassivePercent;

	//Used every time we shoot
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBulletSpreadIncrease;
	//Used every tick while not shooting
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBulletSpreadDecrease;

	//Gunner Gun sounds
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats")
		class USoundCue* pSpinUpSound;
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats")
		class USoundCue* pSpinDownSound;
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats")
		class USoundCue* pSpunUpSound;
	
	//To fire laser separate from bullets
	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* pLaserAudioComp;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* pSpinAudioComp;
	UPROPERTY(EditAnywhere, Category = "Gunner Gun Stats")
		class USoundCue* pLaserSound;


public:
	AGunnerGun();

	virtual void Tick(float deltaTime) override;

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

	virtual void ReloadStart() override;

	UFUNCTION(Server, Reliable)
		void Server_DoneSpinningUp();
	UFUNCTION(Server, Reliable)
		void Server_DoneSpinningDown();
	UFUNCTION(Server, Reliable)
		void Server_FireLaser();

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayLaserSound();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlaySpinSound(USoundCue* sound);

};
