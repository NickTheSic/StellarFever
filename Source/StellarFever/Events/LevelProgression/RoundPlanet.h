// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoundPlanet.generated.h"

UCLASS()
class STELLARFEVER_API ARoundPlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoundPlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	/////ALL VARIABLES BELOW/////

	//The static mesh that the planet uses
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MainMesh")
		class UStaticMeshComponent* pMesh;

	//The collider to see if the player has interacted with it
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MainCollider")
		class UBoxComponent* pCollider;

	//The Value of difficulty the planet will give
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DifficultyIncrease")
		float fDifficultyValue;

	//How long the round will take
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RoundTime")
		float fRoundTime;

	//Name of Planet to be displayed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlanetName", Replicated)
		FName PlanetName;

	//Name of the Event that will activate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Event", Replicated)
		FName EventName;

	//The Value of how much fuel is used for travelling to the planet
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FuelCost", Replicated)
		float fFuelCost;

	//Whether it is active or not to be used
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IsActive")
		bool bisActive;

	//If it has been chosen once, it will be inable to be chosen again
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chosen")
		bool bisUsed;

	//The RoundManager that it is connected to so they can properly call to activate the level
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RoundManager")
		class ARoundManager* pRoundManager;

	//Text for the planet name
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NameText", Replicated)
		class UTextRenderComponent* pNameText;

	//Text for the Event that will trigger
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EventText", Replicated)
		UTextRenderComponent* pEventText;

	//Text for the Event that will trigger
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FuelText", Replicated)
		UTextRenderComponent* pFuelText;

public:
	/////ALL FUNCTIONS BELOW/////

	//List of all Gets
	UFUNCTION()
		float GetDifficultyValue() { return fDifficultyValue; }

	UFUNCTION()
		float GetRoundTime() { return fRoundTime; }

	UFUNCTION()
		FName GetPlanetName() { return PlanetName; }

	UFUNCTION()
		FName GetEventName() { return EventName; }

	UFUNCTION()
		float GetFuelCost() { return fFuelCost; }

	UFUNCTION()
		bool GetIsActive() { return bisActive; }
	UFUNCTION()
		void SetisActive(bool isactive) { bisActive = isactive; }

	UFUNCTION()
		bool GetisUsed() { return bisUsed; }
	UFUNCTION()
		void SetisUsed(bool isused) { bisUsed = isused; }

	UFUNCTION()
		void SetRoundManager(ARoundManager* roundmanager) { pRoundManager = roundmanager; }

	UFUNCTION()
		FName CreateadditionalText(FName event);

	//Activation and Deactivation for the objects
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Deactivate is used specifically for activating a level/round, when interacted by the player
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Disabling ensures the planet can be used again for the random level selection
	UFUNCTION()
		void Disable();
	UFUNCTION(Server, Reliable)
		void Server_Disable();

	//Sets the proper info for each text block
	UFUNCTION()
		void SetupPlanetText();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetupPlanetText();

	//Making the text visible and invisible when near the planet select
	UFUNCTION()
		void ActivateInfo();
	UFUNCTION(Server, Reliable)
		void Server_ActivateInfo();

	UFUNCTION()
		void DeactivateInfo();
	UFUNCTION(Server, Reliable)
		void Server_DeactivateInfo();
};
