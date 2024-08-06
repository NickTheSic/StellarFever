// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "AIEnemy.generated.h"

class UHealthComponent;
class UMaterial;
class UDamageType;
class ADropsBase;
class AWeaponAI;
class AEnemyShield;
class APatrolPath;
class AEnemyMeleeWeapon;
class AShipDoor;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class STELLARFEVER_API AAIEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat1;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat2;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat3;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat4;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat5;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pMat6;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pStunnedMat;

		

	/*UPROPERTY(EditDefaultsOnly, Category = "Material")
		UMaterial* pPoisonedMat;*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	//	USkeletalMeshComponent* pMesh2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UHealthComponent* pHealthComp;

	UFUNCTION() //delegate
	void OnHealthChanged(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
			class AController* InstigatedBy, AActor* DamageCauser);

	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser);

	//FIRE BURN DAMAGE CODE/////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BurnDamage")
		UParticleSystem* pOnFireParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BurnDamage")
		UParticleSystemComponent* pFireParticleSystem;

	//EXPLODER BOT SMOKE GRENADE STUFF
	UPROPERTY(EditAnywhere, Category = "BurnDamage")
		UNiagaraSystem* pFireNiagara;

	UNiagaraComponent* pNiagaraFireEffect;


	//DeathANIM
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
		UParticleSystem* pDeathExplosionEffect;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastExplosion();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bDied;

	bool bLookTowardsObjective;

	UPROPERTY(EditAnywhere, Category = "Death")
		USoundBase* pDeathSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BurnDamage")
		float fBurnDamageDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BurnDamage")
		float fBurnDamageDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BurnDamage")
		float fBurnDamageAmount;

	FTimerDelegate TimerDelegate_BurnDamage;
	FTimerHandle TimerHandle_BurnDamage;

	//UFUNCTION(BlueprintCallable)
	void InitiateBurnDamage(AActor* DamageCauser, float DamageValue, float Duration, float TimesPerSecond);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastBurnEffect();

	UFUNCTION()
		void BurnDamage(AActor* DamageCauser, float DamageValue);

	void EndBurnDamage();

	bool bEnableFlames = false;

	bool bExploded; //meant for exploders

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		bool bShipBoarded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		bool bShipPatroller = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		bool bChasePlayers = false;

	/* true = enemy moves back and forth between point. false = enemy moves circular around all points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		bool bBiDirectional = false;

	/* Enable this bool if the AI is a melee attacker! */
	bool bMeleeAttacker;
	/* Enable this bool if the AI is a boss attacker! */
	bool bBossAttacker;
	bool bGrenadePlay = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
		AEnemyMeleeWeapon* pBossMeleeWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	APatrolPath* get_secure_patrol_path;
	APatrolPath* get_patrol_path();

	TArray<AActor*> SecurePaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		APatrolPath* patrol_path;

	// Items AI Carry
	UPROPERTY(Replicated)
	AEnemyShield* pCurrentShield;

	bool bShieldEnemy = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
	AWeaponAI* pCurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) //meant for scavenger
	bool bDisabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exploder") //meant for scavenger
	bool bExploder;


	FVector DeactivateSpot;

	//STUN CODE/////////////
	FTimerHandle TimerHandle_Stunned;
	float fOriginalWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stunned")
		float fStunnedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stunned")
		bool bStunned;

	UFUNCTION(BlueprintCallable)
		void Stunned();

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastStunEffectOn();

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastStunEffectOff();

	void StunnedRelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops")
		TArray<TSubclassOf<ADropsBase> > DropItemsArray;

	ADropsBase* DropItemClass;

	void DropItem();

	bool bIsActive;  //Robs code
	bool GetIsActive();  //Robs code
	 
	virtual void Activate();

	virtual void Deactivate();

	virtual void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fAIMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float InitialSpeed;

	//Increase Difficulty Code
	//float fDamageIncrease;
	float fSpeedIncrease;
	float fRangeIncrease;
	float fHealthIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
		float InitialHealth;

	float fLevelDifficulty; //? get gamemode

	bool bReset = false;

	void ShipBoardedActivate();
	void ChasePlayerActivate();
	void ChaseDoorsActivate();
	void ShipBoardedScavengersActivate();

	bool bScavengerBoardedEnemyShip = false;

	//Gives AI random abilities
	void ChooseRandomAction();


	UFUNCTION()
	virtual void FindNextTarget();

	bool bResetDoor;


	//LOCK DOOR STUFF
	//UFUNCTION()
	virtual void FindRandomDoor();

	bool bDoorProceed = false;
	TArray<AActor*> pGetAllDoors;
	AShipDoor* CurrentDoor = nullptr;
	FTimerHandle TimerHandle_DoorReset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	bool bGetDoors;

	bool bDoorReset = true;
	bool bReturnToBattle = false;
	bool bEndDoor = false;
	bool bEnableDoorMovement = false;

private:

public:

};