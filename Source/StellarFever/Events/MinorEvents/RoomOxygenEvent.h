// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "RoomOxygenEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ARoomOxygenEvent : public ABaseEvent
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

    //How fast oxygen will lower when the event is active
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementSpeed;

    //How much oxygen will be lost on loweroxygen call
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OxygenValues")
        float fOxygenDecrementValue;

    //The timer for lowering oxygen
    FTimerHandle OxygenDecrementTimer;

    //How fast the max oxygen value will lower whe nthe event is active
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxOxygenValues")
        float fMaxOxygenDecrementSpeed;

    //How much oxygen will be lost on the lowermaxoxygen call
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxOxygenValues")
        float fMaxOxygenDecrementValue;

    //The timer for lowering max oxygen
    FTimerHandle MaxOxygenDecrementTimer;

public:
    /////ALL FUNCTIONS BELOW/////

    ARoomOxygenEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;

    UFUNCTION()
        virtual void Deactivate() override;

    //Called by the timer to lower any colliding players oxygen
    UFUNCTION()
        void LowerOxygen();

    //Called during the events lifetime, lowers all players max oxygen value
    UFUNCTION()
        void LowerMaxOxygen();

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
