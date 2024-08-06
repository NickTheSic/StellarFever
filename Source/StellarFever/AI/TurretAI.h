// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AIEnemy.h"
#include "TurretAI.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UPawnSensingComponent;
class ABasePlayer;
class UDamageType;
class UParticleSystem;
class USoundBase;
class UArrowComponent;
class UCapsuleComponent;
class UHealthComponent;
class UParticleSystemComponent;
class UPrimitiveComponent;
class UPhysicalMaterial;

UCLASS()
//class STELLARFEVER_API ATurretAI : public APawn
class STELLARFEVER_API ATurretAI : public AAIEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATurretAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings")
	bool bLazerTurret;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings")
		float fTurnAngle;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ATurretLaserProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCapsuleComponent* pCapsuleCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* pMeshTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* pMeshBottom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TracerTargetName;

	float fHealthIncrease;

	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	
	UFUNCTION()
		void OnPawnSeen(APawn* SeenPawn);

	UPROPERTY()
	ABasePlayer* playerSeen = nullptr;

	UPROPERTY()
	bool bPlayerSeen = false;


	//ROTATION VARIABLES
	UPROPERTY(EditAnywhere, Category = "Movement")
	float fPitchValue;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float fYawValue;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float fRollValue;

	UPROPERTY()
	bool bRotateDirection;

	FTimerHandle TimerHandle_SwitchDirection;

	UFUNCTION()
	void SwitchDirection();


	UFUNCTION()
	void FirePrepare();
	UFUNCTION()
	void FireCommence();
	UFUNCTION()
	void FireCommenceProjectile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* pMuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* pTracerEffect;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* pDeathExplosionEffect;*/

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* pFireSound;

	UPROPERTY()
	float fFireTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fTimeBetweenShots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fTimeBetweenShotsProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fBulletSpread;
	float fWeaponRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fItemLoudness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fBaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float fTurretSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void ResetTurret();

	FTimerHandle TimerHandle_ResetRotation;

	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	FVector InitialForwardVector;

	UPROPERTY()
	bool bResetTurret = false;

	bool bPatrolMode = true;

	UFUNCTION()
	void ResetTurretPositionTimer();
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastFireExtras(FVector TracerEndPoint);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastDeathExtras();

	UPROPERTY()
	bool bStartTrigger = false;
	UFUNCTION()
	void StartTrigger();

	UPROPERTY()
	bool bPlayerDead = false;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


	//C++ Attempt on blueprints! (May not keep)
	//UFUNCTION()
	void TickLaser();
	void SetupLaser();
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastLaser();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", Replicated)
	UArrowComponent* pArrow2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", Replicated)
	UParticleSystemComponent* pLaserParticle;


	TArray<AActor*> pobjectVariable;
	float fEventDeltaSeconds;
	FHitResult LineTraceSingle_OutHit;
	bool bBlockingHit;
	bool bInitialOverlap;
	float fTime;
	float fDistance;
	FVector BreakHitResult_Location;
	FVector BreakHitResult_ImpactPoint;
	FVector BreakHitResult_Normal;
	FVector BreakHitResult_ImpactNormal;
	UPhysicalMaterial* pPhysMat;
	AActor* pHitActor;
	UPrimitiveComponent* pHitComponent;
	FName HitBoneName;
	int32 HitItem;
	int32 FaceIndex;
	FVector TraceStart2;
	FVector TraceEnd2;

};
