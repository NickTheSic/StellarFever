// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerGun.h"
#include "../../Components/RaycastComponent.h"
#include "../../Player/BasePlayer.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../../StellarFeverGameInstance.h"
#include "Components/WidgetComponent.h"
#include "Hud/GunHud.h"
#include "Shake/Shake_Gunner.h"
#include "WeaponAnimInstance.h"

AGunnerGun::AGunnerGun()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    fDamage = 4.0f;
    fLaserDamage = 50.0f;
    fAttackRate = 30.0; //bullets per second
    fLaserAttackRate = 0.75f;
    fReloadTime = 3.6f;
    fSpinTime = 1.0f;

    fMinBulletSpread = 3.0f;
    fCurrentBulletSpread = fMinBulletSpread;
    fMaxBulletSpread = 7.0f;
    fBulletSpreadIncrease = 0.5f;
    fBulletSpreadDecrease = 0.5f;

    fWeaponRange = 50000.0f;
    fStartFalloff = 1000.0f;
    fEndFalloff = 5000.0f;

    ucGunnerPassivePercent = 6;

    bIsAttacking = false;
    bIsAltAttacking = false;
    bIsActive = false;
    bIsReloading = false;
    bIsSpinningUp = false;
    bHasSpunUp = false;

    usMaxMagAmmo = 200;  //200 bullets in a mag
    usMagAmmo = usMaxMagAmmo;

    usNumMags = 2;

    usMaxReserveAmmo = usMaxMagAmmo * usNumMags;    //2 mags
    usReserveAmmo = usMaxReserveAmmo;

    AnimState = EAnimationState::Neutral;

    pLaserAudioComp = CreateDefaultSubobject<UAudioComponent>("Laser Audio Component");
    pLaserAudioComp->SetupAttachment(RootComponent);
    pLaserAudioComp->SetIsReplicated(true);
    
    pSpinAudioComp = CreateDefaultSubobject<UAudioComponent>("Spin Audio Component");
    pSpinAudioComp->SetupAttachment(RootComponent);
    pSpinAudioComp->SetIsReplicated(true);

    SetReplicates(true);
    SetReplicateMovement(true);
    SetReplicatingMovement(true);
}

void AGunnerGun::BeginPlay()
{
    Super::BeginPlay();
    pLaserAudioComp->SetSound(pLaserSound);
    pLaserAudioComp->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pGeneralSoundClass;
    pSpinAudioComp->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pGeneralSoundClass;
    UShake_Gunner* shake = static_cast<UShake_Gunner*>(pCamShakeSubclass.GetDefaultObject());
    if (shake)
    {
        shake->Init();
    }

    pTPRenderedMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    pTPRenderedMesh->SetAnimInstanceClass(pAnimationInstance);
    pTPRenderedMesh->Play(true);
    pFPRenderedMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    pFPRenderedMesh->SetAnimInstanceClass(pAnimationInstance);
    pFPRenderedMesh->Play(true);
    Cast<UWeaponAnimInstance>(pTPRenderedMesh->GetAnimInstance())->SetWeaponReference(this);
    Cast<UWeaponAnimInstance>(pFPRenderedMesh->GetAnimInstance())->SetWeaponReference(this);
    pGunHud->AttachToComponent(pFPRenderedMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HudSocket");
}

void AGunnerGun::Tick(float deltaTime)
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

void AGunnerGun::ReloadStart()
{
    //if spun up, spinning up, or spinning down, don't reload
    if (bHasSpunUp == true || bIsSpinningUp == true || bIsSpinningDown == true)
    {
        return;
    }
    //otherwise we must be in a neutral mode, you can reload
    else
    {
        Super::ReloadStart();
    }
}

void AGunnerGun::AttackStart()
{
    //only spin up if you have ammo, are active, and not reloading
    if (usMagAmmo > 0 && bIsActive == true && bIsReloading == false)
    {
        //if we're not spinning up, we haven't already finished spinning up, and we're not spinning down
        if (bIsSpinningUp == false && bHasSpunUp == false && bIsSpinningDown == false)
        {
            bIsAttacking = true;
            //Call AltAttack (this bypasses setting bIsAltAttacking to true in AltAttackStart, meaning if they let go of M1 they won't stay revved)
            AltAttack();
            AnimState = EAnimationState::Charging;
        }
        //if we're revving up already, we don't need to call AltAttack as the player already has
        else if (bIsSpinningUp == true)
        {
            bIsAttacking = true;
            bIsSpinningDown = false; //in case we stopped holding m2 during spinup
            AnimState = EAnimationState::Charging;
        }
        //if we're spinning down, mark gun to spin up again
        else if (bIsSpinningDown == true)
        {
            bIsAttacking = true;
            bIsSpinningUp = true;
            AnimState = EAnimationState::Charging;
        }
        //if we're spun up, start shooting
        else if (bHasSpunUp == true)
        {
            bIsAttacking = true;
            //setup shooting timers
            AnimState = EAnimationState::Attacking;
            GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AGunnerGun::Attack, 1.0f / fAttackRate, true, 0.0f); //fire immediately
            GetWorld()->GetTimerManager().SetTimer(LaserTimer, this, &AGunnerGun::Server_FireLaser, fLaserAttackRate, true);//fire based on laser attack rate
        }
    }
    else if (usMagAmmo == 0)
    {
        //we haven't already spun up, and we're not spinning down
        if (bHasSpunUp == false && bIsSpinningDown == false)
        {
            //we're neutral, so just reload
            ReloadStart();
        }
        //if we're spinning down, mark gun to reload
        else if (bIsSpinningDown == true)
        {
            bIsReloading = true;
        }
        //if we're spun up, spin down
        else if (bHasSpunUp == true)
        {
            //stop holding spin
            bIsAltAttacking = false;
            //mark to reload
            bIsReloading = true;
            //no longer spun up
            bHasSpunUp = false;
            //play spindown sound
            NetMC_PlaySpinSound(pSpinDownSound);
            //spinning down
            bIsSpinningDown = true;
            GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningDown, 0.5f, false);
        }
    }
}

