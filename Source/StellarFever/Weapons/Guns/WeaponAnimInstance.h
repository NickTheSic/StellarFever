// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		class AWeaponBase* pWeaponReference = nullptr;
public:
	UFUNCTION(BlueprintCallable)
		void SetWeaponReference(class AWeaponBase* weapon) { pWeaponReference = weapon; }
	UFUNCTION(BlueprintCallable)
		AWeaponBase* GetWeaponReference() const { return pWeaponReference; }
};
