// Copyright Quarantine Games 2020

#include "PlayerAnimInstance.h"
#include "../BasePlayer.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (pOwningPlayer)
	{
		fMovementSpeedVelocity    = pOwningPlayer->GetVelocity().Size();
		fMovementDirection        = CalculateDirection(pOwningPlayer->GetVelocity(), pOwningPlayer->GetActorRotation());


		bIsDead          = pOwningPlayer->GetIsPlayerDead();
		bIsDowned        = pOwningPlayer->GetIsPlayerDowned();
		bIsFalling       = pOwningPlayer->GetMovementComponent()->IsFalling();
		bIsCrouched      = pOwningPlayer->GetMovementComponent()->IsCrouching();
		bIsHoldingPickup = pOwningPlayer->GetIsPlayerHoldingItem();

		bShouldBlendLower = (fMovementSpeedVelocity <= 15.0f && !bIsCrouched);

		if (AWeaponBase* ActiveWeapon = pOwningPlayer->GetActiveWeapon())
		{
			eWeaponType = ActiveWeapon->GetWeaponType();
		}
		else
		{
			eWeaponType = EWeaponType::Gun;
		}
	}
	else
	{
		//This appears while playing in the event graph which is something that I don't want
		//UE_LOG(LogTemp, Warning, TEXT("The owning player for animation was null"));
	}
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	if (!ensure(Pawn != nullptr)) return;

	pOwningPlayer = Cast<ABasePlayer>(Pawn);
	if (!ensure(pOwningPlayer != nullptr)) return;

}

void UPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerAnimInstance,             bIsFalling);
	DOREPLIFETIME(UPlayerAnimInstance,           bIsReloading);
	DOREPLIFETIME(UPlayerAnimInstance,            bIsReviving);
	DOREPLIFETIME(UPlayerAnimInstance,              bIsDowned);
	DOREPLIFETIME(UPlayerAnimInstance,                bIsDead);
	DOREPLIFETIME(UPlayerAnimInstance,       bIsHoldingPickup);
	DOREPLIFETIME(UPlayerAnimInstance,            bIsCrouched);
	DOREPLIFETIME(UPlayerAnimInstance,       bShouldBlendLower);
									        
	DOREPLIFETIME(UPlayerAnimInstance,            eWeaponType);

	DOREPLIFETIME(UPlayerAnimInstance, fMovementSpeedVelocity);
	DOREPLIFETIME(UPlayerAnimInstance,     fMovementDirection);
}
