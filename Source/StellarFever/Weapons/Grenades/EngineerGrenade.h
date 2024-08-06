// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GrenadeBase.h"
#include "EngineerGrenade.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEngineerGrenade : public AGrenadeBase
{
	GENERATED_BODY()
	
protected:

    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats", meta = (AllowPrivateAccess = "true"))
	    float fSentryDamage;
    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats", meta = (AllowPrivateAccess = "true"))
        float fSentryRange;

    bool bIsSentry;

    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats", meta = (AllowPrivateAccess = "true"))
        float fSentryJamTime;
    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats", meta = (AllowPrivateAccess = "true"))
        float fSentryExplodeTime;
    FTimerHandle SentryJamTimer;

    //Tracer target name
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gun Stats")
        FName TracerTargetName;
    //Tracer effect
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun Stats")
        class UParticleSystem* pTracerEffect;

    //need animations before we can use this
    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats")
        class USoundCue* pSentryBuildSound;
    //need a timer to play this sound every X amount of time
    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats")
        class USoundCue* pSentryIdleSound;
    UPROPERTY(EditAnywhere, Category = "Engineer Grenade Stats")
        class USoundCue* pSentryShootSound;

    virtual void BeginPlay() override;

public:
	
	AEngineerGrenade();

    virtual void AttackStart() override;
    virtual void Attack() override;
    virtual void AttackStop() override;

    virtual void NetMC_Equip() override;
    virtual void NetMC_UnEquip() override;

    virtual void Explode() override;

    virtual void ResetVars() override;

    virtual void GiveGrenade() override;

    void SentryAttack();

    void SentryJam();

    virtual void ExplosionRaycast(TArray<class AActor*> enemies) override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION(NetMulticast, Reliable)
        void NetMC_ShowTracer(AActor* target);

};
