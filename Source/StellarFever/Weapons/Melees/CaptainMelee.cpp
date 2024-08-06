// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptainMelee.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/CaptainPlayer.h"
#include "../../AI/AIEnemy.h"
#include "../../Components/RaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Player/Animation/PlayerActionMontageComponent.h"

ACaptainMelee::ACaptainMelee()
{
    fDamage = 25.0f;
    fRageDamage = 70.0f;
    fRageDuration = 20.0f;
    fStunChance = 50.0f;
    fRageStunChance = 80.0f;

    fCooldownTime = 40.0f;

    bIsRaging = false;
}

void ACaptainMelee::AttackStart()
{
    if (bIsActive == true && bIsAttacking == false)
    {
        //Super::Attack();
        pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type);

        //When we left click we attack after our attackRate
        bIsAttacking = true;
        NetMC_PlaySound(pMissSound);
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeBase::Attack, fAttackRate, false);
        //Attack();
        //if not already alt attacking, disable inventory
        if (bIsAltAttacking == false)
            pOwnerRef->SetCanAccessInventory(false);
    }
}

void ACaptainMelee::Attack()
{
    //raycast to hit
    FHitResult hit;
    pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayerT<AAIEnemy>(hit, fWeaponRange, ECollisionChannel::ECC_Pawn, false);
    
    //if raging
    if (bIsRaging)
    {
        //if we hit (using rage damage)
        if (DealDamage(&hit, fRageDamage))
        {
            NetMC_PlaySound(pHitSound);
            //see if we stun
            if (FMath::FRandRange(0.0f, 100.0f) < fRageStunChance)
            {
                //stun enemy
                AAIEnemy* enemy = static_cast<AAIEnemy*>(hit.GetActor());
                if (enemy != nullptr)
                {
                    enemy->Stunned();
                }
            }
        }
    }
    else
    {
        //if we hit (using normal damage)
        if (DealDamage(&hit, fDamage))
        {
            NetMC_PlaySound(pHitSound);
            //see if we stun
            if (FMath::FRandRange(0.0f, 100.0f) < fStunChance)
            {
                //stun enemy
                AAIEnemy* enemy = static_cast<AAIEnemy*>(hit.GetActor());
                if (enemy != nullptr)
                {
                    enemy->Stunned();
                }
            }
        }
    }

    //show hit line
    DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Green, false, 0.5f);
    //we are finished attacking
    AttackCompleted();
}

void ACaptainMelee::AttackStop()
{
    //nothing happens when we release left click
}

void ACaptainMelee::AltAttackStart()
{
    //if active and not attacking and not raging and not cooling down
    if (bIsActive == true && bIsAttacking == false && bIsRaging == false && bIsCoolingDown == false)
    {
        bIsAltAttacking = true;
        //alt attack
        AltAttack();
    }
}

void ACaptainMelee::AltAttack()
{
    pOwnerRef->GetPlayerSoundComponent()->PlayAltMelee();

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "RAGE TIME");
    //alt attack changes a bIsRaging bool which will apply rage
    bIsRaging = true;

    //cast player and call start rage mode
    static_cast<ACaptainPlayer*>(pOwnerRef)->StartRageMode();
    //Lock to melee, increase movement speed, increase shield recharge

    //setup timer to end rage mode
    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ACaptainMelee::EndRage, fRageDuration, false);
}

void ACaptainMelee::AltAttackStop()
{
    //nothing happens when we release M2
}

void ACaptainMelee::EndRage()
{
    //cast player and call end rage mode
    static_cast<ACaptainPlayer*>(pOwnerRef)->EndRageMode();
    
    //rage ended
    bIsAltAttacking = false;
    bIsRaging = false;
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "RAGE ENDED");
    GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);

    //start cooling down
    bIsCoolingDown = true;
    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AMeleeBase::SetIsCooledDown, fCooldownTime, false);
}