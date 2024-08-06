// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "BridgeEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABridgeEvent : public ABaseEvent
{
	GENERATED_BODY()
	

public:
    /////ALL VARIABLES BELOW/////

    //List of all eventobjects that need to be interacted with to stop event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable objects", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //Timer to start minor events when actives
    FTimerHandle MinorEventTimer;

    //Speed of when a minor event will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinorEventSpeed")
        float fMinorEventSpeed;

    //Value of how much the screen will shake when timer ends
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenShakeValue")
        float fScreenShakeValue;

    //Timer to shake the screen
    FTimerHandle ScreenShakeTimer;

    //Speed of how often the screen will shake
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenShakeSpeed")
        float fScreenShakeSpeed;

    //Timer to deactivate the event if the players aren't fast enough
    FTimerHandle MainEventTimer;

    //Speed of how long a player has to deactivate the event objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEventSpeed")
        float fMainEventSpeed;

    //The time added once any event object is interacted with
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEventSpeed")
        float fBonusSpeed;

    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BridgeAudio")
        UAudioComponent* pBridgeAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BridgeMusic")
        USoundCue* pBridgeMusic;


public:
    /////ALL FUNCTIONS BELOW/////

    ABridgeEvent();

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

    //Will activate a random main event in the world
    UFUNCTION()
        void TriggerMainEvent();

    //Will shake the screen when called
    UFUNCTION()
        void TriggerScreenShake();

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText(bool isactive);

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;

    //Resets the eventobjects status if it is not deactivated fast enough
    UFUNCTION()
        void ResetEvent();
    UFUNCTION(Server, Reliable)
        void Server_ResetEvent();
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ResetEvent();

};