void AGunnerGun::Attack()
{
    //if we have ammo, and are spun up, shoot 
    if (usMagAmmo > 0 && bHasSpunUp == true)
    {
        //% based saving ammo
        if (FMath::RandRange(0, 100) > ucGunnerPassivePercent)
            NetMC_UpdateText_Firing();

        GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Red, FString::FromInt(usMagAmmo));

        //Raycast shenanigans
        FHitResult hit;
        //casting owner as character
        if (pOwnerRef != nullptr)
        {
            Super::Attack();

            //RaycastSingleFromPlayer code copied below
            FVector loc;
            FRotator dir;
            pOwnerRef->GetController()->GetPlayerViewPoint(loc, dir);

            //randomize a shot within the cone
            FVector ShotDirection = FMath::VRandCone(dir.Vector(), FMath::DegreesToRadians(fCurrentBulletSpread));

            if (fCurrentBulletSpread < fMaxBulletSpread)
                fCurrentBulletSpread += fBulletSpreadIncrease;

            GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Red, "FIRING. Current Spread: " + FString::SanitizeFloat(fCurrentBulletSpread));

            pOwnerRef->GetRaycastComponent()->RaycastSingle(hit, loc, ShotDirection.Rotation(), fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false);
            //if the hit lands past our start falloff, deal damage from 1 to appropriate damage based on how far the shot is between our StartFalloff and our WeaponRange
            //otherwise the hit landed before our falloff, meaning we do max damage
            if (hit.Distance > fStartFalloff)
                DealDamage(&hit, FMath::Clamp((fDamage - (fDamage * ((hit.Distance - fStartFalloff) / (fEndFalloff - fStartFalloff)))), 1.0f, fDamage));
            else
                DealDamage(&hit, fDamage);

	    
            //play weapon sound
            NetMC_PlaySound(pFireSound);
            //muzzle flash
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_PlayParticleEffectAttached(pParticleEmitter, pFPRenderedMesh, "SOCKET_Bullet_Barrel", pFPRenderedMesh->GetSocketLocation("SOCKET_Bullet_Barrel"), FVector::OneVector, GetActorRotation());
            NetMC_PlayGunParticleEffectAttached(pParticleEmitter, pTPRenderedMesh, "SOCKET_Bullet_Barrel", pTPRenderedMesh->GetSocketLocation("SOCKET_Bullet_Barrel"), FVector::OneVector, GetActorRotation());
            //tracer
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_FireExtras(pFPRenderedMesh->GetSocketLocation("SOCKET_Bullet_Barrel"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("SOCKET_Bullet_Barrel"), hit.ImpactPoint, pTracerEffect, pTracerImpact);
            
            PlayCameraShake();

            MakeNoiseAI(&hit);
        }
    }
    //if no ammo, stop playing audio
    else if (usMagAmmo == 0 && bHasSpunUp == true)
    {
        NetMC_StopSound();
        NetMC_PlaySoundSingle(pNoAmmoSound); //looked it up, this is accurate to how dry firing a minigun would sound
    }
}

