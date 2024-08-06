// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineerGrenade.h"
#include "../../Player/BasePlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Guns/EngineerBullet.h"
#include "../../AI/EnemyShield.h"
#include "../../AI/AIEnemy.h"
#include "Shakes/Shake_EngineerGrenade.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

AEngineerGrenade::AEngineerGrenade()
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
    pTPRenderedMesh->OnComponentBeginOverlap.AddDynamic(this, &AEngineerGrenade::OnOverlapBegin);

    pSphereComponent->SetCollisionProfileName("NoCollision");
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

    fGrenadeRange = 200.0f; //grenade explosion range
    fSentryRange = 600.0f; //sentry range
    fDamage = 30.0f; //grenade damage
    fSentryDamage = 20.0f; //sentry damage
    fAttackRate = 1.0f; //sentry firing frequency
    bIsSentry = false; //grenade by default
    fSentryJamTime = 10.0f; //sentry jams after 10s of being active
    fSentryExplodeTime = 2.0f; //time for sentry to explode after jamming

    TracerTargetName = "Target";
}

void AEngineerGrenade::BeginPlay()
{
    Super::BeginPlay();

    UShake_EngineerGrenade* shake = static_cast<UShake_EngineerGrenade*>(cameraShake.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }
}

void AEngineerGrenade::AttackStart()
{
    if (bHasGrenade && bIsActive)
    {
        Attack();
    }
}

void AEngineerGrenade::Attack()
{
    //let player aim the grenade
    bIsAiming = true;
}

//throw on release
void AEngineerGrenade::AttackStop()
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
        //set prediction array to hidden
        //for (int i = 0; i < GrenadePredictionArray.Num(); i++)
        //    GrenadePredictionArray[i]->SetActorHiddenInGame(true);
        Client_HidePath();

        //throw grenade
        Throw();
    }
}

void AEngineerGrenade::NetMC_Equip()
{
    //if we have a grenade and it hasn't exploded
    if (bHasGrenade == true && bHasExploded == false)
    {
        Super::NetMC_Equip();
        pTPRenderedMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    }
    //otherwise it's in the world or already dealt with so do nothing
    else
    {
        return;
    }
}

void AEngineerGrenade::NetMC_UnEquip()
{
    //if we don't have a grenade and it hasn't exploded
    if (bHasGrenade == false && bHasExploded == false)
    {
        bIsActive = false;
    }
    //if we have a grenade OR it has exploded
    else
    {
        Super::NetMC_UnEquip();
    }
}

void AEngineerGrenade::Explode()
{
    NetMC_PlayWorldCameraShake();

    //NetMC_PlayParticleEffectAtLocation(pParticleEmitter, GetActorLocation());
    NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "RootBone", pTPRenderedMesh->GetBoneLocation("RootBone"), FVector(20.0f, 20.0f, 20.0f));   //rTodo: so we can scale it up

    //play explode sound
    NetMC_PlaySoundSingle(pExplodeSound);

    //change sphere radius to appropriate range, update overlaps is true
    pSphereComponent->SetSphereRadius(fGrenadeRange, true);

    //overlapping actors
    TArray<AActor*> overlappingActors;
    pSphereComponent->GetOverlappingActors(overlappingActors);

    //damage all enemies within the radius
    ExplosionRaycast(overlappingActors);

    pExplosionForce->FireImpulse();

    //we are finished with this grenade, hide it
    bHasExploded = true;

    //setup recharge timer
    GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &AGrenadeBase::GiveGrenade, fRechargeTime, false);
    Client_StartGrenadeTimer();

    NetMC_UnEquip();
}

void AEngineerGrenade::ExplosionRaycast(TArray<AActor*> enemies)
{
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

void AEngineerGrenade::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)


