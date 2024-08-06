// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "GreenhouseEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGreenhouseEvent : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //Event Objects that are spawned and used to deactivate the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObject", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //How much oxygen will be lowered per timer tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementValue;

    //How fast the oxygen timer will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementSpeed;

    //The timer to call the lower oxygen function
    FTimerHandle OxygenDecrementTimer;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

public:
    /////ALL FUNCTIONS BELOW/////

    AGreenhouseEvent();

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

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText(bool isactive);

    //Will call the oxygen value on each player in the world to lower the values.
    UFUNCTION()
        virtual void LowerOxygen();
};
