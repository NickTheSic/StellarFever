// Fill out your copyright notice in the Description page of Project Settings.


#include "MedicGrenade.h"
#include "../../Player/BasePlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../Guns/EngineerBullet.h"
#include "../../AI/EnemyShield.h"
#include "Shakes/Shake_MedicGrenade.h"
#include "../../AI/AIEnemy.h"
#include "DrawDebugHelpers.h"

AMedicGrenade::AMedicGrenade()
{
    pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    pTPRenderedMesh->SetCollisionProfileName("NoCollision"); //OverlapAll == WorldStatic object, OverlapAllDynamic == WorldDynamic object
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pTPRenderedMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    pTPRenderedMesh->SetHiddenInGame(false);
    pTPRenderedMesh->SetSimulatePhysics(false);
    pTPRenderedMesh->SetEnableGravity(true);

    pTPRenderedMesh->IgnoreActorWhenMoving(pOwnerRef, true);
    pTPRenderedMesh->OnComponentBeginOverlap.AddDynamic(this, &AMedicGrenade::OnOverlapBegin);

    pSphereComponent->SetCollisionProfileName("NoCollision");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    bHasExplodedTwice = false;
    bHasAttached = false;
    fAttackRate = 2.0f;
    fDamage = 70.0f;
}

void AMedicGrenade::BeginPlay()
{
    Super::BeginPlay();

    UShake_MedicGrenade* shake = static_cast<UShake_MedicGrenade*>(cameraShake.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }
}

void AMedicGrenade::AttackStart()
{
    if (bHasGrenade && bIsActive)
    {
        Attack();
    }
}

void AMedicGrenade::Attack()
{
    //let player aim
    bIsAiming = true;
}

//throw on release
void AMedicGrenade::AttackStop()
{
    if (bIsAiming)
    {
        //overlap all to stick to a surface (still works if enemy/wall is on Block for grenades)
        pTPRenderedMesh->SetCollisionProfileName("OverlapAll");
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
        pTPRenderedMesh->SetGenerateOverlapEvents(true);

        //no longer aiming
        bIsAiming = false;

        Client_HidePath();

        //throw grenade
        Throw();
    }

}

//rTodo OPTIMIZATION: the code in FireImpulse() code can literally replace the need for a RadialForceComponent and a SphereComponent
void AMedicGrenade::Explode()
{
    //overlapping actors
    TArray<AActor*> overlappingActors;
    pSphereComponent->GetOverlappingActors(overlappingActors);

    NetMC_PlayWorldCameraShake();

    NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Sphere002", pTPRenderedMesh->GetBoneLocation("Sphere002"), FVector(20.0f, 20.0f, 20.0f));
    //NetMC_PlayParticleEffectAttached();   //rTodo: so we can scale it up

    //first explosion
    if (bHasExplodedTwice == false)
    {
        NetMC_PlaySoundSingle(pExplodeSound);
        //fire impulse
        pExplosionForce->FireImpulse();

        //Setup timer for 2nd explosion
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMedicGrenade::Explode, fAttackRate, false);

        ExplosionRaycast(overlappingActors);

        //we have exploded once
        bHasExplodedTwice = true;
    }
    //2nd explosion (if you right click before this goes off, next time you throw it only deals half damage)
    else
    {
        //fire impulse
        pExplosionForce->FireImpulse();

        ExplosionRaycast(overlappingActors);
    
        //reset for next time we do an explosion
        bHasExplodedTwice = false;
        bHasExploded = true;

        //setup recharge timer
        GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &AGrenadeBase::GiveGrenade, fRechargeTime, false);
        Client_StartGrenadeTimer();
        NetMC_UnEquip();
    }
}

void AMedicGrenade::ExplosionRaycast(TArray<AActor*> enemies)
{
    if (bHasExplodedTwice == false)
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
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "X: " + FString::SanitizeFloat(GetActorLocation().X) + "\nY: " + FString::SanitizeFloat(GetActorLocation().Y) + "\nZ: " + FString::SanitizeFloat(GetActorLocation().Z));
    }
    else
    {
        //explode with half damage
        for (auto i : enemies)
        {
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
                                shield->HurtShield(fDamage / 2.0f);
                                UGameplayStatics::ApplyDamage(i, fDamage / 2.0f, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                            }
                            //otherwise hurt the AI
                            else
                            {
                                UGameplayStatics::ApplyDamage(i, fDamage / 2.0f, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                            }
                        }
                        //otherwise hurt the AI
                        else
                        {
                            UGameplayStatics::ApplyDamage(i, fDamage / 2.0f, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                        }
                    }
                    //otherwise hurt the AI
                    else
                    {
                        UGameplayStatics::ApplyDamage(i, fDamage / 2.0f, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
                    }
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Raycast failed against: " + i->GetName());
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "We Hit: " + hit.GetActor()->GetName());
                }

                //DrawDebugLine(GetWorld(), GetActorLocation(), i->GetActorLocation(), FColor::Green, false, 5.0f, -1.0f, 2.0f);

            }
        }
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "X: " + FString::SanitizeFloat(GetActorLocation().X) + "\nY: " + FString::SanitizeFloat(GetActorLocation().Y) + "\nZ: " + FString::SanitizeFloat(GetActorLocation().Z));
        //this grenade is finished everything it needs to do, so unequip now
        NetMC_UnEquip();
    }
}

void AMedicGrenade::PreExplode_Implementation(AActor* OtherActor)
{
    //attach. NOTE: World geometry NEEDS to generate overlap events or this will not work
    AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
    //stop it from moving -- ESSENTIAL -- or else the mesh keeps getting updated on the trajectory path
    pTrajMovement->bSimulationEnabled = false;
    //Explode (rTodo: or set up timer to explode)
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMedicGrenade::Explode, fAttackRate, false);
    NetMC_PlaySound(pExplodeSound);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Medic Grenade is set to explode");
}

void AMedicGrenade::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    //if not player, and not this grenade, and we have thrown our grenade, and we haven't gone through any explosion code and we're not attached yet
    if (OtherActor != pOwnerRef && OtherActor != this && bHasGrenade == false && bHasExploded == false && bHasExplodedTwice == false && bHasAttached == false)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Medic Grenade Overlapped Something");
        //Server call for pre explosion code, hopefully this fixes the issue online where the grenade just stuck to an enemy and didn't do anything
        PreExplode(OtherActor);
        bHasAttached = true;
    }
}

void AMedicGrenade::ResetVars()
{
    Super::ResetVars();
    bHasExplodedTwice = false;
    bHasAttached = false;
}

void AMedicGrenade::NetMC_Equip()
{
    //if we have a grenade and it hasn't exploded
    if (bHasGrenade == true && bHasExploded == false)
    {
        //equip as normal
        Super::NetMC_Equip();
        pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    }
    //we don't have a grenade OR it has exploded
    else
    {
        return; //do nothing
    }
}

//if we unequip
void AMedicGrenade::NetMC_UnEquip()
{
    //if we have thrown our grenade AND it hasn't exploded yet
    if (bHasGrenade == false && bHasExploded == false)
    {
        //set to inactive for now, Unequip will be called later once it's done exploding
        bIsActive = false;
    }
    //we haven't thrown our grenade OR it has already exploded
    else
    {
        //unequip as normal
        Super::NetMC_UnEquip();
    }
}