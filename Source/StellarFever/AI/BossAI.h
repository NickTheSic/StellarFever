// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseAIBoss.h"
#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "BossAI.generated.h"

class AEnemyMeleeWeapon;
class USceneComponent;
class AFragGrenadeAI;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABossAI : public ABaseAIBoss
{
	GENERATED_BODY()

public:
	ABossAI(FObjectInitializer const& object_initializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void HealSelfInitialize() override;

	UFUNCTION()
	virtual void HealSelfTick() override;

	FTimerHandle TimerHandle_HealSelf;

	int iHealIndex;

	bool bHealing2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
		float fHealPerTick = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
		float fHealTickInterval = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal Floats")
	float fBelowHealthRun;

	bool bRunToHeal = false;

	bool bResetHealActive = false;

	UFUNCTION()
	void ResetHealActive();

	FTimerHandle TimerHandle_ResetHealActive;


	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
	//AEnemyMeleeWeapon* pBossMeleeWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TSubclassOf<AEnemyMeleeWeapon> StarterMeleeClass;

	void AttackMelee();

	bool bFollowed = false;
	


	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser) override;

	virtual void StrafeFunction() override;


	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowPoint")
		USceneComponent* ProtectInfrontPosition;*/


	virtual void Activate() override;

		// OLD CONVERTED CODE - blueprint to C++

	//UFUNCTION(BlueprintCallable, meta = (Category, OverrideNativeName = "ShotGrenade"))
	//	void ShotGrenade();

	////UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Gravity", Category = "Default", MultiLine = "true", OverrideNativeName = "Gravity"))
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Gravity", Category = "Default", MultiLine = "true", OverrideNativeName = "Gravity"))
	//	float Gravity;

	////UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Velocity", Category = "Default", MultiLine = "true", OverrideNativeName = "Velocity"))
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Velocity", Category = "Default", MultiLine = "true", OverrideNativeName = "Velocity"))
	//	float Velocity;

	UFUNCTION()
	void ShotGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grenades")
	TSubclassOf<AFragGrenadeAI> FragGrenadeClass;

	FTimerHandle TimerHandle_SpawnGrenade;

	//bool bGrenadePlay = false;
};
