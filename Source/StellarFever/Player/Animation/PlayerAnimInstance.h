// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../../Weapons/WeaponBase.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsCrouched = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsReviving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsDowned = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsDead = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		EWeaponType eWeaponType;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bIsHoldingPickup = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float fMovementSpeedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float fMovementDirection;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		bool bShouldBlendLower = false;


	UPROPERTY()
	class ABasePlayer* pOwningPlayer = nullptr;
	

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
