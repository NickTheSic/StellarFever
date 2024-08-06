// Coded by Nick Lemmon

#include "InventoryComponent.h"
#include "../Weapons/WeaponBase.h"
#include "../Weapons/GunBase.h"
#include "../Weapons/GrenadeBase.h"
#include "../Weapons/MeleeBase.h"
#include "../Player/BasePlayer.h"
#include "Net/UnrealNetwork.h"


#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::SetDisabledOrEnabled(int activate)
{
	// Check if we have the weapon then Enable it or Disable it

	if (pGun != nullptr)     activate == 0 ? pGun->NetMC_Equip()     : pGun->NetMC_UnEquip();
	if (pMelee != nullptr)   activate == 1 ? pMelee->NetMC_Equip()   : pMelee->NetMC_UnEquip();
	if (pGrenade != nullptr) activate == 2 ? pGrenade->NetMC_Equip() : pGrenade->NetMC_UnEquip();
}

void UInventoryComponent::PopulateInventory()
{
	UWorld* world = GetWorld();
	FActorSpawnParameters spawnParams;


	//Gun
	if (weaponTemplates.gunTemplate != nullptr)
	{
		AWeaponBase* gun = world->SpawnActor<AWeaponBase>(weaponTemplates.gunTemplate, spawnParams);
		AddItemToInventory(gun);
	}
	else
	{
		print("No gun template added");
	}


	//Melee
	if (weaponTemplates.meleeTemplate != nullptr)
	{
		AWeaponBase* melee = world->SpawnActor<AWeaponBase>(weaponTemplates.meleeTemplate, spawnParams);
		AddItemToInventory(melee);
	}
	else
	{
		print("No melee template added");
	}


	//Grenade
	if (weaponTemplates.grenadeTemplate != nullptr)
	{
		AWeaponBase* grenade = world->SpawnActor<AWeaponBase>(weaponTemplates.grenadeTemplate, spawnParams);
		AddItemToInventory(grenade);
	}
	else
	{
		print("No grenade template added");
	}


	SetupInventoryAfterAllItemsHaveBeenAdded();
}

void UInventoryComponent::ReplenishAmmo(uint16 ammo)
{
	if (AGunBase* gun = Cast<AGunBase>(pGun))
	{
		gun->AmmoRefill(ammo);
	}

	if (AGrenadeBase* grenade = Cast<AGrenadeBase>(pGrenade))
	{
		grenade->GiveGrenade();
	}

}

AWeaponBase* UInventoryComponent::SwitchToGun()
{
	if (bCanAccessInventory)
	{
		// If we have a gun we can grab it
		if (pGun != nullptr)
		{
			cCurrentIndex = 0;
			UpdateActiveWeaponEnabled();
			return pGun;
		}
		else if (allWeapons.IsValidIndex(cCurrentIndex))
		{
			//if we don't have a gun we can just return the current weapon
			return allWeapons[cCurrentIndex];
		}
		print ("No gun or active weapon")
	}

	print("Can't gun access Inventory");
	return nullptr;
}

AWeaponBase* UInventoryComponent::SwitchToMelee()
{
	if (bCanAccessInventory)
	{
		//If we have a melee item we can activate it and return it
		if (pMelee != nullptr)
		{
			cCurrentIndex = 1;
			UpdateActiveWeaponEnabled();
			return pMelee;
		}
		else if (allWeapons.IsValidIndex(cCurrentIndex))
		{
			//Lets return the currently active weapon
			return allWeapons[cCurrentIndex];
		}
		//We have no melee or currently active weapon
		print("No melee or active weapon");
	}

	print("Can't access melee Inventory");
	return nullptr;
}

