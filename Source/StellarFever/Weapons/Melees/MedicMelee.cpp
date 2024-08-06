// Fill out your copyright notice in the Description page of Project Settings.


#include "MedicMelee.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/Animation/PlayerActionMontageComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../AI/AIEnemy.h"
#include "../../Components/RaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

AMedicMelee::AMedicMelee()
{
    fCooldownTime = 60.0f;  //can't alt fire for 1 min
}

void AMedicMelee::AltAttackStart()
{
    //if active and not attacking and not cooling down
    if (bIsActive == true && bIsAttacking == false && bIsAltAttacking == false && bIsCoolingDown == false)
    {
        bIsAltAttacking = true;
        //disable inventory while attacking
        pOwnerRef->SetCanAccessInventory(false);
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMedicMelee::AltAttack, fAttackRate, false);
        pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type, true);
    }
}

//Insta-Kill an enemy, 60s cooldown
void AMedicMelee::AltAttack()
{
    //raycast to hit
    FHitResult hit;
    //if we hit with our alt fire, continue with the code
    if (pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayerT<AAIEnemy>(hit, fWeaponRange, ECollisionChannel::ECC_Pawn, false))
    {
        NetMC_PlaySound(pAltFireSound);
        //deal 999 damage, should kill any enemy
        DealDamage(&hit, 999.0f);
        //we are cooling down
        GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AMeleeBase::SetIsCooledDown, fCooldownTime, false);
        bIsCoolingDown = true;
        pOwnerRef->GetPlayerSoundComponent()->PlayAltMelee();
    }

    //disable inventory while attacking
    pOwnerRef->SetCanAccessInventory(true);

    //show hit line
    DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Green, false, 0.5f);
    bIsAltAttacking = false;
}

void AMedicMelee::AltAttackStop()
{

}