// Fill out your copyright notice in the Description page of Project Settings.

#include "GunBase.h"
#include "Engine/EngineTypes.h"
#include "Components/WidgetComponent.h"
#include "../AI/AIEnemy.h"
#include "../AI/EnemyShield.h"
#include "../AI/BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Player/BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShake.h"
#include "../AI/EnemyShield.h"
#include "Net/UnrealNetwork.h"
#include "Guns/Hud/GunHud.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "../Player/Animation/PlayerActionMontageComponent.h"
#include "../BeenoFight/BeenoDimeBoss.h"

#define SURFACE_FLESHHEADSHOT   SurfaceType1
#define SURFACE_FLESHIMPACT     SurfaceType2

AGunBase::AGunBase()
{
    pGunHud = CreateDefaultSubobject<UWidgetComponent>("Gun Hud Widget");
	pGunHud->SetCollisionProfileName("NoCollision");
    pGunHud->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pGunHud->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pGunHud->SetIsReplicated(true);
    pGunHud->SetupAttachment(RootComponent);
    pGunHud->bOwnerNoSee = false;
    pGunHud->bOnlyOwnerSee = true;


    //haptic = CreateDefaultSubobject<UHapticFeedbackEffect_Curve>("feedback");
    haptic = NewObject<UHapticFeedbackEffect_Curve>();

    Type = WeaponType::Gun;

    TracerTargetName = "Target";

    fItemLoudness = .67f;

	SetReplicates(true);
	SetReplicatingMovement(true);
    

}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    UGunHud* ugunhud = Cast<UGunHud>(pGunHud->GetUserWidgetObject());
    ugunhud->SetGun(this);
}

void AGunBase::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AGunBase::ReloadStart()
{
	//if not at full mag and we have ammo to reload and we're not already reloading
	if (usMagAmmo != usMaxMagAmmo && usReserveAmmo > 0 && bIsReloading == false && bIsActive == true)
	{
        //stop all attacking code
        AttackStop();
        AltAttackStop();


        pOwnerRef->GetPlayerAnimationComponent()->PlayReloadAnimation();

		AnimState = EAnimationState::Reloading;
        UpdateAnimation();
		//Setup relevant animations

		bIsReloading = true; //we are reloading

		//If attacking but someone presses 'R'
		if (bIsAttacking == true || bIsAltAttacking == true)
		{
			//stop attacking
			GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
			bIsAttacking = false;
			bIsAltAttacking = false;
		}

        //set reload sound and play
        NetMC_PlaySound(pReloadSound);

		//Set Reload Timer to match reload sound
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AGunBase::Reload, fReloadTime, false);

        //pOwnerRef->SetCanAccessInventory(false); //lock inventory until done reloading
	}
}

void AGunBase::Reload()
{
    
    //if we have more or equal to a full mag, reload accordingly
    if (usReserveAmmo + usMagAmmo >= usMaxMagAmmo)
    {
        //take current ammo and add to reserve
        usReserveAmmo += usMagAmmo;
        //set mag ammo to max
        usMagAmmo = usMaxMagAmmo;
        //remove full clip from reserve ammo
        usReserveAmmo -= usMaxMagAmmo;
    }
    //otherwise we are below a full mag, reload what we can
    else
    {
        usMagAmmo += usReserveAmmo;
        usReserveAmmo = 0;
    }

    //update ammo and hud text
    NetMC_UpdateText(usMagAmmo, usReserveAmmo);

    //Might need to set up a timer to call ReloadStop()
    //Ideally our ammo gets refilled once the clip is inserted, and we can't shoot until your gun returns to the normal state
    //but for now, based on reloadtime, you get your ammo setup and then we instantly call ReloadStop()
    ReloadStop();
}

void AGunBase::ReloadStop()
{
    AnimState = EAnimationState::Neutral;
    //Reset animations

    //we are done reloading
    bIsReloading = false;
    //let player access inventory again
    pOwnerRef->SetCanAccessInventory(true);

    //clear reload timer
    GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}