AWeaponBase* UInventoryComponent::SwitchToGrenade()
{
	// If we can access the inventory
	if (bCanAccessInventory)
	{
		//If the grenade isn't null we can just return it
		if (pGrenade != nullptr)
		{
			AGrenadeBase* grenade = static_cast<AGrenadeBase*>(pGrenade);
			//Make sure we haven't thrown the grenade before switching to it
			//BUT if you're a gunner, you need to equip the grenade to detonate it
			if (grenade->HasGrenade())
			{
				cCurrentIndex = 2;
				UpdateActiveWeaponEnabled();
				return pGrenade;
			}
			//If we have thrown our grenade, return current weapon
			else if (allWeapons.IsValidIndex(cCurrentIndex))
			{
				//Just return the current weapon
				print("Grenade was null, returning current weapon")
					//MIGHT Cause Issues with Throwing a Grenade!
					return allWeapons[cCurrentIndex];
			}
		}
		else if(allWeapons.IsValidIndex(cCurrentIndex))
		{
			//Just return the current weapon
			print("Grenade was null, returning current weapon")
				//MIGHT Cause Issues with Throwing a Grenade!
			return allWeapons[cCurrentIndex];
		}
		//There was no grenade or active weapon
		print("Both the grenade and current weapon were null");
	}

	print("Can't access grenade Inventory");
	return nullptr;
}

AWeaponBase* UInventoryComponent::SwitchToNext()
{
	//If we can't access the inventory return a nullptr and do nothing
	if (!bCanAccessInventory)
	{
		print("Can't access Inventory");
		return nullptr;
	}

	//We can access the inventory so do a bounds check
	cCurrentIndex++;
	if (cCurrentIndex >= allWeapons.Num())
	{
		cCurrentIndex = 0;
	}

	//If we are at an invalid index we return a nullptr
	//I don't think we should get here but I have to check for it anyway
	if (!allWeapons.IsValidIndex(cCurrentIndex))
	{
		print("Invalid index");
		return nullptr;
	}


	//TODO: New code that hopefully stops
	if (AGrenadeBase* grenade = Cast<AGrenadeBase>(allWeapons[cCurrentIndex]))
	{
		if (grenade->HasGrenade() == false)
		{
			cCurrentIndex = 0;

			if (!allWeapons.IsValidIndex(cCurrentIndex))
			{
				print("Invalid index");
				return nullptr;
			}
		}
	}

	//Activate the newly active weapon and return it
	UpdateActiveWeaponEnabled();
	return allWeapons[cCurrentIndex];
}

AWeaponBase* UInventoryComponent::SwitchToPrev()
{
	//We can't access the inventory so we return a nullptr
	if (!bCanAccessInventory)
	{
		print("Can't access Inventory");
		return nullptr;
	}

	//Change the current index and do a bounds check
	cCurrentIndex--;
	if (cCurrentIndex < 0)
	{
		//Set the current index to one less than the amount we have in the inventory
		cCurrentIndex = allWeapons.Num() - 1;
	}

	//We should check to make sure it is a valid index
	if (!allWeapons.IsValidIndex(cCurrentIndex))
	{
		print("Invalid index");
		return nullptr;
	}


	//TODO: New code that hopefully stops
	if (AGrenadeBase* grenade = Cast<AGrenadeBase>(allWeapons[cCurrentIndex]))
	{
		if (grenade->HasGrenade() == false)
		{
			cCurrentIndex--;

			if (!allWeapons.IsValidIndex(cCurrentIndex))
			{
				print("Invalid index");
				return nullptr;
			}
		}
	}


	//Update the active weapon and return it
	UpdateActiveWeaponEnabled();
	return allWeapons[cCurrentIndex];
}

AWeaponBase* UInventoryComponent::GetLastActiveWeapon()
{
	//Check to make sure that we still have it at a valid index
	if (allWeapons.IsValidIndex(cCurrentIndex))
	{
		UpdateActiveWeaponEnabled();
		return allWeapons[cCurrentIndex];
	}

	return pGun;
}

void UInventoryComponent::ThrowGrenade()
{
	if(pGrenade)
		pGrenade->AttackStop();
}

