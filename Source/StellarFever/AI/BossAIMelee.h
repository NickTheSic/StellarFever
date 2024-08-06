// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseAIBoss.h"
#include "BossAIMelee.generated.h"

class AEnemyMeleeWeapon;
class USceneComponent;
class UMaterialInterface;
class AEnemyShield;
class UDamageType;
class UHealthComponent;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABossAIMelee : public ABaseAIBoss
{
	GENERATED_BODY()
	
public:
	ABossAIMelee(FObjectInitializer const& object_initializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	//HEALTH STUFF BELOW
	UFUNCTION()
	virtual void HealSelfInitialize() override;

	UFUNCTION()
	virtual void HealSelfTick() override;

	FTimerHandle TimerHandle_HealSelf;

	int iHealIndex;

	bool bHealing2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
	float fHealPerTick = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
	float fHealTickInterval = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
	float fBelowHealthRun;

	bool bRunToHeal = false;

	bool bResetHealActive = false;

	UFUNCTION()
	void ResetHealActive();

	FTimerHandle TimerHandle_ResetHealActive;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TSubclassOf<AEnemyMeleeWeapon> StarterMeleeClass;

	void AttackMelee();

	bool bFollowed = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowPoint")
		//USceneComponent* ProtectInfrontPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		TSubclassOf<AEnemyShield> StarterShieldClass;


	//Shockwave Timer Handle
	FTimerHandle TimerHandle_ShockWave;

	UFUNCTION()
	void ShockwaveAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShockwave();
	
	bool bShockwaveEnable = false;
	bool bShockWaveFlag = false;

		//Grenades missing BELOW
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* pCyanRadius;;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
		float fShockwaveDelay = 0.9f;

	//Lunge Attack
	UFUNCTION()
	void LungeAttack();
	bool bLungeAttack = false;
	FTimerHandle TimerHandle_LungeAttack;


	UFUNCTION()
	void EnableShield();
	UFUNCTION()
	void DisableShield();

	bool bShieldRestart = false;
	FTimerHandle TimerHandle_ShieldRestart;


	//UFUNCTION()
	virtual void StrafeFunction() override;
	
	//bool bStrafeAway = false;
	//FTimerHandle TimerHandle_Strafe;

	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
		UAnimMontage* Attack1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
		UAnimMontage* Attack2;
};
