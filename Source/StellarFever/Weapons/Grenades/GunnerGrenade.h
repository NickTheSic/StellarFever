// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GrenadeBase.h"
#include "../../Interfaces/RaycastNotifyable.h"
#include "GunnerGrenade.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGunnerGrenade : public AGrenadeBase, public IRaycastNotifyable
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditAnywhere, Category = "Gunner Grenade Stats", meta = (AllowPrivateAccess = "true"))
        uint8 ucAmmoRefillPercentage;
    UPROPERTY(EditAnywhere, Category = "Gunner Grenade Stats", meta = (AllowPrivateAccess = "true"))
	    uint8 ucRefillUses;

    bool bCanRefill;

    uint8 ucTimesExploded;

    UPROPERTY(EditAnywhere, Category = "Gunner Grenade Stats")
        class USoundCue* pLandSound;
    UPROPERTY(EditAnywhere, Category = "Gunner Grenade Stats")
        class USoundCue* pUnfoldSound;
    UPROPERTY(EditAnywhere, Category = "Gunner Grenade Stats")
        class USoundCue* pAmmoGrabSound;

    virtual void BeginPlay() override;

public:
	AGunnerGrenade();

    using AActor::NotifyHit;
	virtual void NotifyHit(AActor* instigator, const FHitResult* hit) override;

    virtual void NetMC_Equip() override;
    virtual void NetMC_UnEquip() override;

    virtual void Explode() override;

    virtual void AttackStart() override;
    virtual void Attack() override;
    virtual void AttackStop() override;

    virtual void ExplosionRaycast(TArray<class AActor*> enemies) override;

    UFUNCTION()
        virtual void OnCompHit(
            UPrimitiveComponent* HitComp, 
            AActor* OtherActor, 
            UPrimitiveComponent* OtherComp, 
            FVector NormalImpulse, 
            const FHitResult& Hit);

    UFUNCTION()
        virtual void OnProjectileStop(const FHitResult& hit);

    virtual void ResetVars() override;
};
