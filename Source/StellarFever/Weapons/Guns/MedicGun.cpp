// Fill out your copyright notice in the Description page of Project Settings.

#include "MedicGun.h"
#include "../../Components/RaycastComponent.h"
#include "../../Components/HealthComponent.h"
#include "../../Player/BasePlayer.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "Hud/GunHud.h"
#include "Shake/Shake_Medic.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "WeaponAnimInstance.h"

AMedicGun::AMedicGun()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    fDamage = 12.0f;

    fAttackRate = 16.6f; //bullets per second
    fReloadTime = 3.0f; //what currently matches the sound files
    fAltAttackCharge = 1.4f; //what currently matches the sound files
    fMinBulletSpread = 0.0f;
    fCurrentBulletSpread = fMinBulletSpread;
    fMaxBulletSpread = 5.0f;
    fBulletSpreadIncrease = 0.5f;
    fBulletSpreadDecrease = 0.5f;

    fWeaponRange = 50000.0f;
    fStartFalloff = 1000.0f;
    fEndFalloff = 5000.0f;

    bIsAttacking = false;
    bIsAltAttacking = false;
    bIsActive = false;
    bIsReloading = false;

    usMaxMagAmmo = 35;  //35 bullets in a mag
    usMagAmmo = usMaxMagAmmo;

    usNumMags = 6;

    usMaxReserveAmmo = usMaxMagAmmo * usNumMags;    //6 mags
    usReserveAmmo = usMaxReserveAmmo;

    AnimState = EAnimationState::Neutral;
}

void AMedicGun::BeginPlay()
{
    Super::BeginPlay();

    UShake_Medic* shake = static_cast<UShake_Medic*>(pCamShakeSubclass.GetDefaultObject());
    if (shake)
    {
        shake->Init();
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

void AMedicGun::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    //if we have a bulletspread and we're not shooting
    if (fCurrentBulletSpread > fMinBulletSpread && bIsAttacking == false)
    {
        //reduce spread
        fCurrentBulletSpread -= fBulletSpreadDecrease;
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, "Reducing fBulletSpread: " + FString::SanitizeFloat(fCurrentBulletSpread));
        
        //if we go past our min, set to min
        if (fCurrentBulletSpread <= fMinBulletSpread)
            fCurrentBulletSpread = fMinBulletSpread;

    }
}

void AMedicGun::AttackStart()
{
    //if active and not reloading and not alt attacking
    if (bIsActive == true && bIsReloading == false && bIsAltAttacking == false)
    {
        //Call attack with no initial delay, based on fAttackRate, true for calling multiple times
        bIsAttacking = true;
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMedicGun::Attack, 1.0f / fAttackRate, true, 0.0f);
        GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Red, "We are attacking");

        //FVector loc;
        //FRotator dir;
        //pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);
        //
        //NetMC_PlayParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, dir);
    }
}

void AMedicGun::Attack()
{
    //if we have ammo, shoot
    if (usMagAmmo > 0)
    {
        //Raycast shenanigans
        FHitResult hit;
        //casting owner as character
        if (pOwnerRef != nullptr)
        {
            Super::Attack();

            //update ammo and hud text
            NetMC_UpdateText_Firing();
            GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Red, FString::FromInt(usMagAmmo));

            //RaycastSingleFromPlayer code copied below
            FVector loc;
            FRotator dir;

            if (pOwnerRef->GetController())
            {
                pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);
            
                //randomize a shot within the cone
                FVector ShotDirection = FMath::VRandCone(dir.Vector(), FMath::DegreesToRadians(fCurrentBulletSpread));

                //increase bullet spread
                if (fCurrentBulletSpread < fMaxBulletSpread)
                    fCurrentBulletSpread += fBulletSpreadIncrease;

                //deal damage
                pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, ShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, true);
                //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our WeaponRange
                //otherwise the hit landed before our falloff, meaning we do max damage
                if (hit.Distance > fStartFalloff)
                    DealDamage(&hit, FMath::Clamp((fDamage - (fDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fDamage));
                else
                    DealDamage(&hit, fDamage);

                //play weapon sound
                NetMC_PlaySound(pFireSound);

                //noise for ai
                MakeNoiseAI(&hit);

                PlayCameraShake();

                //muzzle flash
                if (pFPRenderedMesh->bOnlyOwnerSee)
                    Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "Muzzle", pFPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
                NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
                //tracer
                if (pFPRenderedMesh->bOnlyOwnerSee)
                    Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
                NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            }

        }
    }
    //if we have no ammo
    else if (usMagAmmo == 0)
    {
        //play no ammo sound
        NetMC_PlaySoundSingle(pNoAmmoSound);
        //stop attacking
        AttackStop();
        ReloadStart();
    }
}

void AMedicGun::AttackStop()
{
    //if active and not reloading and not alt attacking (we don't want to clear timer during alt attack)
    if (bIsActive == true && bIsReloading == false && bIsAltAttacking == false)
    {
        bIsAttacking = false;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    }
}

void AMedicGun::AltAttackStart()
{
    //if active and not reloading and not attacking and we have ammo loaded
    if (bIsActive == true && bIsReloading == false && bIsAttacking == false && usMagAmmo > 0)
    {
        //setup alt fire and start playing
        NetMC_PlaySound(pAltFireSound);

        //Call AltAttack with an initial delay of fAltAttackCharge (to charge up)
        bIsAltAttacking = true;
        GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMedicGun::AltAttack, fAltAttackCharge, false);
        GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Red, "We are alt attacking");

        //lock inventory
        pOwnerRef->SetCanAccessInventory(false);
    }
    else if (usMagAmmo == 0)
    {
        ReloadStart();
    }
}

//this only gets called if we have ammo
void AMedicGun::AltAttack()
{
    Super::AltAttack();

    GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Red, "In AltAttack");

    //Raycast shenanigans
    FHitResult hit;
    //casting owner as character
    if (pOwnerRef != nullptr)
    {
        //raycast from camera
        pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayer(hit, fWeaponRange, ECollisionChannel::ECC_GameTraceChannel3, false);
    }

    //if we hit something
    if (hit.GetActor() != nullptr)
    {
        if (hit.GetActor()->ActorHasTag("Player"))
        {
            //cast teammate
            ABasePlayer* teammate = static_cast<ABasePlayer*>(hit.GetActor());
            //if teammate exists, heal for ammo
            if (teammate != nullptr)
            {
                teammate->GetHealthComponent()->Heal(usMagAmmo);
            }
            //muzzle flash
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "Muzzle", pFPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            //tracer
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

        }
    }
    //if we didn't hit teammate, heal self
    else
    {
        pOwnerRef->GetHealthComponent()->Heal(usMagAmmo);
    }
    
    //shoot ammo regardless of hit or miss
    usMagAmmo = 0;
    PlayCameraShake();
    bIsAltAttacking = false;    // no longer attacking
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer); //clear timer

    //update ammo and hud text
    NetMC_UpdateText(usMagAmmo, usReserveAmmo);

    //unlock inventory
    pOwnerRef->SetCanAccessInventory(true);
}

void AMedicGun::AltAttackStop()
{
    //I don't think we need anything here, nothing should happen when you release M2
}
