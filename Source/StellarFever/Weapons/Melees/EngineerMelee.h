// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MeleeBase.h"
#include "EngineerMelee.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEngineerMelee : public AMeleeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* pBurnArea;

	bool bIsBurning;

	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnDamage;
	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnDuration;
	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnHitsPerSecond;

	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats", meta = (AllowPrivateAccess = "true"))
		float fBurnCooldown;
	FTimerHandle BurnTimer;

	UPROPERTY(EditAnywhere, Category = "Engineer Melee Stats")
		class USoundCue* pAltFireSound;

	UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
		TArray<class AAIEnemy*> CheckArray;     //enemies to raycast against every x seconds
	UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
		TArray<class AAIEnemy*> BurnArray;     //enemies to raycast against every x seconds
public:
	AEngineerMelee();

	virtual void AltAttackStart() override;
	virtual void AltAttack() override;
	virtual void AltAttackStop() override;

	void EndBurn();

	void IgniteCheck();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
};
