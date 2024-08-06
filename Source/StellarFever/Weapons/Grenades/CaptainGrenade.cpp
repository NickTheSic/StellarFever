// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptainGrenade.h"
#include "../../Player/BasePlayer.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../AI/AIEnemy.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "../Guns/EngineerBullet.h"
#include "NiagaraComponent.h"
#include "Shakes/Shake_CaptainGrenade.h"

ACaptainGrenade::ACaptainGrenade()
{
    pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    pTPRenderedMesh->SetCollisionProfileName("BlockAll"); //OverlapAll == WorldStatic object, OverlapAllDynamic == WorldDynamic object
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    pTPRenderedMesh->SetHiddenInGame(false);
    pTPRenderedMesh->SetSimulatePhysics(false);
    pTPRenderedMesh->SetEnableGravity(true);

    pTPRenderedMesh->OnComponentHit.AddDynamic(this, &ACaptainGrenade::OnCompHit);
    pSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACaptainGrenade::OnOverlapBegin);
    pSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ACaptainGrenade::OnOverlapEnd);

    pSphereComponent->SetCollisionProfileName("NoCollision");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    pExplodeAudioComponent = CreateDefaultSubobject<UAudioComponent>("Explosion Audio Component");

    //burns for 8dmg/s for 4 seconds
    fBurnDamage = 8.0f;
    fBurnDuration = 4.0f;
    fBurnHitsPerSecond = 1.0f;

    //grenade lasts for 15s
    fGrenadeDuration = 15.0f;

    bHasFinishedBurning = false;
}

void ACaptainGrenade::BeginPlay()
{
    Super::BeginPlay();

    UShake_CaptainGrenade* shake = static_cast<UShake_CaptainGrenade*>(cameraShake.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }
}

void ACaptainGrenade::AttackStart()
{
    if (bHasGrenade && bIsActive)
    {
        Attack();
    }
}

void ACaptainGrenade::Attack()
{
    //let player aim
    bIsAiming = true;
}

void ACaptainGrenade::AttackStop()
{
    if (bIsAiming)
    {
        //no longer aiming
        bIsAiming = false;
        //set prediction array to hidden
        Client_HidePath();

        pTPRenderedMesh->SetCollisionProfileName("BlockAll");
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

        //throw grenade
        Throw();
    }
}

void ACaptainGrenade::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    //if our spherecomponent brought us here
    if (OverlappedComp == pSphereComponent)
    {
        //if not player, and not this grenade, and we have exploded
        if (OtherActor != pOwnerRef && OtherActor != this && bHasExploded == true)
        {
            //if the actor is an enemy
            if (OtherActor->ActorHasTag("Enemy"))
            {
                AAIEnemy* ai = static_cast<AAIEnemy*>(OtherActor);

                //if enemy exists
                if (ai != nullptr)
                {
                    if (!CheckArray.Contains(ai))
                        CheckArray.Add(ai); //start checking this AI
                    FHitResult hit;
                    //if the AI is in line of sight
                    if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), ai->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
                    {
                        BigFireArray.Add(ai); //they take full damage
                        ai->InitiateBurnDamage(pOwnerRef, fBurnDamage, fGrenadeDuration, fBurnHitsPerSecond * 5);
                    }
                    else
                    {
                        SmallFireArray.Add(ai); //they are not in line of sight
                    }
                }
            }
        }
    }
}

//When an enemy leaves the radius, they take normal afterburn damage
void ACaptainGrenade::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    //if not player, and not this grenade, and we have exploded
    if (OtherActor != pOwnerRef && OtherActor != this && bHasExploded == true)
    {
        //if the actor is an enemy
        if (OtherActor->ActorHasTag("Enemy"))
        {
            AAIEnemy* ai = static_cast<AAIEnemy*>(OtherActor);
            FHitResult hit;
            //if enemy exists
            if (ai != nullptr)
            {
                if (BigFireArray.Contains(ai))  //if in direct line of sight
                {
                    ai->InitiateBurnDamage(pOwnerRef, fBurnDamage, fBurnDuration, fBurnHitsPerSecond); //burn enemy that exited radius
                    BigFireArray.Remove(ai);
                }
                else if (SmallFireArray.Contains(ai)) //if not in direct line of sight
                {
                    SmallFireArray.Remove(ai); //remove from array
                }
                //if being checked against
                if (CheckArray.Contains(ai))
                {
                    CheckArray.Remove(ai); //remove from check array
                }
            }
        }
    }
}

