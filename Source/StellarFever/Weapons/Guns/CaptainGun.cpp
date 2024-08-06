// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptainGun.h"
#include "../../Player/BasePlayer.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Components/RaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "Hud/GunHud.h"
#include "Shake/Shake_Captain.h"
#include "WeaponAnimInstance.h"

ACaptainGun::ACaptainGun()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    fDamage = 15.0f;
    fBigBarrelDamage = 25.0f;
    fBigBarrelCoolDown = 20.0f;
    fAttackRate = 1.0f;
    fReloadTime = 3.3f;
    fMinBulletSpread = 0.0f;
    fMaxBulletSpread = 3.0f;
    
    fWeaponRange = 50000.0f;
    fStartFalloff = 500.0f; //full dmg before 5m
    fEndFalloff = 3000.0f;  //no damage after 30m

    bIsAttacking = false;
    bIsActive = false;
    bIsReloading = false;

    usMaxMagAmmo = 4;  //2 shots in a mag
    usMagAmmo = usMaxMagAmmo;

    usNumMags = 12;

    usMaxReserveAmmo = usMaxMagAmmo * usNumMags;    //12 mags
    usReserveAmmo = usMaxReserveAmmo;

    AnimState = EAnimationState::Neutral;
}

void ACaptainGun::BeginPlay()
{
    Super::BeginPlay();

    UShake_Captain* shake = static_cast<UShake_Captain*>(pCamShakeSubclass.GetDefaultObject());
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

void ACaptainGun::AttackStart()
{
    //if active and not reloading and not alt attacking && not cooling down
    if (bIsActive == true && bIsReloading == false && bIsAltAttacking == false && bIsMainCoolingDown == false)
    {
        //Call attack
        bIsAttacking = true;
        Attack();
        GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Red, "We are attacking");
    }
}

void ACaptainGun::Attack()
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
            NetMC_PlaySound(pFireSound);
            
            //Get player direction
            FVector loc;
            FRotator dir, rot;
            pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);

            //Shots
            FVector ShotDirection = dir.Vector();//straight shot
            pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, ShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false);
            //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our EndFalloff
            //otherwise the hit landed before our falloff, meaning we do max damage
            if (hit.Distance > fStartFalloff)
                DealDamage(&hit, FMath::Clamp((fDamage - (fDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fDamage));
            else
                DealDamage(&hit, fDamage);

            //muzzle flash
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "Muzzle", pFPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
            //tracer
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

            hit.Reset(); //reset for next shot

            //This should only raycast the outside shots and exclude the center shot
            // .  .  .
            // .     .
            // .  .  .
            for (int yaw = -1; yaw < 2; yaw++)
            {
                //change yaw
                rot.Yaw = dir.Yaw + (fMaxBulletSpread * yaw) + (FMath::FRandRange(-1.5f, 1.5f));
                for (int pitch = -1; pitch < 2; pitch++)
                {
                    //if we're doing a middle shot
                    if (yaw == 0.0f && pitch == 0.0f)
                    {
                        //continue to next shot
                        continue;
                    }

                    //our shot  main shot +  bullet spread * direction + random variance 
                    rot.Pitch = dir.Pitch + (fMaxBulletSpread * pitch) + (FMath::FRandRange(-1.5f, 1.5f));
            
                    FVector NewShotDirection = rot.Vector();   //base our shot off of the original shot with fSpread modifications
                    pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, NewShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false); //shoot raycast

                    //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our EndFalloff
                    //otherwise the hit landed before our falloff, meaning we do max damage
                    if (hit.Distance > fStartFalloff)
                        DealDamage(&hit, FMath::Clamp((fDamage - (fDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fDamage));
                    else
                        DealDamage(&hit, fDamage);

                    //tracer
                    if (pFPRenderedMesh->bOnlyOwnerSee)
                        Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
                    NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

                    hit.Reset(); //reset for next shot
                }
            }

            MakeNoiseAI(&hit);

            PlayCameraShake();

            //Start cooldown
            bIsAttacking = false;
            GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &ACaptainGun::MainCoolDown, fAttackRate);
            bIsMainCoolingDown = true;
            //lock inventory
            pOwnerRef->SetCanAccessInventory(false);
        }
    }
    else if (usMagAmmo == 0)
    {
        //set no ammo sound and play
        NetMC_PlaySoundSingle(pNoAmmoSound);
        //Stop attacking
        AttackStop();
        ReloadStart();
    }
}

void ACaptainGun::AttackStop()
{
    bIsAttacking = false;
}

