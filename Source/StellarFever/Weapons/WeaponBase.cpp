// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/StaticMeshComponent.h"
#include "../Player/BasePlayer.h"
#include "../StellarFeverGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Animation/AnimationAsset.h"
#include "../Player/Animation/PlayerActionMontageComponent.h"
#include "../Components/PlayerSoundComponent.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	//pAnimation = CreateDefaultSubobject<UAnimSequence>("Animation");	//causes errors if no animation is set up. rTodo: will have to test once animations are implemented

	pMesh = CreateDefaultSubobject<USkeletalMesh>("Weapon Mesh");

	pTPRenderedMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Third Person Rendered Weapon Mesh");
	pTPRenderedMesh->SetCollisionProfileName("NoCollision");
	pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pTPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pTPRenderedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pTPRenderedMesh->SetIsReplicated(true);
	//pTPRenderedMesh->SetDisablePostProcessBlueprint(true);
	pTPRenderedMesh->bRenderCustomDepth = true;
	pTPRenderedMesh->SetCustomDepthStencilValue(0);
	pTPRenderedMesh->bOwnerNoSee = true;
	//pTPRenderedMesh->SetDepthPriorityGroup(ESceneDepthPriorityGroup::SDPG_Foreground);
	//pTPRenderedMesh->ViewOwnerDepthPriorityGroup = ESceneDepthPriorityGroup::SDPG_Foreground;
	RootComponent = pTPRenderedMesh;

	pFPRenderedMesh = CreateDefaultSubobject<USkeletalMeshComponent>("First Person Rendered Weapon Mesh");
	pFPRenderedMesh->SetCollisionProfileName("NoCollision");
	pFPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pFPRenderedMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pFPRenderedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pFPRenderedMesh->SetIsReplicated(true);
	pFPRenderedMesh->SetDisablePostProcessBlueprint(true);
	pFPRenderedMesh->bRenderCustomDepth = true;
	pFPRenderedMesh->bOnlyOwnerSee = true;
	//pFPRenderedMesh->SetHiddenInGame(true);
	pFPRenderedMesh->SetCustomDepthStencilValue(0);

	//rTodo: attach to first person mesh
	pFPRenderedMesh->SetupAttachment(RootComponent);

	//will follow a socket on the weapon
	pParticleEmitter = CreateDefaultSubobject<UNiagaraSystem>("Niagara Emitter");
	
	pAudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	pAudioComponent->SetupAttachment(RootComponent);
	pAudioComponent->SetIsReplicated(true);

	fActivationTime = 0.5f;

	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	//set mesh and physics asset
	pTPRenderedMesh->SetSkeletalMesh(pMesh);
	pTPRenderedMesh->SetPhysicsAsset(pPhysAsset);
	pFPRenderedMesh->SetSkeletalMesh(pMesh);
	pFPRenderedMesh->SetPhysicsAsset(pPhysAsset);

	pAudioComponent->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pGeneralSoundClass;

}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Server_SetWeaponOwner_Implementation(ABasePlayer* wepOwner)
{
	//Set owner of weapon
	SetOwner(wepOwner);
	//Set owner reference
	pOwnerRef = wepOwner;

	if (pOwnerRef)
	{
		pTPRenderedMesh->IgnoreActorWhenMoving(pOwnerRef, true);
		pTPRenderedMesh->IgnoreComponentWhenMoving(pOwnerRef->GetCapsuleComponent(), true);
		pFPRenderedMesh->IgnoreActorWhenMoving(pOwnerRef, true);
		pFPRenderedMesh->IgnoreComponentWhenMoving(pOwnerRef->GetCapsuleComponent(), true);
	}
}

//bool AWeaponBase::Server_SetWeaponOwner_Validate()
//{
//	return true;
//}

//will likely need to net_multicast this
void AWeaponBase::NetMC_Enable_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetWorld()->GetTimerManager().SetTimer(activationTimer, this, &AWeaponBase::NetMC_Activate, fActivationTime, false);
}

void AWeaponBase::NetMC_Activate_Implementation()
{
	bIsActive = true;
}

//will likely need to net_multicast this
void AWeaponBase::NetMC_Disable_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	bIsActive = false;
}

//bool AWeaponBase::Server_Disable_Validate()
//{
//	return true;
//}

void AWeaponBase::NetMC_Equip_Implementation()
{
	//Enable our weapon
	NetMC_Enable();
	//set proper animation state
	AnimState = EAnimationState::Equipping;

	//play proper animation
	AnimState = EAnimationState::Equipping;
	UpdateAnimation();
}

//bool AWeaponBase::NetMC_Equip_Validate()
//{
//	return true;
//}

