// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "../Player/BasePlayer.h"
#include "../Components/PlayerSoundComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Guns/EngineerBullet.h"
#include "Grenades/GrenadePrediction.h"
#include "Net/UnrealNetwork.h"
#include "../AI/AIEnemy.h"
#include "Grenades/PredictionPath.h"
#include "Components/ProgressBar.h"

AGrenadeBase::AGrenadeBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    fGrenadeRange = 600.0f; //3m radius

    pTPRenderedMesh->SetGenerateOverlapEvents(true);
    pTPRenderedMesh->SetCollisionObjectType(ECC_GameTraceChannel1); //grenade

    
    pTrajMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
    pTrajMovement->bShouldBounce = true; //blocking grenades bounce, overlap grenades won't get the chance to
    pTrajMovement->bSimulationEnabled = false; //disabled by default so grenade doesn't fly all over the place when inactive
    pTrajMovement->bAutoUpdateTickRegistration = false;

    //sets mesh as the moving component (interpolated component is for networking smoothing)
    pTrajMovement->SetUpdatedComponent(pTPRenderedMesh);
    pTrajMovement->SetInterpolatedComponent(pTPRenderedMesh);

    pExplosionForce = CreateDefaultSubobject<URadialForceComponent>("Explosion Force Sphere");
    pExplosionForce->bIgnoreOwningActor = false; //do physics on actor that owns this component
    pExplosionForce->bImpulseVelChange = true; //ignore mass when applying force/impulse
    pExplosionForce->Falloff = ERadialImpulseFalloff::RIF_Linear; //weaker force further from origin
    pExplosionForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_Pawn); // affect pawns?

    pExplosionForce->SetupAttachment(RootComponent);

    pSphereComponent = CreateDefaultSubobject<USphereComponent>("Damage Sphere");
    pSphereComponent->SetCollisionObjectType(ECC_GameTraceChannel1); //grenade
    pSphereComponent->SetupAttachment(RootComponent);

    pExplosionForce->Radius = fGrenadeRange;
    pSphereComponent->InitSphereRadius(fGrenadeRange);

    bHasGrenade = true;
    bHasExploded = false;

    Type = WeaponType::Grenade;

    PredictPath = CreateDefaultSubobject<UPredictionPath>("Grenade Prediction Path");
    PredictPath->SetIsReplicated(true);

    fRechargeTime = 30.0f;

    predictParams.ActorsToIgnore.Add(pOwnerRef);    //ignore player for collision
    predictParams.ActorsToIgnore.Add(this);    //ignore self for collision
    predictParams.bTraceWithCollision = true;   //trace with collision
    predictParams.bTraceComplex = true;         //collisions based on triangles rather than simple primitives
    predictParams.bTraceWithChannel = true;     //trace by channel for collision
    predictParams.TraceChannel = ECollisionChannel::ECC_GameTraceChannel1; //grenade collision channel
    predictParams.MaxSimTime = 3.0f;
    predictParams.SimFrequency = 10.0f; //recommended between 10 and 30 depending on quality vs performance
    predictParams.DrawDebugType = EDrawDebugTrace::None;
    predictParams.ProjectileRadius = 30.0f;

    SetActorTickInterval(0.05f);

    ucPredictNum = 5;
}

void AGrenadeBase::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        SpawnPredictionPath();
    }
}

void AGrenadeBase::SpawnPredictionPath_Implementation()
{
    PredictPath->SpawnPath(ucPredictNum, pGrenadePredictionExample, pPredictMaterial, this);
}

