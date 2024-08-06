// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager2.generated.h"

UCLASS()
class STELLARFEVER_API ATutorialManager2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialManager2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float deltaTime) override;

	UPROPERTY(EditAnywhere)
		TArray<class AAIEnemy*> ACAEnemies;
	UPROPERTY(EditAnywhere)
		TArray<class AAIEnemy*> GUNEnemies;
	UPROPERTY(EditAnywhere)
		TArray<class AAIEnemy*> MELEEEnemies;
	UPROPERTY(EditAnywhere)
		TArray<class AAIEnemy*> GRENADEEnemies;
	UPROPERTY(EditAnywhere)
		TArray<class AAIEnemy*> SHIPEnemies;

	AAIEnemy* pScavenger;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AScavengerAI> subclass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AVaccineObject> vaccineSubclass;

	UPROPERTY(EditAnywhere)
		TArray<class AShipDoor*> ShipDoors;

	UPROPERTY(EditAnywhere)
		TArray<class USoundCue*> SoundCues;

	UPROPERTY(EditAnywhere)
		class ASpawnPoint* SpawnPoint;

	UPROPERTY(EditAnywhere)
		class ASpawnPoint* VaccineSpawnPoint;

	UFUNCTION(BlueprintCallable)
		void UnlockDoor(AShipDoor* door);
	UFUNCTION(BlueprintCallable)
		void LockDoor(AShipDoor* door);

	UFUNCTION(BlueprintCallable)
		AShipDoor* GetDoor1() { return ShipDoors[0]; }
	UFUNCTION(BlueprintCallable)
		AShipDoor* GetDoor2() { return ShipDoors[1]; }
	UFUNCTION(BlueprintCallable)
		AShipDoor* GetDoor3() { return ShipDoors[2]; }
	UFUNCTION(BlueprintCallable)
		AShipDoor* GetDoor4() { return ShipDoors[3]; }

	UFUNCTION(BlueprintCallable)
		void PrePlanetSelect();

	UFUNCTION(BlueprintCallable)
		int32 GetProgression() { return ProgressionTracker; }

	UFUNCTION(BlueprintCallable)
		void IncrementProgression();

	UPROPERTY(EditAnywhere)
		AActor* TeleporterWall;
	UPROPERTY(EditAnywhere)
		class AEventObject* ESButton;

	class ABasePlayer* player;

	class UAudioComponent* pAudioComponent;
	FTimerHandle TutorialSoundTimer;
	UFUNCTION()
	void OnAudioFinished();

	int32 ProgressionTracker;

	//List of the events that will activate when entering the room
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Events")
		TArray<class ABaseEvent*> pEvents;

	bool bHPDone, bAmmoDone, bEnemyShipDone;
	void UpdateMedBay() { bHPDone = true; }
	void UpdateAmmoBay() { bAmmoDone = true; }

	void PlayMovement();
	void PlayMovement_Reminder();	//movement reminder
	void PlayPS();
	void PlayPS_Reminder();	//planet select reminder
	void PlayFixEvent();
	void PlayFixEvent_Reminder();
	void PlayEventLine();
	UFUNCTION(BlueprintCallable)
	void PlayEventDescriptionLine();
	UFUNCTION(BlueprintCallable)
	void PlayACACombatLine();
	void PlayACACombatLine_Reminder();
	void PlayGunCombatLine();
	void PlayGunCombatLine_Reminder();
	void PlayMeleeCombatLine();
	void PlayMeleeCombatLine_Reminder();
	void PlayGrenadeCombatLine();
	void PlayGrenadeCombatLine_Reminder();
	void PlayHPAmmoBay();
	void PlayHPAmmoBay_Reminder();
	void PlayScavengerChase();
	void PlayScavengerChase_Reminder();
	void PlayVaccineDropOff();
	void PlayVaccineDropOff_Reminder();
	UFUNCTION(BlueprintCallable)
	void PlayTutorialDone();
	void TutorialDone();
};
