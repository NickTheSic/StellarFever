// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "..\Components/HealthComponent.h"
#include "AIEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ExploderAI.generated.h"

class UBehaviorTree;
class UPawnSensingComponent;
class ABasePlayer;
class UNiagaraComponent;
class UNiagaraSystem;
class UMaterialInterface;

UENUM(BlueprintType)
enum Status
{
	Patrolling   UMETA(DisplayName = "Patrolling"),
	Chasing      UMETA(DisplayName = "Chasing"),
	Exploding	 UMETA(DisplayName = "Exploding"),
};

UCLASS()
class STELLARFEVER_API AExploderAI : public AAIEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AExploderAI();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* pSphereExplosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* pSphereInstigate;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;

	ABasePlayer* TargetActor;

	//UFUNCTION() //delegate
		void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
			class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Category = "Exploder")
	float fMovementSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
	float fRequiredDistanceToTarget;

	void SelfDestruct();

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void MulticastSelfDestruct();

	//bool bExploded;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* pGreenRadius;

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
	float fExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
	float fExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
	float fSelfDamageInterval;

	FTimerHandle TimerHandle_SelfDamage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exploder")
		float InitialDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class USoundCue* pSelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class USoundCue* pExplodeSound;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound", Replicated)
		class UAudioComponent* pBallAudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		class USoundCue* pBallRollSound;

		bool bBallRoll = false;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class UParticleSystem* pExplosionEffect;

	// Dynamic material to pulse on damage
	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* pMatInst;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastExplode();

	float fNearestTargetDistance;

	class AActor* pBestTargetActor;
	TArray<AActor*> pOutActors;
	ABasePlayer* LockedPlayer;

	FVector DeactivateSpot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	TEnumAsByte<Status> eStatus;


	//EXPLODER BOT SMOKE GRENADE STUFF
	UPROPERTY(EditAnywhere, Category = "Particles")
		UNiagaraSystem* pExplosionNiagara;

	UNiagaraComponent* pNiagaraEffect;

	bool bDisabled = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Reset() override;

	void FindNextTarget();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Activate() override;

	UFUNCTION()
	void SurviveBeginning();

	FTimerHandle TimerHandle_SurviveBeginning;

	FTimerHandle TimerHandle_DisableTimer;

	
};