void AGrenadeBase::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (pOwnerRef != nullptr && pOwnerRef->GetLocalRole() == ROLE_Authority)
    {
        if (bIsAiming)
        {
            //predict path from player's view
            FVector loc;
            FRotator dir;
            pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);
            predictParams.StartLocation = loc + dir.Vector() * 100.0f; 
            predictParams.LaunchVelocity = dir.Vector() * 630.0f;
            UGameplayStatics::PredictProjectilePath(pTPRenderedMesh, predictParams, predictResult);

            float xVal, yVal;

            //if there is more path data than our prediction number, restrict it to our prediction amount
            if (predictResult.PathData.Num() > ucPredictNum)
            {
                xVal = (predictResult.PathData[ucPredictNum - 1].Location.X - GetActorLocation().X) / (ucPredictNum - 1);
                yVal = (predictResult.PathData[ucPredictNum - 1].Location.Y - GetActorLocation().Y) / (ucPredictNum - 1);
            }
            //otherwise we're hitting something so we can just use the normal number
            else
            {
                xVal = (predictResult.PathData[predictResult.PathData.Num() - 1].Location.X - GetActorLocation().X) / (predictResult.PathData.Num() - 1);
                yVal = (predictResult.PathData[predictResult.PathData.Num() - 1].Location.Y - GetActorLocation().Y) / (predictResult.PathData.Num() - 1);
            }
       

            //for size of prediction array
            int i = 0;
            //for (auto& obj : GrenadePredictionArray)
            for (auto& obj : PredictPath->GetPredictionArray())
            {
                //if we have enough path data for our prediction array
                if (i < predictResult.PathData.Num())
                {
                    //update position based on our x and y increments, multiplied by i for our number in the prediction array
                    obj->SetActorLocation(FVector(GetActorLocation().X + xVal * i, GetActorLocation().Y + yVal * i, predictResult.PathData[i].Location.Z));
                    //i++;
                    obj->SetActorRotation(FRotator(0.0f, dir.Yaw, 0.0f));
                    //if invisible
                    if (obj->IsHidden())
                    {
                        //reveal
                        //obj->SetActorHiddenInGame(false);
                        Client_ShowPathSingle(i);
                    }
                }
                //if there are no more valid points
                else
                {
                    //hide actor
                    //obj->SetActorHiddenInGame(true);
                    Client_HidePathSingle(i);
                }
                i++;
            }
        }
    }
}

void AGrenadeBase::Throw()
{
    if (bHasGrenade && bIsActive)
    {
        //Play throw sound
        NetMC_PlaySound(pThrowSound);

        //Get Player viewpoint
        FVector loc;
        FRotator dir;
        pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);

        //resetting moving component (gets cleared after OnProjectileStop)
        pTrajMovement->SetUpdatedComponent(pTPRenderedMesh);
        pTrajMovement->SetInterpolatedComponent(pTPRenderedMesh);

        //for network smoothing
        pTrajMovement->ResetInterpolation();

        //enable physics simulation on grenade
        pTrajMovement->bSimulationEnabled = true;

        pTPRenderedMesh->SetWorldLocation(loc);
        pTPRenderedMesh->SetWorldRotation(FRotator(0.0f, dir.Yaw, 0.0f));
        pTPRenderedMesh->SetCollisionObjectType(ECC_GameTraceChannel1); //grenade

        pFPRenderedMesh->SetVisibility(false);
        pTPRenderedMesh->SetOwnerNoSee(false);
        pTPRenderedMesh->SetVisibility(true);

        //throw grenade
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        pTrajMovement->AddForce(dir.Vector() * 100000.0f);
        //pTrajMovement->SetVelocityInLocalSpace(dir.Vector() * 1000.0f);
        bHasGrenade = false;

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "In AGrenadeBase::Attack");
    }
}

//When we right click
void AGrenadeBase::AltAttackStart()
{
    //if we have grenade(s)
    if (bHasGrenade && bIsActive)
    {
        AltAttack();
    }
    //else
    //{
    //    GiveGrenade();
    //}
}
//AltAttack code
void AGrenadeBase::AltAttack()
{
    if (pOwnerRef->GetLocalRole() == ROLE_Authority)
    {
        //invert our aim bool
        bIsAiming = true;

        //if we are now aiming
        if (bIsAiming)
        {
            //set prediction array to visible
            Client_ShowPath();
        }
        //if we are no longer aiming
        else
        {
            AltAttackStop();
        }
    }
}
//When we release right click
void AGrenadeBase::AltAttackStop()
{
    //we are no longer aiming
    bIsAiming = false;
    //set prediction array to hidden
    Client_HidePath();
}