{
    //if other actor is not our owner, or ourself, we have thrown a grenade and it hasn't exploded and it's a grenade
    if (OtherActor != pOwnerRef && OtherActor != this && bHasGrenade == false && bHasExploded == false && bIsSentry == false)
    {
        //explode if we hit an enemy or their shield
        if (OtherActor->ActorHasTag("Enemy") || OtherActor->ActorHasTag("Shield"))
        {
            Explode();
        }
        //otherwise turn into a sentry that attacks within a range
        else
        {
            //change sphere radius to appropriate range, update overlaps is true
            pSphereComponent->SetSphereRadius(fSentryRange, true);
            //attach. NOTE: World geometry NEEDS to generate overlap events or this will not work
            AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
            //stop it from moving -- ESSENTIAL -- or else the mesh keeps getting updated on the trajectory path
            pTrajMovement->bSimulationEnabled = false;
            //start attacking in sentry mode
            bIsSentry = true;

            //rTodo: once animations are implemented, we'll pass a wait time to the sentry attack function, and add it to the sentry jam time
            NetMC_PlaySoundSingle(pSentryBuildSound);

            //setup attack timer to call SentryAttack() every fAttackRate seconds and loop it 
            GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEngineerGrenade::SentryAttack, fAttackRate, true, 1.0f);
            //setup jam timer to explode the sentry after X amount of time
            GetWorld()->GetTimerManager().SetTimer(SentryJamTimer, this, &AEngineerGrenade::SentryJam, fSentryJamTime + 1.0f, false);
        }
    }
}

void AEngineerGrenade::SentryAttack()
{
    //overlapping actors
    TArray<AActor*> overlappingActors;
    pSphereComponent->GetOverlappingActors(overlappingActors);

    float tempDist = fSentryRange; //for finding closest enemy
    uint8 enemyNum = 0; //for enemy we're shooting

    //for all overlapped actors
    for (int i = 0; i < overlappingActors.Num(); i++)
    {
        //check enemies or shields
        if (overlappingActors[i]->ActorHasTag("Enemy") || overlappingActors[i]->ActorHasTag("Shield"))
        {
            FHitResult hit;
            //if we can raycast against the enemy
            if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), overlappingActors[i]->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel1))
            {
                if (hit.GetActor() == overlappingActors[i])
                {
                    //check their distance
                    float temp = FVector::Dist(GetActorLocation(), overlappingActors[i]->GetActorLocation());
                    //if they're closer than our current target, save it
                    if (temp < tempDist)
                    {
                        tempDist = temp;
                        enemyNum = i;
                    }
                }
            }
        }
    }

    //if we found an enemy
    if (tempDist < fSentryRange)
    {
        //if an enemy is closest
        if (overlappingActors[enemyNum]->ActorHasTag("Enemy"))
        {
            //Shoot at enemyNum
            UGameplayStatics::ApplyDamage(overlappingActors[enemyNum], fSentryDamage, GetOwner()->GetInstigatorController(), pOwnerRef, DamageType);
        }
        //if a shield is closest
        else if (overlappingActors[enemyNum]->ActorHasTag("Shield"))
        {
            AEnemyShield* shield = Cast<AEnemyShield>(overlappingActors[enemyNum]);
            if (shield)
            {
                //damage shield
                shield->HurtShield(fSentryDamage);
            }
        }
        
        //play shoot sound
        NetMC_PlaySound(pSentryShootSound);

        NetMC_ShowTracer(overlappingActors[enemyNum]);

        //you'll need direction from sentry to enemy and rotate accordingly
    }
}

void AEngineerGrenade::NetMC_ShowTracer_Implementation(AActor* target)
{
    //rTodo: will need to rotate turret to face target
    //Add muzzle effect

    if (pTracerEffect)
    {
        //FVector MuzzleLocation = pMeshComp->GetSocketLocation(MuzzleSocketName);
        FVector MuzzleLocation = GetActorLocation();
        UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTracerEffect, MuzzleLocation);
        if (TracerComp)
        {
            TracerComp->SetVectorParameter(TracerTargetName, target->GetActorLocation());
        }
    }
}

void AEngineerGrenade::SentryJam()
{
    //clear jam and attack timer so it stops functioning
    GetWorld()->GetTimerManager().ClearTimer(SentryJamTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);

    //reset jam timer to call explode after 2s to show that the sentry is down
    GetWorld()->GetTimerManager().SetTimer(SentryJamTimer, this, &AEngineerGrenade::Explode, fSentryExplodeTime, false);
}

void AEngineerGrenade::ResetVars()
{
    Super::ResetVars();

    bIsSentry = false; //grenade by default

    //we shouldn't need to change the size of the sphere component because explode is called one way or another and that will set it back to default.
    //In the case that we jump from station to station and that reinitializes everything or something like that then we could reset it here
}


void AEngineerGrenade::GiveGrenade()
{
    //do normal give grenade code
    Super::GiveGrenade();
    //clear sentryjamtimer since normal code wouldn't do that
    GetWorld()->GetTimerManager().ClearTimer(SentryJamTimer);
}