// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventsManager.h"
#include "EnemyPool.h"
#include "../Player/BasePlayer.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextRenderComponent.h"
#include "../AI/BaseAICharacter.h"
#include "../AI/AIEnemy.h"
#include "../StellarFeverGameInstance.h"
#include "../StellarFeverGameState.h"
#include "../Miscellaneous/ShipState.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "BaseEvent.generated.h"

UCLASS()
class STELLARFEVER_API ABaseEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:

	//All Varibales Below
	
	//Base interaction that MUST be interacted with to end the event, sometimes not active unless other parameters are met
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainCollider")
		UBoxComponent* pFinalCollider;

	//The collider for AI to interact with and if they leave the collider it will stop the timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AICollider")
		UBoxComponent* pAICollider;

	//The Primary Mesh which will give the player a visual to interact with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableMesh")
		UStaticMeshComponent* pInteractableMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
		float fDistanceForRenderOutline = 5000.f;

	//The mesh that will show which state the event is currenly in, Green = good, red = bad
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusMesh", Replicated)
		UStaticMeshComponent* pStatusMesh;

	//The indicators of what is left to interact with/which objects are missing on the event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusIndicators", Replicated)
		TArray<UStaticMeshComponent*> pStatusIndicators;

	//The Prime varible that tells if an event is activated or not, is not meant ot be changed outisde of the Activate and Deactive functions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activated", Replicated)
		bool bIsActive;

	//The main variable for events that determines if the player can deactivate the event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CanDeactivate", Replicated)
		bool bCanDeactivate;

	//The bool that checks if the event has already activated so it cant be activated more than once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HasActivated", Replicated)
		bool bHasActivated;

	//A list of players found at the start so each event can refer to the players if needed for changing values within baseplayer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListOfPlayers")
		TArray<ABasePlayer*> pPlayers;

	//A Call back to the EventManager if needed to interact with the GameMode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventManager")
		class UEventsManager* pEventsManager;

	//Material for a green glowing texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterial* pGreenMaterial;

	//Material for a red glowing texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterial* pRedMaterial;

	//The text that will appear on the event to notify which event it is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TitleText")
		UTextRenderComponent* pTitleText;

	//Optional text used to additional information for certain events and variables.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdditionalText", Replicated)
		UTextRenderComponent* pAdditionalText;

	//The Timer for when the AI is in the collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AITimer")
		FTimerHandle AICollisionTimer;

	//The Timers length set up in blueprints, hard value set in code
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AITimer")
		float fAITimerLength = 30.0f;

	//AI that are currently within the collision box of the event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractingAI")
		TArray<ABaseAICharacter*> pInteractingAI;

	//A link to the Ship state so that we can alter the currency
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shipstate")
		class AShipState* pShipState;

	//a check for the teleporter to see if it should glow or not
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "CanTeleport", Replicated)
		bool bForTeleporter;

	//a check for the stencil to glow
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "stenciltexture", Replicated)
		bool bForStencil;

	//Audio for the event to use when preforming activities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class UAudioComponent* pAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventSound")
		class USoundCue* pActivationSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventSound")
		class USoundCue* pDeactivateSound;

public:

	//All Functions Below

	//Will set the event to be active, and will activate the main collider and set the bool to true, specific events will call this function and preform their own
	//specific implementation.
		virtual void Activate();
		UFUNCTION(Server, Reliable)
			virtual void Server_Activate();

	//Will set the event to be inactive, and will deactivate the main collider and set the bool to false, specific events will call this function and preform
	//their own specific implementation.
		virtual void Deactivate();
		UFUNCTION(Server, Reliable)
			virtual void Server_Deactivate();

	//Used by minigames in events that if the player interacts with the wrong objects 
		virtual void Failure();

	//Used to return when an eventobject is interacted with unique function in each event that has a eventobject
		virtual void EventObjectComplete();
		UFUNCTION(Server, Reliable)
			virtual void Server_EventObjectComplete();

	//Sets up the player pointer for the event, must be done outside of beginplay
		void SetupPlayerPointer();

	//Sets the eventmanager which will be called in the manager itself
	UFUNCTION()
		void SetEventsManager(class UEventsManager* manager) { if (GetLocalRole() == ROLE_Authority) { pEventsManager = manager; } }

	//Returns if the event is active or not
	UFUNCTION()
		bool GetIsActive() { return bIsActive; }

	//Networked function for activating and changing material colors
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Activate();

	//Networked function for deactivating and changing material colors
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Deactivate();

	//So the server calls the sound, and only plays once but broadcast to everyone
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PlaySound(USoundBase* pSound);

	//Used to play the specific music in the level
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_PlayMusic();

	//Used to stop the specific music in the level
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_StopMusic();

	//Ensures material changes are made on a multicast call
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_ChangeStatus(uint8 val);

	//Resets the event so it can be activated again
	UFUNCTION()
		void ResetActivation();

	//Gets the value to see if it has activated
	UFUNCTION()
		bool GetHasActivated() { return bHasActivated; }

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnAIOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	//Collision to see if a player leaves the collider
	UFUNCTION()
		void OnAIExit(class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
};
