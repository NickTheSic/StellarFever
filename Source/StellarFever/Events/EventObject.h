// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "EventObject.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEventObject : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //The owner of this eventobject
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
        ABaseEvent* pEventOwner;

    //The skeletal mesh for the animated button
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "MainButton", Replicated)
        USkeletalMeshComponent* pButtonMesh;

    //The anim for when the button is pressed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonAnim")
        class UAnimationAsset* pButtonAnim;

    //Used for minigame event objects, if this event isnt valid, it will call a seperate function in CallComplete
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valid", Replicated)
        bool bIsUseable;

public:
    /////ALL FUNCTIONS BELOW/////

    AEventObject();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;
        virtual void Multicast_Activate_Implementation() override;
    
    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;
        virtual void Multicast_Deactivate_Implementation() override;

    //The function to play the animation on multicast
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_Animation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop);

    //Used for minigame calls, called if interacted with wrong object
    UFUNCTION()
        virtual void Failure() override;

    //Will call back to the owner of this object to send information that it is interacted with
    UFUNCTION()
        void CallComplete();

    //Sets the owner of the eventobject
    void SetEventOwner(ABaseEvent* pEvent) { pEventOwner = pEvent; }

    //Sets is valid, used by minigame events
    void SetIsValid(bool isValid) { bIsUseable = isValid; }

    //Gets the bIsValid value
    bool GetIsValid() { return bIsUseable; }
};
