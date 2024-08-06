// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "MeleeBase.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMeleeBase : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMeleeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//used for swing animation
	//UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	//	class UBoxComponent* pHitBox;

	FTimerHandle CooldownTimer;

	UPROPERTY(EditAnywhere, Category = "Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fCooldownTime;
	bool bIsCoolingDown;

	UPROPERTY(EditAnywhere, Category = "Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fWeaponRange;

	//Melee sounds
	UPROPERTY(EditAnywhere, Category = "Melee Stats")
		class USoundCue* pMissSound;
	UPROPERTY(EditAnywhere, Category = "Melee Stats")
		class USoundCue* pHitSound;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void AttackCompleted();

	virtual void SetIsCooledDown();

	virtual void AttackStart() override;
	virtual void Attack() override;
	virtual void AttackStop() override;

	virtual bool DealDamage(const FHitResult* hit, float newDamage);
};