void UInventoryComponent::AddItemToInventory(AWeaponBase* weapon)
{
	//Let's check if it is a nullptr in case somewhere along the lines we passed in something null
	if (weapon != nullptr)
	{
		allWeapons.Add(weapon);

		ABasePlayer* owner = Cast<ABasePlayer>(GetOwner());
		if (owner != nullptr)
		{
			weapon->Server_SetWeaponOwner(owner);
			switch (owner->GetPlayerInformation().ePlayerClass)
			{
				case EPlayerClass::Captain:
					weapon->SetupSocketAttachments(owner->GetFirstPersonMesh(), owner->GetMesh(), "CaptainGunSocket");
					break;
				case EPlayerClass::Engineer:
					weapon->SetupSocketAttachments(owner->GetFirstPersonMesh(), owner->GetMesh(), "EngineerGunSocket");
					break;
				case EPlayerClass::Gunner:
					weapon->SetupSocketAttachments(owner->GetFirstPersonMesh(), owner->GetMesh(), "GunGunSocket");
					break;
				case EPlayerClass::Medic:
					weapon->SetupSocketAttachments(owner->GetFirstPersonMesh(), owner->GetMesh(), "MedicGunSocket");
					break;
			}

			//Based on player class, attach weapon to appropriate socket. Otherwise just attach and hope.
					//case EPlayerClass::Captain: weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GunSocket"); break;
			/*switch(owner->GetPlayerInformation().ePlayerClass) {
				case EPlayerClass::Captain: weapon->AttachToComponent(owner->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "CaptainGunSocket");
					weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
				case EPlayerClass::Engineer: weapon->AttachToComponent(owner->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "EngineerGunSocket");
					weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
				case EPlayerClass::Gunner: weapon->AttachToComponent(owner->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GunGunSocket");
					weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
				case EPlayerClass::Medic: weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm");
					weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Marine002RPalm"); break;
				default: weapon->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			}*/
			
		}

		//Disable the weapon as it enters the inventory
		weapon->NetMC_UnEquip();
	}
}

void UInventoryComponent::SetupInventoryAfterAllItemsHaveBeenAdded()
{
	//This is to setup the actual weapon pointers and do a double check that everything was added
	if (allWeapons.IsValidIndex(0))
	{
		if (allWeapons[0] != nullptr)
			pGun = allWeapons[0];
	}
	else
	{
		print("Invalid index at 0 for pGun")
	}

	if (allWeapons.IsValidIndex(1))
	{
		if (allWeapons[1] != nullptr)
			pMelee = allWeapons[1];
	}
	else
	{
		print("Invalid index at 1 for pMelee")
	}

	if (allWeapons.IsValidIndex(2))
	{
		if (allWeapons[2] != nullptr)
		{
			pGrenade = allWeapons[2];
		}
	}
	else
	{
		print("Invalid index at 2 for pGrenade")
	}

}

void UInventoryComponent::EnableInventory()
{
	print("Enabling inventory");

	//We can access the inventory again so lets Enable the weapon at the current index 
	bCanAccessInventory = true;
	if (allWeapons.IsValidIndex(cCurrentIndex))
	{
		if (allWeapons[cCurrentIndex] != nullptr)
		{
			allWeapons[cCurrentIndex]->NetMC_Equip();
		}
	}
}

void UInventoryComponent::DisableInventory()
{
	print("Disabling inventory");

	//We have disabled the inventory so disable the current weapon
	bCanAccessInventory = false;
	if (allWeapons.IsValidIndex(cCurrentIndex))
	{
		if (allWeapons[cCurrentIndex] != nullptr)
		{
			allWeapons[cCurrentIndex]->NetMC_UnEquip();
		}
	}
}

#undef print

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME( UInventoryComponent,          allWeapons); //Maybe not since the other pointers are replicated?
	DOREPLIFETIME( UInventoryComponent,                pGun); 
	DOREPLIFETIME( UInventoryComponent,              pMelee);
	DOREPLIFETIME( UInventoryComponent,            pGrenade);
	DOREPLIFETIME( UInventoryComponent,       cCurrentIndex); 
	DOREPLIFETIME( UInventoryComponent, bCanAccessInventory);

}
