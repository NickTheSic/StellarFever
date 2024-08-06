// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "LifeSupportEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ALifeSupportEvent : public ABaseEvent
{
	GENERATED_BODY()
	

public:
    /////ALL VARIABLES BELOW/////

    //Where the medbayobjects must be brought to these colliders
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectCollider")
        class UBoxComponent* pObjectCollider;

    //The base class used to spawn the medbay objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MedbayObjects")
        TSubclassOf<class AMedbayObject> pMedbayObject;

    //The array that holds all the medbay objects to be activated and deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MedbayObjects")
        AMedbayObject* pMedbayObjectItem;

    //An array of spawn points to generate the shell objects, assigned in blueprints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnpoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //Event Objects that are spawned and used to deactivate the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObject", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //How much health will be lowered per timer tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementValue;

    //How fast the health timer will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementSpeed;

    //The timer to call the lower health function
    FTimerHandle HealthDecrementTimer;

    //Keeps track of all the objects brought to the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iBroughtItems;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //Object that the items will be brought to
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "SupportMesh")
        USkeletalMeshComponent* pDropOffMesh;

    //The med arm when it is returned and active
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "SupportMesh")
        UStaticMeshComponent* pFixedDropOffMesh;

    //Will need a pointer to medbay object eventually

        ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LifeSupportAudio")
        UAudioComponent* pLifeSupportAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LifeSupportMusic")
        USoundCue* pLifeSupportMusic;

public:
    /////ALL FUNCTIONS BELOW/////

    ALifeSupportEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;

    //Used to return when an eventobject is interacted with unique function in each event that has a eventobject
    UFUNCTION()
        virtual void EventObjectComplete() override;
        virtual void Server_EventObjectComplete_Implementation() override;

    //Creates a pool of the object items
    UFUNCTION()
        void GenerateObjects();

    //Will be called by the timer to lower all players health in the game
    UFUNCTION()
        void LowerHealth();

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;

    //Overlap for when the object is inside the objectcollider
    UFUNCTION()
        void OnColliderOverlap(UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

};
