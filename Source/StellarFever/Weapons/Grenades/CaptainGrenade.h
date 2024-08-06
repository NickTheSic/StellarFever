// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GrenadeBase.h"
#include "CaptainGrenade.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ACaptainGrenade : public AGrenadeBase
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats", meta = (AllowPrivateAccess = "true"))
	    float fBurnDamage;
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats", meta = (AllowPrivateAccess = "true"))
	    float fBurnDuration;
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats", meta = (AllowPrivateAccess = "true"))
	    float fBurnHitsPerSecond;
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats", meta = (AllowPrivateAccess = "true"))
        float fGrenadeDuration;
    bool bHasFinishedBurning;

    class UAudioComponent* pExplodeAudioComponent;

    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
        class USoundCue* pBurningSound;

    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
    TArray<class AAIEnemy*> CheckArray;     //enemies to raycast against every x seconds
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
    TArray<class AAIEnemy*> BigFireArray;   //enemies within line of sight of the grenade
    UPROPERTY(EditAnywhere, Category = "Captain Grenade Stats")
    TArray<class AAIEnemy*> SmallFireArray; //enemies not in line of sight of the grenade

    FTimerHandle RaycastTimer;

    virtual void BeginPlay() override;
public:
	ACaptainGrenade();

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
        void OnOverlapEnd(
            UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);

    UFUNCTION()
        virtual void OnCompHit(
            UPrimitiveComponent* HitComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            FVector NormalImpulse,
            const FHitResult& Hit);

    virtual void AttackStart() override;
    virtual void Attack() override;
    virtual void AttackStop() override;

    virtual void Explode() override;
    void ExplodeEnd();

    virtual void GiveGrenade() override;

    virtual void NetMC_Equip() override;
    virtual void NetMC_UnEquip() override;

    UFUNCTION(NetMulticast, Reliable)
    void NetMC_PlayExplodeSound();

    //virtual void ExplosionRaycast(TArray<AActor*> enemies) override;
    void RaycastCheck();
};
