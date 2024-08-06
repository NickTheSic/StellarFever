// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIEnemy.h"
#include "ScavengerAI.generated.h"

class UHealthComponent;
class UParticleSystemComponent;
class UMaterial;
class UDamageType;
class UStaticMeshComponent;
class USphereComponent;
class AVaccineObject;

UCLASS()
class STELLARFEVER_API AScavengerAI : public AAIEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AScavengerAI();

	AVaccineObject* pVaccine = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vaccine")
	AVaccineObject* pVaccine2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vaccine")
	UStaticMeshComponent* pVaccineModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* pInteractCollider;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	//float fAverageMovementSpeed = 1000;

	UPROPERTY()
	AActor* BeginSpawnActor;


	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
			class AController* InstigatedBy, AActor* DamageCauser) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Reset() override;


	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	void FindClosestVaccine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowPrivateAccess = "true"))
		AActor* pBestTargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
		TArray<AActor*> pOutVaccines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bVaccineRetrieved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bScavengerHidden = false;

	float fNearestTargetDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SpawnedLocation;

	FTimerHandle TimerHandle_VaccineSearch;


	FTimerHandle TimerHandle_Dissappear;

	UFUNCTION()
	void Disappear();

	bool bDisappear = false;


	FTimerHandle TimerHandle_PreventDisappearing;

	UFUNCTION()
	void BeginPreventDisappearing();

	bool bPreventDisappearing = false;

	UFUNCTION()
	void TestDrop();
	FTimerHandle TimerHandle_DropTest;

	UFUNCTION()
	void DropVaccineDelay();
	FTimerHandle TimerHandle_DropVaccine;

	bool bVaccineExist;


	//Tells AI if Scavenger is taken - only 1 AI per scavenger
	bool bTaken = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vaccine")
	bool bIsCarrying = false;
};

