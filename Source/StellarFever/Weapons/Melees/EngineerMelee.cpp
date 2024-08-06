// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineerMelee.h"
#include "Components/SphereComponent.h"
#include "../../AI/AIEnemy.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/Animation/PlayerActionMontageComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"

AEngineerMelee::AEngineerMelee()
{
    pBurnArea = CreateDefaultSubobject<UStaticMeshComponent>("Burn Area");
    pBurnArea->SetCollisionProfileName("NoCollision");
    pBurnArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pBurnArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pBurnArea->SetupAttachment(RootComponent); //attach to mesh
    pBurnArea->OnComponentBeginOverlap.AddDynamic(this, &AEngineerMelee::OnOverlapBegin);
    pBurnArea->OnComponentEndOverlap.AddDynamic(this, &AEngineerMelee::OnOverlapEnd);
    pBurnArea->SetHiddenInGame(true);

    bIsBurning = false;

    fBurnDamage = 15.0f;
    fBurnDuration = 4.0f;
    fBurnHitsPerSecond = 2.0f;

    fCooldownTime = 40.0f;
    fBurnCooldown = 3.0f;
}

void AEngineerMelee::AltAttackStart()
{
    //if active and not attacking and not cooling down
    if (bIsActive == true && bIsAttacking == false && bIsCoolingDown == false)
    {
        //alt attack
        AltAttack();
        pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type, true);
    }
}

void AEngineerMelee::AltAttack()
{
    FVector loc;
    FRotator dir;
    pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);

    NetMC_PlaySound(pAltFireSound);
    NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pBurnArea->GetComponentLocation() + FVector(0, 0, -20.0f), FVector(2.0f, 0.5f, 1.0f), dir);
    //turns on blowtorch (change a bool)
    bIsBurning = true;
    //start cooling down
    bIsCoolingDown = true;
    //alt attacking (necessary so that we can't melee)
    bIsAltAttacking = true;
    //setup timer to disable blowtorch 
    GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &AEngineerMelee::EndBurn, fBurnCooldown, false);
    //begin overlapping enemies
    pBurnArea->SetCollisionProfileName("OverlapAll");
    pBurnArea->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
    pBurnArea->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
    //Check overlapped enemies
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEngineerMelee::IgniteCheck, 1.0f / fBurnHitsPerSecond + 0.01f, true);
    //lock inventory
    pOwnerRef->SetCanAccessInventory(false);
}

void AEngineerMelee::AltAttackStop()
{
    //nothing happens when we release m2
}

void AEngineerMelee::OnOverlapBegin(
     UPrimitiveComponent* OverlappedComp,
     AActor* OtherActor,
     UPrimitiveComponent* OtherComp,
     int32 OtherBodyIndex,
     bool bFromSweep,
     const FHitResult& SweepResult)
{
    //if bIsBurning AND we overlapped an enemy
    if (bIsBurning && OtherActor->ActorHasTag("Enemy"))
    {
        //cast enemy
        AAIEnemy* enemy = static_cast<AAIEnemy*>(OtherActor);
        //if enemy exists
        if (enemy != nullptr)
        {
            //add to check array
            CheckArray.Add(enemy);

            FHitResult hit;
            //if we don't hit geometry, ignite enemy
            if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), enemy->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
            {
                //ignite
                enemy->InitiateBurnDamage(pOwnerRef, fBurnDamage, fBurnDuration, fBurnHitsPerSecond);
                BurnArray.Add(enemy);
            }
        }
    }
}

void AEngineerMelee::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (OtherActor->ActorHasTag("Enemy"))
    {
        //cast enemy
        AAIEnemy* enemy = static_cast<AAIEnemy*>(OtherActor);
        //if enemy exists
        if (CheckArray.Contains(enemy))
        {
            CheckArray.Remove(enemy);
        }
        if (BurnArray.Contains(enemy))
        {
            BurnArray.Remove(enemy);
        }
    }
}

void AEngineerMelee::EndBurn()
{
    //stop fire audio
    pAudioComponent->Stop();
    //no longer alt attacking
    bIsAltAttacking = false;
    //clear our burn
    bIsBurning = false;
    GetWorld()->GetTimerManager().ClearTimer(BurnTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    //stop overlapping 
    pBurnArea->SetCollisionProfileName("NoCollision");
    pBurnArea->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
    pBurnArea->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
    //empty check array
    CheckArray.Empty();
    BurnArray.Empty();
    //setup timer to reset cooldown
    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AMeleeBase::SetIsCooledDown, fCooldownTime, false);
    //unlock inventory
    pOwnerRef->SetCanAccessInventory(true);
}

void AEngineerMelee::IgniteCheck()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "IGNITE CHECK");
    for (auto i : CheckArray)
    {
        if (!BurnArray.Contains(i))
        {
            FHitResult hit;
            //if we can raycast against the enemy, ignite them
			if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), i->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel1))
			{   //if we hit the enemy, ignite them
				if (hit.GetActor()->GetName() == i->GetName())
				{
					//ignite
					i->InitiateBurnDamage(pOwnerRef, fBurnDamage, fBurnDuration, fBurnHitsPerSecond);
				}
				//if we hit the shield, ignite the enemy
				else if (hit.GetActor()->ActorHasTag("Shield"))
				{
					//ignite
					i->InitiateBurnDamage(pOwnerRef, fBurnDamage, fBurnDuration, fBurnHitsPerSecond);
				}
			}
        }
    }
}