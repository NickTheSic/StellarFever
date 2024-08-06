// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerGrenade.h"
#include "../../Player/BasePlayer.h"
#include "../../Components/InventoryComponent.h"
#include "../GunBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "../Guns/EngineerBullet.h"
#include "../../AI/EnemyShield.h"
#include "../../AI/AIEnemy.h"
#include "Shakes/Shake_GunnerGrenade.h"

AGunnerGrenade::AGunnerGrenade()
{
    pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    pTPRenderedMesh->SetCollisionProfileName("NoCollision");
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    pTPRenderedMesh->SetHiddenInGame(false);
    pTPRenderedMesh->SetSimulatePhysics(false);
    pTPRenderedMesh->SetEnableGravity(true);

    pTPRenderedMesh->OnComponentHit.AddDynamic(this, &AGunnerGrenade::OnCompHit);
    pTrajMovement->OnProjectileStop.AddDynamic(this, &AGunnerGrenade::OnProjectileStop);

    pTrajMovement->Bounciness = 0.2f; //percentage of velocity maintained after a bounce in the direction of the normal of the impact
    pTrajMovement->Friction = 0.4f;

    pSphereComponent->SetCollisionProfileName("OverlapAll");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    ucAmmoRefillPercentage = 20;
    ucRefillUses = 4; //4 refills, 1/player

    bCanRefill = false;

    fDamage = 30.0f;
    fAttackRate = 1.0f;

    ucTimesExploded = 0;

}

void AGunnerGrenade::BeginPlay()
{
    Super::BeginPlay();

    UShake_GunnerGrenade* shake = static_cast<UShake_GunnerGrenade*>(cameraShake.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }
}

void AGunnerGrenade::AttackStart()
{
    if (bHasGrenade && bIsActive)
    {
        pTrajMovement->SetVelocityInLocalSpace(FVector(1.0f, 0.0f, 0.0f));
        pTrajMovement->AddForce(FVector::UpVector * 1000.0f);
        Attack();
    }
}

void AGunnerGrenade::Attack()
{
    //let player aim
    bIsAiming = true;
}

void AGunnerGrenade::AttackStop()
{
    if (bIsAiming)
    {
        //no longer aiming
        bIsAiming = false;

        Client_HidePath();

        pTPRenderedMesh->SetCollisionProfileName("BlockAll");
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

        //throw grenade
        Throw();
    }
}

void AGunnerGrenade::NotifyHit(AActor* instigator, const FHitResult* hit)
{
    //is instigator a player and we do have refills left and we can refill
    if (instigator->ActorHasTag("Player") && ucRefillUses > 0 && bCanRefill)
    {
        //cast player
        ABasePlayer* player = static_cast<ABasePlayer*>(instigator);
        //all players have a gun
        AGunBase* gun = static_cast<AGunBase*>(player->GetInventoryComponent()->GetGun());
        
        //if total of max ammos is not equal to total of current ammos, refill ammo (basically won't refill ammo if you're already full)
        if (gun->GetMaxMagAmmo() + gun->GetMaxReserveAmmo() != gun->GetMagAmmo() + gun->GetReserveAmmo())
        {
            //refill ammo
            gun->AmmoRefill(gun->GetMaxReserveAmmo() * (ucAmmoRefillPercentage / 100.0f));
            //reduce refill uses
            ucRefillUses--;

            //when we run out of uses
            if (ucRefillUses == 0)
            {
                //we can't refill anymore, unequip
                bCanRefill = false;
                //setup recharge timer
                GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &AGrenadeBase::GiveGrenade, fRechargeTime, false);
                Client_StartGrenadeTimer();
                NetMC_UnEquip();
            }

            //play a sound of ammo clattering for the player
            NetMC_PlaySound(pAmmoGrabSound);
        }
    }
}

void AGunnerGrenade::NetMC_Equip()
{
    //if player tries to equip the grenade and we've thrown it
    if (bHasGrenade == false) 
    {
        //if we can still refill from it (this means it's on the floor)
        //if (bCanRefill == true)
        //{
            //explode instantly, loop based on attack rate
            GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AGunnerGrenade::Explode, fAttackRate, true, 0.0f);
        //}
    }
    //otherwise equip normally
    else
    {
        pTPRenderedMesh->SetCollisionProfileName("NoCollision");
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
        Super::NetMC_Equip();
    }

}

