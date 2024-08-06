// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../../Weapons/WeaponBase.h"
#include "FirstPersonPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UFirstPersonPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsReviving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsDowned = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EWeaponType eWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsHoldingPickup = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fMovementSpeedVelocity;

	UPROPERTY()
	class ABasePlayer* pOwningPlayer = nullptr;

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
