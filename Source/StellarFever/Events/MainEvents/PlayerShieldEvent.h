// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "PlayerShieldEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API APlayerShieldEvent : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //Event Objects that are spawned and used to deactivate the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObject", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //How much shield will be lowered per timer tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShieldValues")
        float fShieldDecrementValue;

    //How fast the shield timer will trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShieldValues")
        float fShieldDecrementSpeed;

    //The timer to call the lower shield function
    FTimerHandle ShieldDecrementTimer;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShieldAudio")
        UAudioComponent* pShieldAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShieldMusic")
        USoundCue* pShieldMusic;

public:
    /////ALL FUNCTIONS BELOW/////

    APlayerShieldEvent();

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

    //Will be called by the timer to lower all players shield in the game
    UFUNCTION()
        void LowerShield();

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;
};
