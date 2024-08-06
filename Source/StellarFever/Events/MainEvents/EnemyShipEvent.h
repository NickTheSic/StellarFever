// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "EnemyShipEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEnemyShipEvent : public ABaseEvent
{
	GENERATED_BODY()

public:
    /////ALL VARIABLES BELOW/////

    //The object that will spawn when event starts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShellObjects")
        TSubclassOf<class AShellObject> pShellObject;

    //Array of ShellObjects that will spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShellObjects")
        TArray<AShellObject*> pShellObjectItems;

    //Where the shellobjects must be brought to these colliders
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectCollider")
        class UBoxComponent* pObjectColliderLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectCollider")
        UBoxComponent* pObjectColliderRight;

    //The BoardingEvent that is linked to this event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoardingEvent")
        class ABoardingEvent* pBoardingEvent;

    //An array of spawn points to generate the shell objects, assigned in blueprints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnpoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //A Timer that is called once every so often to and is set by the event speed float to how often they will occur
    FTimerHandle MinorEventTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minor event speed")
        float fMinorEventSpeed;

    //A Timer that is called to deactivate this event, and starts the boarding event if successful.
    FTimerHandle BoardingTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boarding event speed")
        float fBoardingEventSpeed;

    //Counter for how many objects are brought to the collider
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BroughtItems", Replicated)
        int iBroughtItems;

    //Objects where the items will be dropped off
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons", Replicated)
        class USkeletalMeshComponent* pDropOffMeshLeft;

    //Collision for the left cannon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons")
        class UStaticMeshComponent* pCollisionLeft;

    //Objects where the items will be dropped off
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons", Replicated)
         USkeletalMeshComponent* pDropOffMeshRight;

    //Collision for the right cannon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons")
        class UStaticMeshComponent* pCollisionRight;

    //The animation that plays on the skeletal mesh
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireAnimation")
        class UAnimationAsset* pFireAnim;

    //The audio that will play when the cannon fires
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "FireSound")
        class USoundBase* pFireSound;

    ///// FOR AUDIO /////

//Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyShipAudio")
        UAudioComponent* pEnemyShipAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyShipMusic")
        USoundCue* pEnemyShipMusic;


public:
    /////ALL FUNCTIONS BELOW/////

    AEnemyShipEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
    virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
    virtual void Server_Deactivate_Implementation() override;

    UFUNCTION(Server, Reliable)
        void Server_CreateVisuals();

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText();

    //The function to play the animation on multicast
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_Animation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop);

    //Will grab the eventmanager and activates a minor event on it, TODO: Add functionality to the function
    UFUNCTION()
        void TriggerEvent();

    //Starts the Boarding, circumventiving the object after a certain time limit, if already active will spawn AI again
    UFUNCTION()
        void TriggerBoarding();

    //Creates all the shell objects to spawn
    UFUNCTION()
        void GenerateObjects();

    //Only way to confirm that 3 shells were brought to the event.
    UFUNCTION()
        int GetBroughtItems() { return iBroughtItems; }

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;

    //Overlap for when the object is inside the left objectcollider
    UFUNCTION()
        void OnColliderOverlapLeft(UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

    //Overlap for when the object is inside the right objectcollider
    UFUNCTION()
        void OnColliderOverlapRight(UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

};
