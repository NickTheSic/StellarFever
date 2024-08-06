// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "PowerEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API APowerEvent : public ABaseEvent
{
	GENERATED_BODY()
	

public:
    /////ALL VARIABLES BELOW/////

    //Overlap Collider that will delete the objects brought to it
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverlapComponent")
        UBoxComponent* pObjectCollider;

    //Keeps track of all the objects brought to the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
    int iBroughtItems;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //Spawnpoints for the batteryobjects to spawn in
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnpoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //Reference to the spawned battery on the map
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BatteryObject")
        TSubclassOf<class ABatteryObject> pBatteryObject;

    //The spawned battery objects that will be activated an deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BatteryObject")
        TArray<ABatteryObject*> pBatteryObjectItems;

    //Event Objects that are spawned and used to deactivate the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObject", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //Meshs for when the objects are brought to the proper location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fancy Meshs", Replicated)
        TArray<USkeletalMeshComponent*> pBatteryMesh;

    //The animation for the objects when they are brought to the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerAnim")
        class UAnimationAsset* pBatteryAnim;

    //The anim once a battery is looping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerAnim")
        UAnimationAsset* pLoopingAnim;

    //The animation for the drop off meshs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerAnim")
        UAnimationAsset* pMeshAnim;

    //The mesh that shows where to drop off the items
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fancy Meshs")
        USkeletalMeshComponent* pDropOffMeshLeft;

    //The mesh that shows where to drop off the items
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fancy Meshs")
        USkeletalMeshComponent* pDropOffMeshRight;

    //Timers for both orbs to start loop spins
    FTimerHandle FirstBatteryTimer;
    FTimerHandle SecondBatteryTimer;

    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PowerAudio")
        UAudioComponent* pPowerAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PowerMusic")
        USoundCue* pPowerMusic;


public:
    /////ALL FUNCTIONS BELOW/////

    APowerEvent();

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

    //Timers call these funtions to loop anims
    UFUNCTION()
        void PlayFirstBatteryLoop();
    UFUNCTION()
        void PlaySecondBatteryLoop();

    //The function to play the animation on multicast
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_Animation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop);

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
