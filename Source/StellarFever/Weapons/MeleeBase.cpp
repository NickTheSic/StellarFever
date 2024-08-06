// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeBase.h"
#include "../Player/BasePlayer.h"
#include "../Player/Animation/PlayerActionMontageComponent.h"
#include "../AI/AIEnemy.h"
#include "../Components/RaycastComponent.h"
#include "../Components/PlayerSoundComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

AMeleeBase::AMeleeBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    fDamage = 20.0f;
    fAttackRate = 0.2f;
    fWeaponRange = 300.0f;

    Type = WeaponType::Melee;
}

void AMeleeBase::BeginPlay()
{
    Super::BeginPlay();
}

void AMeleeBase::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AMeleeBase::AttackCompleted()
{
    //we are no longer attacking
    bIsAttacking = false;
    //clear timer
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    //if this melee belongs to the captain and we're alt attacking (raging) return instead of enabling inventory
    if (pOwnerRef->GetPlayerInformation().ePlayerClass == EPlayerClass::Captain &&
        bIsAltAttacking == true)
    {
        return;
    }
    //enable inventory
    pOwnerRef->SetCanAccessInventory(true);
}

void AMeleeBase::AttackStart()
{
    //if not alt attacking and not already attacking
    if (bIsActive == true && bIsAttacking == false && bIsAltAttacking == false)
    {
        //When we left click we attack after our attackRate
        bIsAttacking = true;
        NetMC_PlaySound(pMissSound);
        //disable inventory while attacking
        pOwnerRef->SetCanAccessInventory(false);
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeBase::Attack, fAttackRate, false);
        //Attack();
        pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type);
    }
}

void AMeleeBase::Attack()
{
    //raycast to hit
    FHitResult hit;
    pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayerT<AAIEnemy>(hit, fWeaponRange, ECollisionChannel::ECC_Pawn, false);
    //if we deal damage
    if (DealDamage(&hit, fDamage))
    {
        //play the hit sound
        NetMC_PlaySoundSingle(pHitSound);
    }
    //show hit line
    DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Red, true, 2.0f);
    //we are finished attacking
    AttackCompleted();
}

void AMeleeBase::AttackStop()
{
    
}

bool AMeleeBase::DealDamage(const FHitResult* hit, float newDamage)
{
    //if we hit something
    if (hit->GetActor() != nullptr)
    {
        //print the name of thing we hit
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, hit->GetActor()->GetFName().ToString());

        //if actor we hit was an enemy
        if (hit->GetActor()->ActorHasTag("Enemy"))
        {
            //static cast to an enemy
            AAIEnemy* enemy = static_cast<AAIEnemy*>(hit->GetActor());

            //if enemy exists, deal damage
            if (enemy != nullptr)
            {
                //Blood Splatter/Visual Effect at hit.ImpactPoint?

                //deal fDamage to enemy we hit
                UGameplayStatics::ApplyDamage(hit->GetActor(), newDamage, pOwnerRef->GetController(), pOwnerRef, DamageType);
                return true; //we damaged the enemy
            }
            return false; //enemy was nullptr
        }
        return false; //actor we hit was not an enemy
    }
    return false; //actor we hit was nullptr
}

//When our alt attack cools down
void AMeleeBase::SetIsCooledDown()
{
    GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
    bIsCoolingDown = false;
}