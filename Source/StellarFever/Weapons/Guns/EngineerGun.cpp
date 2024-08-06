// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineerGun.h"
#include "EngineerBulletPoolComponent.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/EngineerPlayer.h"
#include "../../Components/RaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Hud/GunHud.h"
#include "Shake/Shake_Engineer.h"
#include "Shake/Shake_EngineerBullet.h"
#include "Animation/AnimSequence.h"
#include "WeaponAnimInstance.h"

AEngineerGun::AEngineerGun()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

   // pAnimation = CreateDefaultSubobject<UAnimSequence>("Animation");


    fDamage = 10.0f;
    fBurnDuration = 4.0f;
    fBurnsPerSecond = 2.0f;
    fExplosionDamage = 40.0f;
    fMinBulletSpread = 0.0f;
    fMaxBulletSpread = 0.0f;

    fWeaponRange = 50000.0f;
    fStartFalloff = 1000.0f;
    fEndFalloff = 5000.0f;
    
    fAttackRate = .5f;
    fReloadTime = 4.0f;

    bIsAttacking = false;
    bIsActive = false;
    bIsReloading = false;
    bCanShoot = true;

    usMaxMagAmmo = 20;  //35 bullets in a mag
    usMagAmmo = usMaxMagAmmo;

    usNumMags = 6;  //6 mags

    usMaxReserveAmmo = usMaxMagAmmo * usNumMags;
    usReserveAmmo = usMaxReserveAmmo;
    

    AnimState = EAnimationState::Neutral;

    BulletPool = CreateDefaultSubobject<UEngineerBulletPoolComponent>("Bullet Pool");
    BulletPool->SetIsReplicated(true);
    ucMaxActiveBullets = 5;

	SetReplicates(true);
    SetReplicatingMovement(true);
}

void AEngineerGun::BeginPlay()
{
    Super::BeginPlay();

    //sets up bullets for designated player
    if (GetLocalRole() == ROLE_Authority)
    {
        //Initialize our pool to hold our max amount of bullets
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Initializing Bullet Pool");
        BulletPool->Server_Init(ucMaxActiveBullets, this);

        UShake_Engineer* shake = static_cast<UShake_Engineer*>(pCamShakeSubclass.GetDefaultObject());
        if (shake)
        {
            shake->Init();
        }
        UShake_EngineerBullet* bulletShake = static_cast<UShake_EngineerBullet*>(BulletCameraShake.GetDefaultObject());
        if (bulletShake)
        {
            bulletShake->Init();
        }

        pTPRenderedMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        pTPRenderedMesh->SetAnimInstanceClass(pAnimationInstance);
        Cast<UWeaponAnimInstance>(pTPRenderedMesh->GetAnimInstance())->SetWeaponReference(this);
        pTPRenderedMesh->Play(true);

        pFPRenderedMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        pFPRenderedMesh->SetAnimInstanceClass(pAnimationInstance);
        Cast<UWeaponAnimInstance>(pFPRenderedMesh->GetAnimInstance())->SetWeaponReference(this);
        pFPRenderedMesh->Play(true);
        pGunHud->AttachToComponent(pFPRenderedMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HudSocket");
    }
}

void AEngineerGun::AttackStart()
{
    //if no ammo
        //play no ammo sound
        //reload?
        //return
    bIsAttacking = true;

    //if active and not reloading and not alt attacking
    if (bIsActive == true && bIsReloading == false && bCanShoot == true)
    {
        //Call attack with no initial delay, based on fAttackRate, true for calling multiple times
        //GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEngineerGun::Attack, fAttackRate, true, 0.0f);
        AnimState = EAnimationState::Attacking;
        Attack();
        GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Red, "We are attacking");
    }
}

void AEngineerGun::Attack()
{
    //if we have ammo, shoot
    if (usMagAmmo > 0 && bCanShoot)
    {
       // pTPRenderedMesh->PlayAnimation(pAnimation, false);

        //Raycast shenanigans
        FHitResult hit;
		if (pOwnerRef != nullptr)
		{
            Super::Attack();

            //update ammo and hud text
            NetMC_UpdateText_Firing();
			GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Red, FString::FromInt(usMagAmmo));

            FVector loc;
            FRotator dir;
            pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);


            //randomize a shot within the cone (0 by default but used for when we get depressurized)
            FVector ShotDirection = FMath::VRandCone(dir.Vector(), FMath::DegreesToRadians(fMaxBulletSpread));

            if (pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, ShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false))
            {
                //Call Fire() passing in our raycast hit (assuming we hit something)
                BulletPool->Server_Fire(hit, FQuat(dir), fDamage, fBurnDuration, fBurnsPerSecond, fExplosionDamage);

                MakeNoiseAI(&hit);
            }
            
            //play sound
            NetMC_PlaySound(pFireSound);
            //muzzle flash
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "Muzzle", pFPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            //tracer
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

            PlayCameraShake();

            //for adding delay, similar to captaingun
            bCanShoot = false;
            GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AEngineerGun::Server_AttackCooldown, fAttackRate, false);
        }
    }
    //if we have no ammo
    else if (usMagAmmo == 0)
    {
        //play no ammo sound
        NetMC_PlaySoundSingle(pNoAmmoSound);
        AttackStop();
        ReloadStart();
    }
}

void AEngineerGun::AttackStop()
{
    //if active and not reloading
    if (bIsActive == true && bIsReloading == false)
    {
        bIsAttacking = false;
        AnimState = AnimState == EAnimationState::Attacking ? EAnimationState::Neutral : AnimState;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    }
}

void AEngineerGun::AltAttackStart()
{
    //Don't care if active, we can call this as long as it's out
    AltAttack();
}

void AEngineerGun::AltAttack()
{
    bIsAltAttacking = true;
    //Detonate all active bullets
    
    TArray<FVector> locs;
    locs = BulletPool->GetBulletLocations();
    for (int i = 0; i < locs.Num(); i++)
    {
        NetMC_PlayParticleEffectAtLocation(pExplosionEffect, locs[i]);
    }
    //if active bullets, play detonation line
    if (locs.Num() > 0)
    {
        Super::AltAttack();
    }

    BulletPool->Server_ExplodeBullets(fExplosionDamage);

}

void AEngineerGun::AltAttackStop()
{
    bIsAltAttacking = false;
}

void AEngineerGun::Server_AttackCooldown_Implementation()
{
    //we can shoot again
    bCanShoot = true;
    GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
    if (bIsAttacking)
    {
        Attack();
    }

}

void AEngineerGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEngineerGun, BulletPool);
}