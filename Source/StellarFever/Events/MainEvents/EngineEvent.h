// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "EngineEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEngineEvent : public ABaseEvent
{
	GENERATED_BODY()
	

public:
    /////ALL VARIABLES BELOW/////

    //Overlap Collider that will delete the objects brought to it
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverlapComponent")
        UBoxComponent* pObjectCollider;

    //The reference to the engine object that will spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineObject")
        TSubclassOf<class AEngineObject> pEngineObject;

    //The array of engine objects that are activated and deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineObject")
        AEngineObject* pEngineObjectItem;

    //Event Objects that are spawned and used to deactivate the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObject", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //Spawnpoint where the engine object will spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnPoint")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //Timer to activate the minor events
    FTimerHandle MinorEventTimer;

    //Speed of the minor event timer
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minoreventspeed")
        float fMinorEventSpeed;

    //Keeps track of all the objects brought to the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iBroughtItems;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //The mesh where players will drop off the objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineMeshs", Replicated)
        USkeletalMeshComponent* pDropOffMesh;

    //The spinning mesh to show the engine is online
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineMeshs", Replicated)
        USkeletalMeshComponent* pInnerMesh;

    //The spinning mesh to show the engine is online
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineMeshs", Replicated)
        USkeletalMeshComponent* pCenterMesh;

    //The spinning mesh to show the engine is online
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineMeshs", Replicated)
        USkeletalMeshComponent* pOuterMesh;

    //The list of anims for the drop off mesh, one for opening when close, one for closing when leaving and no item is brought
    //and a thrid for closing with the engine piece
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnims")
        class UAnimationAsset* pApproachAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnims")
        UAnimationAsset* pIdleOpenAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnims")
        UAnimationAsset* pLeaveAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnims")
        UAnimationAsset* pItemAnim;

    //The animation for the activemesh, when the engine is deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnim")
        UAnimationAsset* pInnerAnim;

    //The animation for the activemesh, when the engine is deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnim")
        UAnimationAsset* pCenterAnim;

    //The animation for the activemesh, when the engine is deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EngineAnim")
        UAnimationAsset* pOuterAnim;

    //Used to detect if the other engine is active or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OtherEngine")
        AEngineEvent* pOtherEngine;

    //Timer to play anims at proper times
    FTimerHandle AnimTimer;

    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EngineAudio")
        UAudioComponent* pEngineAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EngineMusic")
        USoundCue* pEngineMusic;

    //Soundcue for the double track
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EngineMusic")
        USoundCue* pBothEngineMusic;


public:
    /////ALL FUNCTIONS BELOW/////

    AEngineEvent();

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

    //Will activate a random minor event in the world
    UFUNCTION()
        void TriggerEvent();

    //Creates a pool of the object items
    UFUNCTION()
        void GenerateObjects();

    //The function to play the animation on multicast
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_Animation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop);

    //both functions below to be called by timers
    UFUNCTION()
        void PlayIdleAnim();

    UFUNCTION()
        void PlayInsertionAnim();

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText(bool isactive);

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

    //Collision to see if a player leaves the collider
    UFUNCTION()
        void OnColliderExit(class UPrimitiveComponent* OverlappedComp,
            class AActor* OtherActor,
            class UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);
};
