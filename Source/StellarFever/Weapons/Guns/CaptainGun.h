// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GunBase.h"
#include "CaptainGun.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ACaptainGun : public AGunBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		class UAnimSequence* pAnimation; //For shooting/attacking/throwing animations
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		TSubclassOf<class UWeaponAnimInstance> pAnimationInstance; //For shooting/attacking/throwing animations

	UPROPERTY(EditAnywhere, Category = "Captain Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBigBarrelDamage;
	UPROPERTY(EditAnywhere, Category = "Captain Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBigBarrelCoolDown;

	bool bIsMainCoolingDown, bIsAltCoolingDown;
	FTimerHandle CoolDownTimer;

	UPROPERTY(EditAnywhere, Category = "Captain Gun Stats")
		class USoundCue* pAltFireSound;

	virtual void BeginPlay() override;
public:
	ACaptainGun();

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
	void MainCoolDown();
	void AltCoolDown();

};
