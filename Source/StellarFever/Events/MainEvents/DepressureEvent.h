// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "DepressureEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ADepressureEvent : public ABaseEvent
{
	GENERATED_BODY()

public:
    /////ALL VARIABLES BELOW/////
    
    //The Objects on the map that must be activated before the event ends, there is no main event to activate but a light will still show it is inactive
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObjects", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //Both Min and Max offsets for the bullet spread
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimOffSetVals")
        float fAimOffSetMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimOffSetVals")
        float fAimOffSetMax;

    //Timer for when to turn on and off the mainevents
    FTimerHandle MainEventTimer;

    //Time it takes to break a main event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeForEvent")
        float fMainEventSpeed;

    //Bonus time between button presses
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeForEvent")
        float fBonusEventSpeed;

    //Timer for the minor events
    FTimerHandle MinorEventTimer;

    //Time it takes to trigger a minor event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeForEvent")
        float fMinorEventSpeed;


public:
    /////ALL FUNCTIONS BELOW/////

    ADepressureEvent();

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

    //Lowers the effects of the depressurization for each button clicked
    UFUNCTION()
        void LowerGravityScale();

    //Breaks main events after a long time of no action, would recommend to do so after about a minute or 2
    UFUNCTION()
        void BreakMainEvent();

    //Triggers a minor event when the timer is done
    UFUNCTION()
        void TriggerMinorEvent();
};
