// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "BathroomEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABathroomEvent : public ABaseEvent
{
	GENERATED_BODY()

public:
    /////ALL VARIABLES BELOW/////

    //Where the janitor must be brought to these colliders
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectCollider")
        class UBoxComponent* pObjectCollider;

    //How much health will be lowered per timer tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementValue;

    //How fast the health timer will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementSpeed;

    //The timer to call the lower health function
    FTimerHandle HealthDecrementTimer;

    //How much oxygen will be lowered per timer tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementValue;

    //How fast the oxygen timer will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementSpeed;

    //The timer to call the lower oxygen function
    FTimerHandle OxygenDecrementTimer;

    //Counter for how many objects are brought to the collider
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iBroughtItems;

public:
    /////ALL FUNCTIONS BELOW/////
    ABathroomEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText(bool isactive);

    //Will call the oxygen value on each player in the world to lower the values.
    UFUNCTION()
        virtual void LowerOxygen();

    //Will be called by the timer to lower all players health in the game
    UFUNCTION()
        void LowerHealth();

    //Overlap for when the object is inside the objectcollider
    UFUNCTION()
        void OnColliderOverlap(UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);
};