void AGunnerGun::AttackStop()
{
    //we are no longer firing
    bIsAttacking = false;
    //clear timer so we don't keep shooting
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
    GetWorld()->GetTimerManager().ClearTimer(LaserTimer);

    NetMC_StopSound();

    //if not reloading
    if (bIsReloading == false)
    {
        //if we are still in the process of spinning up when we let go, and we're not holding down M2
        if (bIsSpinningUp == true && bIsAltAttacking == false)
        {
            //if not spinning down
            if (bIsSpinningDown == false)
            {
                //start spinning down (spin up continues, when we hit the function, it sees we're marked to spin down)
                bIsSpinningDown = true;
                AnimState = EAnimationState::CoolingDown;
            }
        }
        //if we are spun up and bIsAltAttacking is false, then we can't be spun up anymore
        else if (bHasSpunUp == true && bIsAltAttacking == false)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SPINNING DOWN");

            //no longer spun up
            bHasSpunUp = false;

            //play spindown sound
            NetMC_PlaySpinSound(pSpinDownSound);

            //start spinning down
            bIsSpinningDown = true;
            AnimState = EAnimationState::CoolingDown;
            GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningDown, 0.5f, false);
        }
        //we're still holding our spin
        else if (bIsSpinningDown == false && bIsAltAttacking == true)
        {
            //this way when we stop firing, we still hear the barrel spin
            NetMC_PlaySpinSound(pSpunUpSound);
        }
    }
}

void AGunnerGun::AltAttackStart()
{
    //if we have ammo and if we're active and not reloading (we don't care if we're firing or not)
    if (usMagAmmo > 0 && bIsActive == true && bIsReloading == false)
    {
        bIsAltAttacking = true; //used to see we're holding down mouse2
        if (bHasSpunUp == false && bIsSpinningUp == false && bIsSpinningDown == false)
        {
            AltAttack(); //call this to start timer for spinup, only if we're not already spun up
        }
    }
    else if (usMagAmmo == 0)
    {
        //we haven't already spun up, and we're not spinning down
        if (bHasSpunUp == false && bIsSpinningDown == false)
        {
            //we're neutral, so just reload
            ReloadStart();
        }
        //if we're spinning down, mark gun to reload
        else if (bIsSpinningDown == true)
        {
            bIsReloading = true;
        }
        //if we're spun up, spin down
        else if (bHasSpunUp == true)
        {
            //stop holding spin
            bIsAltAttacking = false;
            //mark to reload
            bIsReloading = true;
            //no longer spun up
            bHasSpunUp = false;
            //play spindown sound
            NetMC_PlaySpinSound(pSpinDownSound);
            //spinning down
            bIsSpinningDown = true;
            GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningDown, 0.5f, false);
        }
    }
}

void AGunnerGun::AltAttack()
{
    Super::AltAttack();
    AnimState = EAnimationState::Charging;
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SPINNING UP");
    //start spin up sound
    NetMC_PlaySpinSound(pSpinUpSound);
    //We are now spinning up the barrel
    bIsSpinningUp = true;
    bIsSpinningDown = false;
    //set up timer to call DoneSpinningUp()
    GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningUp, fSpinTime);
    //lock inventory
    pOwnerRef->SetCanAccessInventory(false);
}

void AGunnerGun::AltAttackStop()
{
    //player is no longer holding spin on weapon
    bIsAltAttacking = false;

    //if we let go before we're done revving up AND we're not keeping the spin by holding mouse1
    if (bIsSpinningUp == true && bIsAttacking == false)
    {
        //start spinning down (spin up continues, when we hit the function, it sees we're marked to spin down)
        bIsSpinningDown = true;
    }
    //if we're revved up and not shooting, we will no longer be revved up
    else if (bHasSpunUp == true && bIsAttacking == false)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SPINNING DOWN");

        //no longer spun up
        bHasSpunUp = false;
        //play spindown sound
        NetMC_PlaySpinSound(pSpinDownSound);
        //spinning down
        bIsSpinningDown = true;
        AnimState = EAnimationState::CoolingDown;
        GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningDown, 0.5f, false);
    }
}

