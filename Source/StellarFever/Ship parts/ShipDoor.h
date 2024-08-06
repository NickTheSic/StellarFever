// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EmergencyLightComponent.h"
#include "ShipDoor.generated.h"

UCLASS()
class STELLARFEVER_API AShipDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShipDoor();

protected:
	UPROPERTY(EditAnywhere, Category = "door mesh", Replicated)
		class USkeletalMeshComponent* pDoorMesh;

	UPROPERTY(EditAnywhere, Category = "door trigger")
		class UBoxComponent* pDoorTrigger; //main collsion
	//walk in door open

	//emergency doesnt open
	//normal situation would open normally



	UPROPERTY(EditAnywhere, Category = "Animation")
		class UAnimSequenceBase* OpenAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
		class UAnimSequenceBase* CloseAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
		class UAnimSequenceBase* IdleAnim;


	UFUNCTION()
		void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Set Lock")
		bool bIsLocked = false; //for when AI locks door

	bool bIsDoorOpen = false;

	UPROPERTY(EditAnywhere, Category = "Num Interact")
		int numInteracted = 0;
	int numPlayer = 0;
	int numAI = 0;

	FTimerHandle LockTimer;
	FTimerHandle UnlockTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
		float HackingTimer = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
		float FixingTimer = 10.0f;

	UPROPERTY(EditAnywhere)
		class UEmergencyLightComponent* pLockLight;

	UPROPERTY(EditAnywhere)
		class UEmergencyLightComponent* pLockLight2;

	UPROPERTY(EditAnywhere)
		class UAudioComponent* pAudioComp;

	UPROPERTY(EditAnywhere)
		class USoundBase* pOpenSound1;

	UPROPERTY(EditAnywhere)
		class USoundBase* pCloseSound1;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OpenDoor();
	void ClosedDoor();

	void StartFixingDoor();
	void CancelFixingDoor();
	void FixDoor();
	void HackedDoor();
	bool GetLockedStatus();
	float GetHackTimer();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};