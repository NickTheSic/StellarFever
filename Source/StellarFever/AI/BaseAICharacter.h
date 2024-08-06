// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BaseAICharacter.generated.h"

class UHealthComponent;
//class AWeapon;
class USoundBase;
class UAnimMontage;
class UParticleSystem;
class UDamageType;
class UParticleSystemComponent;
class UMaterial;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;
class AWeaponAI;
class ABasePlayer;
class APatrolPath;
class AStellarFeverGameModeBase;
class AAICharacter;
class UPawnSensingComponent;
class ABaseEvent;
class AScavengerAI;

UCLASS()
class STELLARFEVER_API ABaseAICharacter : public AAIEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);
	
	FTimerHandle TimerHandle_StuckAI;

	UFUNCTION()
	void UnstickAI();

	FTimerHandle TimerHandle_ResetShotInCover;

	bool bResetShotInCover = false;

	UFUNCTION()
	void ResetShotInCover();

	//TEMPORARY, REMOVE ME IF YOU NEED, JUSTIN - Matt
	class URaycastComponent* pRaycastComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TSubclassOf<AWeaponAI> StarterWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Player")
	FName WeaponAttachSocketName;

	UFUNCTION(BlueprintCallable, Category = "Player")
	virtual void Fire();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Meshes, Replicated)
		class UAnimBlueprintGeneratedClass* AnimBP_Stand;*/

	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	//bool bDied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerStart")
	bool bEnableTriggerAtStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StopFire")
	bool bStopFiring;
	

	// NEW!!!!!!!!!

	/* Meant for Melee AI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* pMeleeAnimation;


	virtual void FindNextTarget() override;

	UFUNCTION()
		void IsAbleToFire_Code();
		
	UPROPERTY()
	bool bWithinRange;

	bool bChaseEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
		float fFireDistance = 1550;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UBehaviorTree* BehaviorTree;

	bool bDontFire;

	// Changing Enemies when they die code
	float fNearestTargetDistance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
		AActor* pBestTargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutActors;

	FTimerHandle TimerHandle_ChangeDip;

	bool bStandStill;

	float DistancePlayer;



	// Get Events - Rob code
	TArray<AActor*> pGetAllEvents;
	TArray<AActor*> pAllPlayers;
	bool bEventProceed = false;

	ABaseEvent* CurrentMechanism = nullptr;

	bool bPlayerDown;

	void AfterEventsAreGone();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float fReturnToEventDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event", meta = (AllowPrivateAccess = "true"))
		AActor* pBestEventActor;

	float fNearestEventDistance;

	void GetRandomEvent();

	UFUNCTION()
		void FindNextEvent(); //dunno yet? May be optimized


	//BOSS STUFF
	virtual void HealSelfInitialize();

	virtual void HealSelfTick();

	bool bDip = true;

	// if player is within raycast
	bool bPlayerDetected;

	void DetectPlayer();

	//FTimerHandle TimerHandle_Initialize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float fMovementDistance;

	//APatrolPath* get_patrol_path();

	UFUNCTION() // gets called when a single patrol point is reached
	void ChangeDirectionIncrement();

	FTimerHandle TimerHandle_DipTimer;
	UFUNCTION()
	void EnemyDipTimer();
	UFUNCTION()
	void ChangeDip();

	void WithinRangeDip();

	bool bDipRange = false;

	//true if all players are dead
	bool bEndGame = false;

	UFUNCTION()
	virtual void Activate() override;

	UFUNCTION()
	virtual void Deactivate() override;

	UFUNCTION()
	virtual void Reset() override;

	UFUNCTION()
	void EnableTest();

	class URaycastComponent* GetRaycastComponent();

	FTimerHandle TimerHandle_ResetAI; //reset event searching
	FTimerHandle TimerHandle_ResetAIFind;

	/* Attack Range in Behavior Tree */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fAttackRangeDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duck", Replicated)
	bool bIsCrouching;

	FVector RaycastBegin;
	FVector RaycastEnd;

	
	FTimerHandle TimerHandle_StandupPeriod;

	UFUNCTION()
	void StandupPeriod();

	bool bStandupPeriod = false;


	//Return to what they were assigned
	void ReturnToRegularMode();

	bool bBehindAI = false;
	bool bWithinBossRange = false;
	bool bReturnToRegular = false; //meant for ShieldAI


	//LOCK DOOR STUFF //////////
	UFUNCTION()
	virtual void FindRandomDoor() override;


	//SCAVENGER STUFF!!!!!!!
	void FindingScavengerClosest();

	//Chosen Scavenger
	AScavengerAI* scavenger;

	//Target Actor
	ABasePlayer* basePlayer;

	bool bChasingPlayerScavenger = false;

	// Changing Enemies when they die code
	float fNearestScavengerDistance;

	AScavengerAI* scavengerAI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
		AActor* pBestScavengerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutScavengers;

		//Scavengers took
		bool bTook;

		AScavengerAI* FollowScavenger = nullptr;

	bool bProtectingBoss = false;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:

};