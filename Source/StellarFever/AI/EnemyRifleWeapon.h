// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponAI.h"
#include "EnemyRifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEnemyRifleWeapon : public AWeaponAI
{
	GENERATED_BODY()

public:

	AEnemyRifleWeapon();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//virtual void FireAuto(float val) override;
	virtual void FireAutoAI() override;

	FTimerHandle TimerHandle_Reloading;

	UFUNCTION()
	void ReloadTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireExtras(FVector TracerEndPoint);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastReloadExtra();
};