void AGunBase::AmmoRefill(uint16 ammo)
{
    //add ammo to reserves
    usReserveAmmo += ammo;
    //make sure we don't surpass our max reserve limit
    if (usReserveAmmo + usMagAmmo > usMaxReserveAmmo + usMaxMagAmmo)
        usReserveAmmo = usMaxReserveAmmo + usMaxMagAmmo - usMagAmmo; //sets ammo to have enough to fill your mag and have full ammo

    //For example: 20/40    current/reserve
    //             13/40
    //             13/47    40 + 20 - 13 = 47
    //             20/40

    //update ammo hud
    NetMC_UpdateText(usMagAmmo, usReserveAmmo);
}

void AGunBase::NetMC_UnEquip()
{
    //handle any stopping code first
    AttackStop();
    AltAttackStop();
    //stop attacking
    GetWorldTimerManager().ClearTimer(AttackTimer);
    //stop reload
    GetWorldTimerManager().ClearTimer(ReloadTimer);
    AnimState = EAnimationState::Neutral;
    bIsReloading = false;
    //unequip the weapon
    Super::NetMC_UnEquip();
}

bool AGunBase::DealDamage(const FHitResult* hit, float newDamage)
{
    //if we hit something
    if (hit->GetActor() != nullptr)
    {
        //print the name of thing we hit
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, hit->GetActor()->GetFName().ToString());

        //if actor we hit was an enemy
        if (hit->GetActor()->ActorHasTag("Enemy")) //dont forget!
		{
            //static cast to an enemy
            AAIEnemy* enemy = static_cast<AAIEnemy*>(hit->GetActor());

            //if enemy exists, deal damage
            if (enemy != nullptr)
            {
                //Blood Splatter/Visual Effect at hit.ImpactPoint?

                GEngine->AddOnScreenDebugMessage(6, 2.0f, FColor::Red, "Dealing Damage: " + FString::SanitizeFloat(newDamage));
                //ray trace line

                // Physic material (head and body)
                EPhysicalSurface SurfaceType = SurfaceType_Default;

                SurfaceType = UPhysicalMaterial::DetermineSurfaceType(hit->PhysMaterial.Get());

                if (SurfaceType == SURFACE_FLESHHEADSHOT) //if AI is shot in the head
                {
                    newDamage *= 2.0f; //double the damage if shot in the head
                }

                //deal newDamage to enemy we hit
                UGameplayStatics::ApplyDamage(hit->GetActor(), newDamage, pOwnerRef->GetController(), GetOwner(), DamageType);

                return true; //we hit enemy
            }
            return false; //enemy/exploder were nullptr
        }
        //if not an enemy, check for shield
        else if (AEnemyShield* shield = Cast<AEnemyShield>(hit->GetActor()))
        {
            //JUSTIN CODE
            if (shield)
            {
                shield->HurtShield(fDamage); //generic number, come back and apply to all weapons seperately i believe
                return true; //we hit shield
            }
            return false; //we did not hit a shield either
        }
        //Matt code; if not shield or enemy, test for boss
        else if (ABeenoDimeBoss* bossptr = Cast<ABeenoDimeBoss>(hit->GetActor()))
        {
            bossptr->DealDamage(this, newDamage);
        }
    }
    return false; //hit->GetActor() was nullptr
}

void AGunBase::MakeNoiseAI(const FHitResult* hit)
{
    // Enable enemy when shot // JUSTIN code ////////////
    AAIEnemy* aibase = Cast<AAIEnemy>(hit->GetActor()); //implement in all weapons
    if (aibase)
    {
        ABaseAICharacter_AIController* aicontroller = Cast<ABaseAICharacter_AIController>(aibase->GetController()); //ASK GROUP
        if (aicontroller)
        {
            aibase->bLookTowardsObjective = false;
            aicontroller->GetBlackboardComponent()->SetValueAsObject("TargetActor", GetOwner());
            aicontroller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
            aicontroller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
            GetOwner()->MakeNoise(0.1f, nullptr, hit->ImpactPoint);
            //UAISense_Dasmage::ReportDamageEvent(GetWorld(), HitActor, MyOwner->GetInstigatorController(), 1.f, GetActorLocation(), Hit.ImpactPoint); //player fire
        }
    }

    // JUSTIN code
    GetOwner()->MakeNoise(fItemLoudness);
}

