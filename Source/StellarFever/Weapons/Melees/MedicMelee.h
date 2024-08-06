// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MeleeBase.h"
#include "MedicMelee.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMedicMelee : public AMeleeBase
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, Category = "Medic Melee Stats")
		class USoundCue* pAltFireSound;

public:
	AMedicMelee();

	virtual void AltAttackStart() override;
	virtual void AltAttack() override;
	virtual void AltAttackStop() override;
};
