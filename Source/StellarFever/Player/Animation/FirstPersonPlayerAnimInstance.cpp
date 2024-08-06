// Copyright Quarantine Games 2020

#include "FirstPersonPlayerAnimInstance.h"
#include "../BasePlayer.h"
#include "GameFramework/PawnMovementComponent.h"

void UFirstPersonPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (pOwningPlayer)
	{
		fMovementSpeedVelocity = pOwningPlayer->GetVelocity().Size();

		bIsDowned              = pOwningPlayer->GetIsPlayerDowned();
		bIsFalling             = pOwningPlayer->GetMovementComponent()->IsFalling();
		bIsHoldingPickup       = pOwningPlayer->GetIsPlayerHoldingItem();

		if (AWeaponBase* ActiveWeapon = pOwningPlayer->GetActiveWeapon())
		{
			eWeaponType = ActiveWeapon->GetWeaponType();
		}
		else
		{
			eWeaponType = EWeaponType::Gun;
		}
	}
}

void UFirstPersonPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	if (!ensure(Pawn != nullptr)) return;

	pOwningPlayer = Cast<ABasePlayer>(Pawn);
	if (!ensure(pOwningPlayer != nullptr)) return;
}