void AGunnerGrenade::NetMC_UnEquip()
{
    //if we have thrown our grenade and it has refills left, set inactive
    if (bHasGrenade == false && ucRefillUses > 0)
    {
        bIsActive = false;
    }
    //if we have our grenade OR it's out of ammo refills, unequip
    else
    {
        Super::NetMC_UnEquip();
    }
}

//detonate ammo box
void AGunnerGrenade::Explode()
{
    //nobody can refill ammo while it's exploding
    bCanRefill = false;
    
    if (ucTimesExploded < ucRefillUses)
    {
        NetMC_PlayWorldCameraShake();

        //play explosion sound
        NetMC_PlaySoundSingle(pExplodeSound);

        //NetMC_PlayParticleEffectAtLocation(pParticleEmitter, GetActorLocation());
        NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Torus003", pTPRenderedMesh->GetBoneLocation("Torus003"), FVector(20.0f, 20.0f, 20.0f));   //rTodo: so we can scale it up

        //overlapping actors
        TArray<AActor*> overlappingActors;
        pSphereComponent->GetOverlappingActors(overlappingActors);

        //explosion impulse
        pExplosionForce->FireImpulse();

        //deal damage to all enemies within sphere
        ExplosionRaycast(overlappingActors);
        ucTimesExploded++;

        if (ucTimesExploded == ucRefillUses)
        {
            GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
            bHasExploded = true;
            ucRefillUses = 0;

            //setup recharge timer
            GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &AGrenadeBase::GiveGrenade, fRechargeTime, false);
            Client_StartGrenadeTimer();

            NetMC_UnEquip();
            //when we are given a grenade, we'll reset our variables there
        }
    }
}

void AGunnerGrenade::ExplosionRaycast(TArray<AActor*> enemies)
{
    for (auto i : enemies)
    {
        //check for enemy
        if (i->ActorHasTag("Enemy"))
        {
            FHitResult hit;
            //if we don't hit anything with a geometry raycast
            if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), i->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
            {
                //cast to enemy
                AAIEnemy* enemy = static_cast<AAIEnemy*>(i);

                if (enemy->bShieldEnemy == true)
                {
                    //check if it has a shield
                    if (enemy->pCurrentShield)
                    {
                        //if so, cast its shield and deal damage
                        AEnemyShield* shield = Cast<AEnemyShield>(enemy->pCurrentShield);
                        if (shield != nullptr && shield->fShieldHealth > 0.0f)
                        {
                            //hurt the shield and AI
                            shield->HurtShield(fDamage);
                            UGameplayStatics::ApplyDamage(i, fDamage, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                        }
                        //otherwise hurt the AI
                        else
                        {
                            UGameplayStatics::ApplyDamage(i, fDamage, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                        }
                    }
                    //otherwise hurt the AI
                    else
                    {
                        UGameplayStatics::ApplyDamage(i, fDamage, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                    }
                }
                //otherwise hurt the AI
                else
                {
                    UGameplayStatics::ApplyDamage(i, fDamage, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                }
            }
        }
    }
}

//every time the gunner grenade hits something, play a sound
void AGunnerGrenade::OnCompHit(
    UPrimitiveComponent* HitComp, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    FVector NormalImpulse, 
    const FHitResult& Hit)
{
    //if we can't refill yet (hasn't stopped moving)
    if (bCanRefill == false)
    {
        //play a hit sound
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, OtherComp->GetName());
        pAudioComponent->SetSound(pLandSound);
        pAudioComponent->Play();
    }
}

void AGunnerGrenade::OnProjectileStop(const FHitResult& hit)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, hit.GetActor()->GetName());
    //if not player, and not this grenade, and we have thrown our grenade, and we haven't gone through any explosion code
    if (hit.GetActor() != pOwnerRef && hit.GetActor() != this && bHasGrenade == false && bHasExploded == false)
    {
        //stop it from moving
        pTrajMovement->bSimulationEnabled = false;

        //we can now refill ammo from the grenade
        bCanRefill = true;

        //rTodo: call Unfold() here when we get animations
        //in there you will play the unfold sound
    }
}

void AGunnerGrenade::ResetVars()
{
    Super::ResetVars();

    ucRefillUses = 4;
    bCanRefill = false;
    ucTimesExploded = 0;
}