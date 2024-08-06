// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerMelee.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/Animation/PlayerActionMontageComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../AI/AIEnemy.h"
#include "../../AI/EnemyShield.h"
#include "../../Components/RaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Shakes/Shake_GunnerMelee.h"
#include "NiagaraSystem.h"

AGunnerMelee::AGunnerMelee()
{
    fCooldownTime = 40.0f;

    fExplosionDamage = 100.0f;

    pSphere = CreateDefaultSubobject<USphereComponent>("Sphere Component");
    pSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pSphere->SetSphereRadius(400.f);
    pSphere->SetupAttachment(RootComponent);

    pRadialForce = CreateDefaultSubobject<URadialForceComponent>("Radial Force Component");
    pRadialForce->Radius = 400.f;
    pRadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_Pawn);
    pRadialForce->Falloff = ERadialImpulseFalloff::RIF_Constant;
    pRadialForce->ImpulseStrength = 600000.0f;
    pRadialForce->SetupAttachment(RootComponent);
}

void AGunnerMelee::BeginPlay()
{
    Super::BeginPlay(); 
    UShake_GunnerMelee* shake = static_cast<UShake_GunnerMelee*>(cameraShake.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }
}

void AGunnerMelee::AttackStart()
{
    if (bIsActive == true && bIsAttacking == false && bIsAltAttacking == false)
    {
        //When we left click we attack after our attackRate
        bIsAttacking = true;
        NetMC_PlaySound(pMissSound);

        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AGunnerMelee::Attack, fAttackRate, false);
        //lock inventory
        pOwnerRef->SetCanAccessInventory(false);
    }
}

void AGunnerMelee::Attack()
{
    //raycast to hit
    FHitResult hit;
    pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayerT<AAIEnemy>(hit, fWeaponRange, ECollisionChannel::ECC_Pawn, false);
    //if we deal damage
    if (DealDamage(&hit, fDamage))
    {
        //play the hit sound
        NetMC_PlaySoundSingle(pHitSound);
        //if we're here because of the alt attack and we hit,
        if (bIsAltAttacking)
        {
            //go to explosion hammer
            ExplosionHammer();
        }
    }

    //if we went for an alt fire and missed, deactivate
    if (bIsAltAttacking)
    {
        bIsAltAttacking = false;
    }

    //show hit line
    DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Red, true, 2.0f);
    //we are finished attacking
    AttackCompleted();
}

void AGunnerMelee::AttackStop()
{

}

void AGunnerMelee::AltAttackStart()
{
    if (bIsActive && bIsCoolingDown == false && bIsAttacking == false)
    {
        bIsAltAttacking = true;
        AltAttack();
    }
}

void AGunnerMelee::AltAttack()
{
    pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type, true);
    NetMC_PlaySound(pMissSound);

    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AGunnerMelee::Attack, fAttackRate, false);
    //lock inventory
    pOwnerRef->SetCanAccessInventory(false);
}

void AGunnerMelee::AltAttackStop()
{

}

void AGunnerMelee::ExplosionHammer()
{
    //Super::AltAttack();
    pOwnerRef->GetPlayerSoundComponent()->PlayAltMelee();

    //get overlapping actors
    TArray<AActor*> overlapActors;
    pSphere->GetOverlappingActors(overlapActors);

    pRadialForce->FireImpulse();

    NetMC_PlayWorldCameraShake();
    NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "SOCKET_Shell_Blast_Point", pTPRenderedMesh->GetSocketLocation("SOCKET_Shell_Blast_Point"));

    //if it's an enemy, deal explosion damage
    for (auto i : overlapActors)
    {
        //check for enemy
        if (i->ActorHasTag("Enemy"))
        {
            FHitResult hit;
            //if our geometry raycast hits nothing, then we have LoS to enemy, deal damage
            if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), i->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
            {
                //cast to enemy
                AAIEnemy* enemy = static_cast<AAIEnemy*>(i);
                //if it's a shield AI
                if (enemy->bShieldEnemy == true)
                {
                    //check if it has a shield
                    if (enemy->pCurrentShield)
                    {
                        //if so, cast its shield and deal damage
                        AEnemyShield* shield = Cast<AEnemyShield>(enemy->pCurrentShield);
                        if (shield != nullptr && shield->fShieldHealth > 0.0f)
                        {
                            //one shot the shield
                            shield->HurtShield(shield->fShieldHealth);
                        }
                        //otherwise hurt the AI
                        else
                        {
                            UGameplayStatics::ApplyDamage(i, (fDamage * 5.0f), GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                        }
                    }
                    //otherwise hurt the AI
                    else
                    {
                        UGameplayStatics::ApplyDamage(i, (fDamage * 5.0f), GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                    }
                }
                //otherwise hurt the AI
                else
                {
                    UGameplayStatics::ApplyDamage(i, (fDamage * 5.0f), GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                }
            }
        }
    }

    NetMC_PlaySound(pExplodeSound);

    //no longer alt attacking
    bIsAltAttacking = false;

    //cooling down
    bIsCoolingDown = true;
    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AMeleeBase::SetIsCooledDown, fCooldownTime, false);
}

void AGunnerMelee::NetMC_PlayWorldCameraShake_Implementation()
{
    UGameplayStatics::PlayWorldCameraShake(GetWorld(), cameraShake, GetActorLocation(), 0.0f, pRadialForce->Radius, 1.0f, false);
}