void AGrenadeBase::Explode()
{

}

void AGrenadeBase::ExplosionRaycast(TArray<AActor*> enemies)
{

}

void AGrenadeBase::GiveGrenade()
{
    //if we don't have a grenade
    if (bHasGrenade == false)
    {
        //if we're somehow active while our grenade is refreshing
        if (bIsActive)
        {
            //show grenade
            NetMC_Equip();
        }
        
        //reset grenade variables
        ResetVars();
        //clear timer
        GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
        GetWorld()->GetTimerManager().ClearTimer(RechargeTimer);
        //attach to player
        pTrajMovement->ClearPendingForce(true);
        pTrajMovement->SetVelocityInLocalSpace(FVector::ZeroVector);
        pTrajMovement->bSimulationEnabled = false;

        pFPRenderedMesh->SetVisibility(true);
        pTPRenderedMesh->SetOwnerNoSee(true);
        pTPRenderedMesh->SetVisibility(false);
        //pTPRenderedMesh->bOwnerNoSee = true;

        //Based on player class, attach weapon to appropriate socket. Otherwise just attach and hope.
        switch (pOwnerRef->GetPlayerInformation().ePlayerClass)
        {
        case EPlayerClass::Captain: AttachToComponent(pOwnerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
        case EPlayerClass::Engineer: AttachToComponent(pOwnerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
        case EPlayerClass::Gunner: AttachToComponent(pOwnerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
        case EPlayerClass::Medic: AttachToComponent(pOwnerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
        default: AttachToComponent(pOwnerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
        }

        pOwnerRef->GetPlayerSoundComponent()->PlayGrenadeRecharged();

    }
}

void AGrenadeBase::Client_StartGrenadeTimer_Implementation()
{
    pOwnerRef->StartGrenadeTimer();
}

void AGrenadeBase::NetMC_Equip()
{
    Super::NetMC_Equip();
}

//if we unequip
void AGrenadeBase::NetMC_UnEquip()
{
    //hide prediction path, then unequip
    Client_HidePath();
    Super::NetMC_UnEquip();
}

void AGrenadeBase::Client_ShowPath_Implementation()
{
    //for (int i = 0; i < GrenadePredictionArray.Num(); i++)
    //    GrenadePredictionArray[i]->ShowPath();
    PredictPath->Client_ShowPath();
}

void AGrenadeBase::Client_ShowPathSingle_Implementation(uint8 pathNum)
{
    //GrenadePredictionArray[pathNum]->ShowPath();
    PredictPath->Client_ShowPathSingle(pathNum);
}

void AGrenadeBase::Client_HidePath_Implementation()
{
    //for (int i = 0; i < GrenadePredictionArray.Num(); i++)
    //    GrenadePredictionArray[i]->HidePath();
    PredictPath->Client_HidePath();
}

void AGrenadeBase::Client_HidePathSingle_Implementation(uint8 pathNum)
{
    //GrenadePredictionArray[pathNum]->HidePath();
    PredictPath->Client_HidePathSingle(pathNum);
}

void AGrenadeBase::ResetVars()
{
    Super::ResetVars();

    bHasGrenade = true;
    bHasExploded = false;
    bIsAiming = false;
}

void AGrenadeBase::NetMC_PlayWorldCameraShake_Implementation()
{
    UGameplayStatics::PlayWorldCameraShake(GetWorld(), cameraShake, GetActorLocation(), 0.0f, fGrenadeRange * 3.0f, 1.0f, false);
}

void AGrenadeBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGrenadeBase, PredictPath);
    DOREPLIFETIME(AGrenadeBase, RechargeTimer);
    //DOREPLIFETIME(AGrenadeBase, GrenadePredictionArray);
}

