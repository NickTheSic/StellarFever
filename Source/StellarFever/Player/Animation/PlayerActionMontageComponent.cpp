// Copyright Quarantine Games 2020

#include "PlayerActionMontageComponent.h"
#include "../BasePlayer.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"


UPlayerActionMontageComponent::UPlayerActionMontageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPlayerActionMontageComponent::SetupOwningBasePlayer(ABasePlayer* owner)
{
	pOwningPlayer = owner;

	if (!ensure(pOwningPlayer != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("The owning player was a nullptr, animations will not be setup"));
		return;
	}

	//Setup the first person animation instance
	if (USkeletalMeshComponent* fpMesh = pOwningPlayer->GetFirstPersonMesh())
	{
		FirstPersonAnimInstance = fpMesh->GetAnimInstance();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to retrieve the first person animation instance from the mesh"));
	}

	//Setup third person animation instance
	if (USkeletalMeshComponent* tpMesh = pOwningPlayer->GetMesh())
	{
		ThirdPersonAnimInstance = tpMesh->GetAnimInstance();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to retrieve the third person animation instance from the mesh"));
	}
}


void UPlayerActionMontageComponent::PlayAttackAnimation(EWeaponType weaponType, bool bIsAltFire /*= false*/)
{
	switch (weaponType)
	{
	case EWeaponType::Gun:
		if(bIsAltFire == false)
			PlayGunAttack();
		break;

	case EWeaponType::Melee:
		bIsAltFire ? PlayAltMeleeAttack() : PlayMeleeAttack();
		break;

	case EWeaponType::Grenade:
		if (bIsAltFire == false)
			PlayGrenadeThrow();
		break;

	default:
		break;
	}
}



#undef IMPLEMENTPLAYFUNCTION
#define IMPLEMENTPLAYFUNCTION(func, montage)                                                                                \
void UPlayerActionMontageComponent::Play##func(float PlayRate/*= 1.0f*/){                                                   \
	Client_Play##func(PlayRate);                                                                                            \
	NMC_Play##func(PlayRate);																	                            \
}                                                                                                                           \
void UPlayerActionMontageComponent::NMC_Play##func##_Implementation(float PlayRate)											\
{                                                                                                                           \
	if(ThirdPersonAnimInstance != nullptr)																					\
	{                                                                                                                       \
		if(montage##_ThirdPerson != nullptr)																				\
		{                                                                                                                   \
			ThirdPersonAnimInstance->Montage_Play(montage##_ThirdPerson, PlayRate); 										\
		}                                                                                                                   \
		else																												\
		{																													\
			UE_LOG(LogTemp, Warning, TEXT("The third person montage was null and will not be played"));						\
		}                                                                                                                   \
	}																														\
	else																													\
	{																														\
	UE_LOG(LogTemp, Warning, TEXT("The ThirdPersonAnimInstance was null and will not play the montage"));					\
	}                                                                                                                       \
}                                                                                                                           \
void UPlayerActionMontageComponent::Client_Play##func##_Implementation(float PlayRate)										\
{                                                                                                                           \
	if(FirstPersonAnimInstance != nullptr)																					\
	{                                                                                                                       \
		if(montage##_FirstPerson != nullptr)																				\
		{                                                                                                                   \
			FirstPersonAnimInstance->Montage_Play(montage##_FirstPerson, PlayRate); 										\
		}                                                                                                                   \
		else																												\
		{																													\
			UE_LOG(LogTemp, Warning, TEXT("The first person montage you tried to play was null and will not be played"));	\
		}                                                                                                                   \
	}																														\
	else																													\
	{																														\
		UE_LOG(LogTemp, Warning, TEXT("The FirstPersonAnimInstance was null and will not play montage"));					\
	}                                                                                                                       \
}


IMPLEMENTPLAYFUNCTION(ReloadAnimation,           GunReloadMontage);
IMPLEMENTPLAYFUNCTION(EventInteractionAnimation, EventInteractionMontage);
IMPLEMENTPLAYFUNCTION(WeaponSwitchAnimation,     WeaponSwitchMontage);
IMPLEMENTPLAYFUNCTION(GunAttack,                 GunAttackMontage);
IMPLEMENTPLAYFUNCTION(GrenadeThrow,              GrenadeThrowMontage);
IMPLEMENTPLAYFUNCTION(AltMeleeAttack,            AltMeleeAttackMontage);
IMPLEMENTPLAYFUNCTION(AltGunAttack,              AltGunAttackMontage);
IMPLEMENTPLAYFUNCTION(PlanetSelectAnimation,     PlanetSelectMontage);
IMPLEMENTPLAYFUNCTION(ClassAbilityAnimation,     ClassAbilityMontage);

#undef IMPLEMENTPLAYFUNCTION


void UPlayerActionMontageComponent::PlayMeleeAttack(float PlayRate/*= 1.0f*/)
{
	Client_PlayMeleeAttack(PlayRate);
	NMC_PlayMeleeAttack(PlayRate);
}

void UPlayerActionMontageComponent::NMC_PlayMeleeAttack_Implementation(float PlayRate /*= 1.0f*/)
{
	if (ThirdPersonAnimInstance != nullptr)
	{
		int randomPlay = FMath::RandRange(0, MeleeAttackMontage_ThirdPerson.Num() - 1); //-1 so we don't get the Num, just 1 less for the proper index

		if (MeleeAttackMontage_ThirdPerson.IsValidIndex(randomPlay) && MeleeAttackMontage_ThirdPerson[randomPlay] != nullptr)
		{
			ThirdPersonAnimInstance->Montage_Play(MeleeAttackMontage_ThirdPerson[randomPlay], PlayRate);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The MeleeAttackMontage_ThirdPerson was null and will not be played"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The ThirdPersonAnimInstance was null and will not play the montage"));
	}
}

void UPlayerActionMontageComponent::Client_PlayMeleeAttack_Implementation(float PlayRate/*= 1.0f*/)
{
	if (FirstPersonAnimInstance != nullptr)
	{
		if (MeleeAttackMontage_FirstPerson != nullptr)
		{
			FirstPersonAnimInstance->Montage_Play(MeleeAttackMontage_FirstPerson, PlayRate);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The MeleeAttackMontage_FirstPerson was null and will not be played"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The FirstPersonAnimInstance was null and will not play the montage"));
	}
}

