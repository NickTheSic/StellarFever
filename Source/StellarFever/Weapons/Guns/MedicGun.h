// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GunBase.h"
#include "MedicGun.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMedicGun : public AGunBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		class UAnimSequence* pAnimation; //For shooting/attacking/throwing animations
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		TSubclassOf<class UWeaponAnimInstance> pAnimationInstance; //For shooting/attacking/throwing animations
	UPROPERTY(EditAnywhere, Category = "Medic Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fAltAttackCharge;

	//Used every time we shoot
	UPROPERTY(EditAnywhere, Category = "Medic Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBulletSpreadIncrease;
	//Used every tick while not shooting
	UPROPERTY(EditAnywhere, Category = "Medic Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fBulletSpreadDecrease;

	//Medic Gun sounds
	UPROPERTY(EditAnywhere, Category = "Medic Gun Stats")
		class USoundCue* pAltFireSound;

	virtual void BeginPlay() override;

public:
	AMedicGun();

	virtual void Tick(float deltaTime) override;

	//When we left click
	virtual void AttackStart() override;
	//Attack code
	virtual void Attack() override;
	//When we release left click
	virtual void AttackStop() override;

	//When we right click
	virtual void AltAttackStart() override;
	//AltAttack codeS
	virtual void AltAttack() override;
	//When we release right click
	virtual void AltAttackStop() override;
};
