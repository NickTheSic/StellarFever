// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MeleeBase.h"
#include "CaptainMelee.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ACaptainMelee : public AMeleeBase
{
	GENERATED_BODY()
	
public:
	ACaptainMelee();

protected:

	bool bIsRaging;
	UPROPERTY(EditAnywhere, Category = "Captain Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fRageDamage;
	UPROPERTY(EditAnywhere, Category = "Captain Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fRageDuration;

	UPROPERTY(EditAnywhere, Category = "Captain Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fStunChance;
	UPROPERTY(EditAnywhere, Category = "Captain Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fRageStunChance;

public:
	virtual void AttackStart() override;
	virtual void Attack() override;
	virtual void AttackStop() override;

	virtual void AltAttackStart() override;
	virtual void AltAttack() override;
	virtual void AltAttackStop() override;

	void EndRage();
};