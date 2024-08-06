// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "../Weapons/WeaponBase.h"
#include "ClassWeaponTemplates.generated.h"

/**
 * 
 */
USTRUCT()
struct STELLARFEVER_API FClassWeaponTemplates
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Weapon Templates")
		TSubclassOf<class AWeaponBase> gunTemplate = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon Templates")
		TSubclassOf<class AWeaponBase> meleeTemplate = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon Templates")
		TSubclassOf<class AWeaponBase> grenadeTemplate = nullptr;

};
