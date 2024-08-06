// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GrenadeBase.h"
#include "MedicGrenade.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMedicGrenade : public AGrenadeBase
{
	GENERATED_BODY()
	
protected:
    bool bHasExplodedTwice;
    bool bHasAttached;
    virtual void BeginPlay() override;

public:
	AMedicGrenade();

    virtual void AttackStart() override;
    virtual void Attack() override;
    virtual void AttackStop() override;

    UFUNCTION(Server, Reliable)
    void PreExplode(AActor* OtherActor);

	virtual void Explode() override;
    virtual void ResetVars() override;

    virtual void NetMC_Equip() override;
    virtual void NetMC_UnEquip() override;

    virtual void ExplosionRaycast(TArray<class AActor*> enemies) override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};