void ACaptainGun::AltAttackStart()
{
    //if active and not reloading and not alt attacking and not cooling down
    if (bIsActive == true && bIsReloading == false && bIsAttacking == false && bIsAltCoolingDown == false && bIsMainCoolingDown == false)
    {
        //Call attack
        bIsAltAttacking = true;
        AltAttack();
        GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Red, "We are attacking");
    }
}

void ACaptainGun::AltAttack()
{
    //I'm not sure what the designers want to do with ammo here, does Big barrel have ammo? does it drain the main ammo supply?
    //Does it fire all your shots? Or is it simply something you get back on cooldown, in which case i need no ammo code

    //Raycast shenanigans
    FHitResult hit;

    //casting owner as character
    if (pOwnerRef != nullptr)
    {
        //Get player direction
        FVector loc;
        FRotator dir, rot;
        pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);

        Super::AltAttack();
        NetMC_PlaySound(pAltFireSound);

        //Shots
        FVector ShotDirection = dir.Vector();//straight shot
        pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, ShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false);
        //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our EndFalloff
        //otherwise the hit landed before our falloff, meaning we do max damage
        if (hit.Distance > fStartFalloff)
            DealDamage(&hit, FMath::Clamp((fBigBarrelDamage - (fBigBarrelDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fBigBarrelDamage));
        else
            DealDamage(&hit, fBigBarrelDamage);
        hit.Reset(); //reset for next shot

        //muzzle flash
        if (pFPRenderedMesh->bOnlyOwnerSee)
            Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "Muzzle", pFPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
        NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "Muzzle", pTPRenderedMesh->GetSocketLocation("Muzzle"), FVector::OneVector, GetActorRotation());
        //tracer
        if (pFPRenderedMesh->bOnlyOwnerSee)
            Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
        NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

        //This should only raycast the outside shots and exclude the center shot
        // .  .  .
        // .     .
        // .  .  .
        for (int yaw = -1; yaw < 2; yaw++)
        {
            //change yaw
            rot.Yaw = dir.Yaw + (fMaxBulletSpread * yaw) + (FMath::FRandRange(-1.5f, 1.5f));
            for (int pitch = -1; pitch < 2; pitch++)
            {
                //if we're doing a middle shot
                if (yaw == 0.0f && pitch == 0.0f)
                {
                    //continue to next shot
                    continue;
                }

                //our shot  main shot +  bullet spread * direction + random variance 
                rot.Pitch = dir.Pitch + (fMaxBulletSpread * pitch) + (FMath::FRandRange(-1.5f, 1.5f));

                FVector NewShotDirection = rot.Vector();   //base our shot off of the original shot with fSpread modifications
                pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, NewShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false); //shoot raycast
				//pTPRenderedMesh.GetBoneLocation("Muzzle")
                
                //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our EndFalloff
                //otherwise the hit landed before our falloff, meaning we do max damage
                if (hit.Distance > fStartFalloff)
                    DealDamage(&hit, FMath::Clamp((fBigBarrelDamage - (fBigBarrelDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fBigBarrelDamage));
                else
                    DealDamage(&hit, fBigBarrelDamage);

                //tracer
                if (pFPRenderedMesh->bOnlyOwnerSee)
                    Client_FireExtras(pFPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
                NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("Muzzle"), hit.ImpactPoint, pTracerEffect, pTracerImpact);

                hit.Reset(); //reset for next shot
            }
        }
    }

    MakeNoiseAI(&hit);
    
    PlayCameraShake();

    //Start cooldown
    bIsAltAttacking = false;
    GetWorld()->GetTimerManager().SetTimer(CoolDownTimer, this, &ACaptainGun::AltCoolDown, fBigBarrelCoolDown);
    bIsAltCoolingDown = true;
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &ACaptainGun::MainCoolDown, fAttackRate);
    bIsMainCoolingDown = true;
}

void ACaptainGun::AltAttackStop()
{
    bIsAltAttacking = false;
}

void ACaptainGun::AltCoolDown()
{
    bIsAltCoolingDown = false;
    GetWorld()->GetTimerManager().ClearTimer(CoolDownTimer);
    GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::Red, "ALT COOLED DOWN");
    pOwnerRef->GetPlayerSoundComponent()->PlayAltFireRecharged();
}

void ACaptainGun::MainCoolDown_Implementation()
{
    //unlock inventory
    pOwnerRef->SetCanAccessInventory(true);
    bIsMainCoolingDown = false;
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::Red, "MAIN COOLED DOWN");
}