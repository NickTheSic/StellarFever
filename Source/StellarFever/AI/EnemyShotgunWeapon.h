// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponAI.h"
#include "EnemyShotgunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEnemyShotgunWeapon : public AWeaponAI
{
	GENERATED_BODY()

public:

	AEnemyShotgunWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float fReloadTimeDelay;

	void ShotgunReload();

	FTimerHandle TimerHandle_ReloadShotgun;

	TArray<FHitResult*> HitResultArray;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void FireSingleAI() override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastFireExtras(FVector TracerEndPoint);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void MulticastReloadSound();
};