void ACaptainGrenade::OnCompHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    //only explode once and only explode if thrown
    if (bHasExploded == false && bHasGrenade == false)
    {
        //start burning on impact
        Explode();
    }
}

void ACaptainGrenade::Explode()
{
    NetMC_PlayExplodeSound();
    NetMC_PlayWorldCameraShake();
    NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Top_grenade", GetActorLocation(), FVector(1.8f, 1.8f, 1.8f));
    
    //looped burning sound
    NetMC_PlaySound(pBurningSound);

    //we have exploded
    bHasExploded = true;
    bHasFinishedBurning = false;

    //we want things to overlap now
    pSphereComponent->SetCollisionProfileName("OverlapAll");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    //setup timer to call ExplodeEnd()
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &ACaptainGrenade::ExplodeEnd, fGrenadeDuration, false);
    GetWorld()->GetTimerManager().SetTimer(RaycastTimer, this, &ACaptainGrenade::RaycastCheck, .25f, true);
}

void ACaptainGrenade::ExplodeEnd()
{
    //stop playing our burn sound
    pAudioComponent->Stop();

    //reset explosion variable
    bHasExploded = false;
    bHasFinishedBurning = true;
    //back to nocollision so we don't trigger anything
    pSphereComponent->SetCollisionProfileName("NoCollision");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    //clear timer
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);

    //setup recharge timer
    GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &AGrenadeBase::GiveGrenade, fRechargeTime, false);
    Client_StartGrenadeTimer();

    //empty arrays
    CheckArray.Empty();
    BigFireArray.Empty();
    SmallFireArray.Empty();

    NetMC_UnEquip();
}

void ACaptainGrenade::RaycastCheck()
{
    for (auto& i : CheckArray)
    {
        FHitResult hit;
        //if the AI is in line of sight
        if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), i->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
        {
            //and the AI is in BigFireArray
            if (BigFireArray.Contains(i))
            {
                return; //do nothing
            }
            //if they're not in the big fire array
            else
            {
                //add them to it and burn them
                BigFireArray.Add(i);
                i->InitiateBurnDamage(pOwnerRef, fBurnDamage, fGrenadeDuration, fBurnHitsPerSecond * 5);
            }
        }
        //AI is not in line of sight
        else
        {
            //and they were in the big fire array
            if (BigFireArray.Contains(i))
            {
                //move them to small fire array and burn them
                BigFireArray.Remove(i);
                SmallFireArray.Add(i);
                i->InitiateBurnDamage(pOwnerRef, fBurnDamage, fBurnDuration, fBurnHitsPerSecond);
            }
            //otherwise they're in the small fire array
            else
            {
                return; //do nothing
            }
        }
    }
}

void ACaptainGrenade::GiveGrenade()
{
    Super::GiveGrenade();
    bHasFinishedBurning = false;
}

void ACaptainGrenade::NetMC_PlayExplodeSound_Implementation()
{
    pExplodeAudioComponent->SetSound(pExplodeSound);
    pExplodeAudioComponent->Play();
}

void ACaptainGrenade::NetMC_Equip()
{
    //if we have a grenade
    if (bHasGrenade == true)
    {
        //equip as normal
        Super::NetMC_Equip(); 
        pTPRenderedMesh->SetCollisionProfileName("NoCollision");
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    }
    //we don't have a grenade
    else
    {
        return; //do nothing
    }
}

void ACaptainGrenade::NetMC_UnEquip()
{
    //if we have thrown our grenade and the grenade hasn't finished its flame shenanigans
    if (bHasGrenade == false && bHasFinishedBurning == false)
    {
        //just set to inactive
        bIsActive = false;
    }
    //if we have our grenade OR the grenade has finished its business
    else
    {
        //unequip as normal
        Super::NetMC_UnEquip();
    }

}