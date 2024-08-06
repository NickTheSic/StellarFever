// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "OxygenSupplyEvent.generated.h"

/**
This event reduces the current oxygen and max oxygen on the player over time. To finish the event the player must move 3 spawned oxygensupplyactors into the 
object collider and then hold interact onto the final collider to stop the event. Event holds several lights as well to facilitate a UI, it will tell the player
how much longer they must hold interact with the object. Recommended to only have 1 of such event in the map as their are no proper ways to distinguish the 
spawned actors from each other and may(?) cause issues, the code has tried to take that into account but tests havent provided concrete proof.
 * 
 */
UCLASS()
class STELLARFEVER_API AOxygenSupplyEvent : public ABaseEvent
{
	GENERATED_BODY()

public:
    /////ALL VARIABLES BELOW/////

    //Object that will spawn when the oxygen event starts, assigned properly in blueprints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenObjects")
        TSubclassOf<class AOxygenSupplyActor> pOxygenObject;

    //The oxygen objects that will be activated or deactivated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenObjects")
        TArray<AOxygenSupplyActor*> pOxygenObjectItems;
	
    //Array of Spawnpoints on the map previously placed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectSpawnPoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //The Collider in which the objects will overlap with and disappear once within range
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectCollider")
        class UBoxComponent* pObjectCollider;

    //How fast the loweroxygen function will occur
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LowerOxygenValues")
        float fOxygenDecrementSpeed;

    //How much Oxygen will lower per timer value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LowerOxygenValues")
        float fOxygenDecrementValue;

    //Timer for when to call the loweroxygen function
    FTimerHandle OxygenDecrementTimer;

    //Counter for how many objects are brought to the collider
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iBroughtItems;

    //The mesh where the objects will be dropped off at
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenMeshs", Replicated)
        USkeletalMeshComponent* pDropOffMesh;

    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenMeshs", Replicated)
        USkeletalMeshComponent* pArmsMesh;

    //A List of all oxygen anims, 1 for looping when fixed, 3 for each arm replacing the filter
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenAnims")
        class UAnimationAsset* pLoopingAnim;

    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenAnims")
        UAnimationAsset* pFirstArmAnim;

    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenAnims")
        UAnimationAsset* pSecondArmAnim;

    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "OxygenAnims")
        UAnimationAsset* pThirdArmAnim;

    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OxygenAudio")
        UAudioComponent* pOxygenAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OxygenMusic")
        USoundCue* pOxygenMusic;

public:
    /////ALL FUNCTIONS BELOW/////
    AOxygenSupplyEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;

    //Will call the oxygen value on each player in the world to lower the values.
    UFUNCTION()
        virtual void LowerOxygen();

    //Creates all the oxygen objects to spawn
    UFUNCTION()
        void GenerateObjects();

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;

    //The function to play the animation on multicast
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_Animation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop);

    //Overlap for when the object is inside the objectcollider
    UFUNCTION()
        void OnColliderOverlap( UPrimitiveComponent* OverlappedComp, 
                                AActor* OtherActor, 
                                UPrimitiveComponent* OtherComp, 
                                int32 OtherBodyIndex, 
                                bool bFromSweep, 
                                const FHitResult& SweepResult);

};