void AGunBase::NetMC_FireExtras_Implementation(FVector TracerBeginPoint, FVector TracerEndPoint, UParticleSystem* pEffect, UNiagaraSystem* pImpact)
{
    if (!pOwnerRef->IsLocallyControlled())
    {
        //Add tracer effect
        if (pEffect)
        {
            UParticleSystemComponent* pTracerRef = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pEffect, TracerBeginPoint);
            //pTracerRef->SetVectorParameter("BeamEndPoint", TracerEndPoint);
            pTracerRef->SetVectorParameter(TracerTargetName, TracerEndPoint);
        }

        //add bullet impact
        if (pImpact)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), pImpact, TracerEndPoint);
        }
    }
}

void AGunBase::Client_FireExtras_Implementation(FVector TracerBeginPoint, FVector TracerEndPoint, UParticleSystem* pEffect, UNiagaraSystem* pImpact)
{
    //Add tracer effect
    if (pEffect)
    {
        UParticleSystemComponent* pTracerRef = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pEffect, TracerBeginPoint);
        //pTracerRef->SetVectorParameter("BeamEndPoint", TracerEndPoint);
        pTracerRef->SetVectorParameter(TracerTargetName, TracerEndPoint);
    }

    //add bullet impact
    if (pImpact)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), pImpact, TracerEndPoint);
    }
}

void AGunBase::PlayCameraShake_Implementation()
{
    if (pOwnerRef != nullptr)
    {
        //camera shake
        APlayerController* pc = Cast<APlayerController>(pOwnerRef->GetController());
        pc->PlayerCameraManager->PlayCameraShake(pCamShakeSubclass);
        //haptic feedback (VR and advanced controllers)
        pc->SetDisableHaptics(false);
        pc->PlayHapticEffect(haptic, EControllerHand::Left, 1.0f);
        pc->PlayHapticEffect(haptic, EControllerHand::Right, 1.0f);
        //force feedback (controllers)
        pc->ClientPlayForceFeedback(controllerForce);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "pOwnerRef was null on PlayCameraShake");
    }
}

//client side was 1 off from server side since fire is a server call, lowering ammo in here keeps clients up to date with server
void AGunBase::NetMC_UpdateText_Firing_Implementation()
{
    usMagAmmo--;
    //update ammo hud text
    UGunHud* ugunhud = Cast<UGunHud>(pGunHud->GetUserWidgetObject());
    if (ugunhud->IsValidLowLevel())
        ugunhud->UpdateText(usMagAmmo, usReserveAmmo);
}
//this function is for every other ammo hud update
void AGunBase::NetMC_UpdateText_Implementation(uint16 magAmmo, uint16 reserveAmmo)
{
    //update ammo hud text
    UGunHud* ugunhud = Cast<UGunHud>(pGunHud->GetUserWidgetObject());
    if (ugunhud->IsValidLowLevel())
        ugunhud->UpdateText(magAmmo, reserveAmmo);
}

void AGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGunBase, pTracerEffect);
	DOREPLIFETIME(AGunBase, pGunHud);
	DOREPLIFETIME(AGunBase, fCurrentBulletSpread);
	DOREPLIFETIME(AGunBase, usMagAmmo);
	DOREPLIFETIME(AGunBase, usMaxMagAmmo);
	DOREPLIFETIME(AGunBase, usReserveAmmo);
	DOREPLIFETIME(AGunBase, usMaxReserveAmmo);
}