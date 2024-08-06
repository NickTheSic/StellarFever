// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "RoomHealthEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ARoomHealthEvent : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //When player is within it they will trigger the oncollideroverlap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerCollider")
        class UBoxComponent* pPlayerCollider;

    //Listed of players that will be affected by the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affected Players")
        TArray<ABasePlayer*> pAffectedPlayers;

    //How fast health will lower when the event is active
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementSpeed;

    //How much health will be lost on lowerhealth call
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValues")
        float fHealthDecrementValue;

    //The timer for lowering health
    FTimerHandle HealthDecrementTimer;

public:
    /////ALL FUNCTIONS BELOW/////

    ARoomHealthEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;

    UFUNCTION()
        virtual void Deactivate() override;

    //Called by the timer to lower any colliding players health
    UFUNCTION()
        void LowerHealth();

    //Collision call to find players colliding with it
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