void AWeaponBase::NetMC_UnEquip_Implementation()
{
	//if we switch weapons while attacking or alt attacking, clear attack timer (timer used by both)
	if (bIsAttacking == true || bIsAltAttacking == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	}

	//stops all audio when we switch weapons
	if (pAudioComponent != nullptr)
		pAudioComponent->Stop();

	//Disable our weapon
	NetMC_Disable();

	//set proper animation state
	AnimState = EAnimationState::Neutral;
	//We don't need to play an unequip animation because we already hid the weapon
}

//bool AWeaponBase::NetMC_UnEquip_Validate()
//{
//	return true;
//}

//mainly for override purposes
void AWeaponBase::ResetVars()
{
	bIsAttacking = false;
	bIsAltAttacking = false;
}

void AWeaponBase::AttackStart()
{

}

void AWeaponBase::Attack()
{
	pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type);
}

void AWeaponBase::AttackStop()
{

}

void AWeaponBase::AltAttackStart()
{

}

void AWeaponBase::AltAttack()
{
	pOwnerRef->GetPlayerAnimationComponent()->PlayAttackAnimation(Type, true);
	switch (Type)
	{
	case EWeaponType::Gun: pOwnerRef->GetPlayerSoundComponent()->PlayAltFire(); break;
	case EWeaponType::Melee: pOwnerRef->GetPlayerSoundComponent()->PlayAltMelee(); break;
	default: break;
	}
	
}

void AWeaponBase::AltAttackStop()
{

}

void AWeaponBase::NetMC_PlaySound_Implementation(USoundCue* sound)
{
	if (pAudioComponent)
	{
		pAudioComponent->SetSound(sound);
		pAudioComponent->Play();
	}
}

void AWeaponBase::NetMC_StopSound_Implementation()
{
	if (pAudioComponent->IsPlaying())
		pAudioComponent->Stop();
}

//plays a sound attached to the main mesh
void AWeaponBase::NetMC_PlaySoundSingle_Implementation(USoundCue* sound)
{
	UGameplayStatics::SpawnSoundAttached(sound, pTPRenderedMesh);
}

void AWeaponBase::UpdateAnimation()
{
	//Called when our Animation State Changes
	//Based on AnimState, swap to appropriate animation
	switch (AnimState)
	{
	case EAnimationState::Neutral: break;
	case EAnimationState::Charging: break;
	case EAnimationState::Attacking: break;
	case EAnimationState::Equipping: break;
	case EAnimationState::CoolingDown: break;
	case EAnimationState::Reloading: break;
	default: return;
	};
}

//play a particle effect attached to a given mesh
void AWeaponBase::NetMC_PlayParticleEffectAttached_Implementation(UNiagaraSystem* particleEffect, USceneComponent* mesh, FName boneName, FVector pos, FVector scale, FRotator rot)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(particleEffect, mesh, boneName, pos, rot, scale, EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

//play a particle effect attached to a given mesh
void AWeaponBase::NetMC_PlayGunParticleEffectAttached_Implementation(UNiagaraSystem* particleEffect, USceneComponent* mesh, FName boneName, FVector pos, FVector scale, FRotator rot)
{
	if (!pOwnerRef->IsLocallyControlled())
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(particleEffect, mesh, boneName, pos, rot, scale, EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
	}
}

//play a particle effect attached to a given mesh
void AWeaponBase::Client_PlayParticleEffectAttached_Implementation(UNiagaraSystem* particleEffect, USceneComponent* mesh, FName boneName, FVector pos, FVector scale, FRotator rot)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(particleEffect, mesh, boneName, pos, rot, scale, EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

//play a particle effect at a given point
void AWeaponBase::NetMC_PlayParticleEffectAtLocation_Implementation(UNiagaraSystem* particleEffect, FVector pos)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), particleEffect, pos);
}

//play a particle effect at a given point
void AWeaponBase::Client_PlayParticleEffectAtLocation_Implementation(UNiagaraSystem* particleEffect, FVector pos)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), particleEffect, pos);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//in case of weapon attachments, then these get changed on startup, might need to be replicated for that change to get updated
	DOREPLIFETIME(AWeaponBase, fDamage);
	DOREPLIFETIME(AWeaponBase, fAttackRate);

	DOREPLIFETIME(AWeaponBase, pTPRenderedMesh);
	DOREPLIFETIME(AWeaponBase, pFPRenderedMesh);

	//particles and audio must be known across clients, I'm assuming these need to be replicated for that
	DOREPLIFETIME(AWeaponBase, pParticleEmitter);
	DOREPLIFETIME(AWeaponBase, pAudioComponent);

	DOREPLIFETIME(AWeaponBase, bIsActive); //need to know which weapon is active

	DOREPLIFETIME(AWeaponBase, pOwnerRef);
}

void AWeaponBase::SetupSocketAttachments(USkeletalMeshComponent* FPMesh, USkeletalMeshComponent* TPMesh, FString socket)
{
	pFPRenderedMesh->AttachToComponent(FPMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(socket));
	pTPRenderedMesh->AttachToComponent(TPMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(socket));
}