//Called after spinup timer is done
void AGunnerGun::Server_DoneSpinningUp_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Spun Up!");
    //if not marked to spin down, finish spin up shenanigans
    if (bIsSpinningDown == false)
    {
        //start playing spun up sound
        NetMC_PlaySpinSound(pSpunUpSound);
        //we aren't spinning up 
        bIsSpinningUp = false;
        //because we have spun up
        bHasSpunUp = true;
        //clear timer so we don't become spun up
        GetWorld()->GetTimerManager().ClearTimer(SpinTimer);
        AnimState = EAnimationState::Attacking;
        //if marked to attack, start shooting
        if (bIsAttacking)
        {
            //setup shooting timers
            GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AGunnerGun::Attack, 1.0f / fAttackRate, true, 0.0f); //fire immediately
            GetWorld()->GetTimerManager().SetTimer(LaserTimer, this, &AGunnerGun::Server_FireLaser, fLaserAttackRate, true);//fire based on laser attack rate
        }
    }
    //if marked to spin down
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SPINNING DOWN");

        //start playing spin down sound
        NetMC_PlaySpinSound(pSpinDownSound);
        //not spinning up
        bHasSpunUp = false;
        bIsSpinningUp = false;
        AnimState = EAnimationState::CoolingDown;
        //start spinning down
        GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningDown, 0.5f, false);
    }
}

//Called when we're done spinning down
void AGunnerGun::Server_DoneSpinningDown_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Spun Down!");
    //if not marked to spin up, finish up spin down shenanigans
    if (bIsSpinningUp == false)
    {
        //done spinning down
        bIsSpinningDown = false;
        //clear spin timer
        GetWorld()->GetTimerManager().ClearTimer(SpinTimer);
        //unlock inventory
        pOwnerRef->SetCanAccessInventory(true);
        AnimState = EAnimationState::Neutral;

        //if marked to reload
        if (bIsReloading)
        {
            bIsReloading = false; //necessary to reload
            ReloadStart();
        }
    }
    //if marked to spin up
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SPINNING UP");
        //start playing spin up sound
        NetMC_PlaySpinSound(pSpinUpSound);
        //not spinning down
        bIsSpinningDown = false;
        AnimState = EAnimationState::Charging;
        //start spinning up
        GetWorld()->GetTimerManager().SetTimer(SpinTimer, this, &AGunnerGun::Server_DoneSpinningUp, fSpinTime, false);
    }
}

void AGunnerGun::Server_FireLaser_Implementation()
{
    //if we have ammo (won't drain any), shoot
    if (usMagAmmo > 0 && bHasSpunUp == true)
    {
        //Raycast shenanigans
        FHitResult hit;
        //if owner exists
        if (pOwnerRef != nullptr)
        {
            //raycast from player
            pOwnerRef->GetRaycastComponent()->RaycastSingleFromPlayer(hit, fWeaponRange, ECollisionChannel::ECC_GameTraceChannel2, false);
            //deal damage
            DealDamage(&hit, fLaserDamage);
            //play a laser sound
            NetMC_PlayLaserSound();
            //tracer
            if (pFPRenderedMesh->bOnlyOwnerSee)
                Client_FireExtras(pFPRenderedMesh->GetSocketLocation("SOCKET_Lazer_Barrel"), hit.ImpactPoint, pLaserEffect, pLaserHitEffect);
            NetMC_FireExtras(pTPRenderedMesh->GetSocketLocation("SOCKET_Lazer_Barrel"), hit.ImpactPoint, pLaserEffect, pLaserHitEffect);
            //NetMC_PlayParticleEffectAtLocation(pLaserHitEffect, hit.ImpactPoint);
        }
    }
}

void AGunnerGun::NetMC_PlayLaserSound_Implementation()
{
    pLaserAudioComp->Play();
}

void AGunnerGun::NetMC_PlaySpinSound_Implementation(USoundCue* sound)
{
    pSpinAudioComp->SetSound(sound);
    pSpinAudioComp->Play();
}