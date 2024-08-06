// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "../Player/ClassWeaponTemplates.h"
#include "../Weapons/GunBase.h"
#include "../Weapons/MeleeBase.h"
#include "../Weapons/GrenadeBase.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	/*A stuct to hold each of the weapon templates in a single variable here*/
	UPROPERTY(EditAnywhere, Category = "Weapon Templates")
		FClassWeaponTemplates weaponTemplates;

protected:
	/*An array for all the weapons that are part of the inventory*/
	UPROPERTY(VisibleAnywhere, Replicated)
		TArray<class AWeaponBase*> allWeapons;

	/*A direct pointer to the Gun weapon that the inventory has*/
	UPROPERTY(VisibleAnywhere, Replicated)
		class AWeaponBase* pGun     = nullptr;
	
	/*A Direct pointer to the melee weapon that is stored in the inventory*/
	UPROPERTY(VisibleAnywhere, Replicated)
		class AWeaponBase* pMelee   = nullptr;

	/*A pointer to the grenade item that is stored in the inventory*/
	UPROPERTY(VisibleAnywhere, Replicated)
		class AWeaponBase* pGrenade = nullptr;

	/*The index in the array that is currently active.  Replicated to clients*/
	UPROPERTY(Replicated)
		int8 cCurrentIndex; 

	/*A variable to store if the player is able to access the inventory. 
	Since to get the weapon in player they go through the inventory*/
	UPROPERTY(Replicated)
		bool bCanAccessInventory = true;

public:	

	/*A Publicly callable function so that no other class needs to know about the current index*/
	void UpdateActiveWeaponEnabled() { SetDisabledOrEnabled(cCurrentIndex); };
		
	/*A wrapper function to directly refill the ammo in the gun without directly trying to get the gun*/
	void ReplenishAmmo(uint16 ammo);

	//Functions for switching between items
	//Calls equip and internally changes variables of the active weapon
	class AWeaponBase* SwitchToGun();
	class AWeaponBase* SwitchToMelee();
	class AWeaponBase* SwitchToGrenade();

	class AWeaponBase* SwitchToNext();
	class AWeaponBase* SwitchToPrev();

	//This switches to and returns the last active weapon
	class AWeaponBase* GetLastActiveWeapon();


	//These functions return the pointer directly
	//without calling equip and unequip like the SwitchTo functions
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AGunBase*     GetGun()      { return Cast<AGunBase>(pGun); };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AMeleeBase*   GetMelee()    { return Cast<AMeleeBase>(pMelee); };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AGrenadeBase* GetGrenade()  { return Cast<AGrenadeBase>(pGrenade); };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AWeaponBase*  GetNextItem() { return allWeapons[(cCurrentIndex + 1) % allWeapons.Num()]; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AWeaponBase*  GetPrevItem() { return allWeapons[(allWeapons.Num() + (cCurrentIndex - 1)) % allWeapons.Num()]; };


	UFUNCTION(meta = (DeprecatedFunction))
	void ThrowGrenade();

	void AddItemToInventory(class AWeaponBase* weapon);

	void EnableInventory();
	void DisableInventory();


	//A setter function for setting the bCanAccessInventory bool
	void SetCanAccessInventory(bool bAccess) { bCanAccessInventory = bAccess; }

	//A Getter function for getting the bCanAccessInventory bool
	FORCEINLINE bool CanAccessInventory() { return bCanAccessInventory; }

	//A public function to call a private function
	void SetupInventory() { PopulateInventory(); }

	//This function handles setting up internal variables for the inventory
	void SetupInventoryAfterAllItemsHaveBeenAdded();

private:
	void SetDisabledOrEnabled(int activate);

	//The private function to add all items and populate the inventory
	void PopulateInventory();

};
