// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "../Animation/PlayerActionMontageComponent.h"
#include "../../Components/InventoryComponent.h"
#include "Components/WidgetInteractionComponent.h"


#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif


/* Inventory Switching */

void ABasePlayer::MouseWheelUp()
{
	if (GetIsPlayerInMenu())
	{
		if (pWidgetInteractionComponent->IsOverInteractableWidget())
		{
			pWidgetInteractionComponent->ScrollWheel(1); //Simulate scrolling on UI
		}
		return;
	}

	Server_MouseWheelUp();
}

void ABasePlayer::Server_MouseWheelUp_Implementation()
{
	if (pInventoryComponent->CanAccessInventory())
	{
		pActiveWeapon = pInventoryComponent->SwitchToPrev();

		if (pPlayerAnimComponent != nullptr)
		{
			pPlayerAnimComponent->PlayWeaponSwitchAnimation();
		}
	}
	//else notify player?
}


void ABasePlayer::MouseWheelDown()
{
	if (GetIsPlayerInMenu())
	{
		if (pWidgetInteractionComponent->IsOverInteractableWidget())
		{
			pWidgetInteractionComponent->ScrollWheel(-1); //Simulate scrolling on UI
		}
		return;
	}

	Server_MouseWheelDown();
}

void ABasePlayer::Server_MouseWheelDown_Implementation()
{
	if (pInventoryComponent->CanAccessInventory())
	{
		pActiveWeapon = pInventoryComponent->SwitchToNext();

		if (pPlayerAnimComponent != nullptr)
		{
			pPlayerAnimComponent->PlayWeaponSwitchAnimation();
		}
	}
	//else notify player?
}


void ABasePlayer::SwitchToSlotOne()
{
	Server_SwitchToSlotOne();
}

void ABasePlayer::Server_SwitchToSlotOne_Implementation()
{
	if (pInventoryComponent->CanAccessInventory())
	{
		pActiveWeapon = pInventoryComponent->SwitchToGun();

		if (pPlayerAnimComponent != nullptr)
		{
			pPlayerAnimComponent->PlayWeaponSwitchAnimation();
		}
	}
	//else notify player?
}


void ABasePlayer::SwitchToSlotTwo()
{
	Server_SwitchToSlotTwo();
}

void ABasePlayer::Server_SwitchToSlotTwo_Implementation()
{
	if (pInventoryComponent->CanAccessInventory())
	{
		pActiveWeapon = pInventoryComponent->SwitchToMelee();

		if (pPlayerAnimComponent != nullptr)
		{
			pPlayerAnimComponent->PlayWeaponSwitchAnimation();
		}
	}
	//else notify player?
}


void ABasePlayer::SwitchToSlotThree()
{
	Server_SwitchToSlotThree();
}

void ABasePlayer::Server_SwitchToSlotThree_Implementation()
{
	//if you're not a gunner, switch normally
	if (playerInformation.ePlayerClass != EPlayerClass::Gunner)
	{
		if (pInventoryComponent->CanAccessInventory())
		{
			pActiveWeapon = pInventoryComponent->SwitchToGrenade();

			if (pPlayerAnimComponent != nullptr)
			{
				pPlayerAnimComponent->PlayWeaponSwitchAnimation();
			}
		}
	}
	//if you are a gunner
	else
	{
		if (pInventoryComponent->CanAccessInventory())
		{
			//and you've thrown your grenade
			if (pInventoryComponent->GetGrenade()->HasGrenade() == false)
			{
				//just call equip to detonate it, don't switch weapons
				pInventoryComponent->GetGrenade()->NetMC_Equip();
			}
			else
			{
				//otherwise switch normally
				pActiveWeapon = pInventoryComponent->SwitchToGrenade();
			}
		}
	}
}


/* Inventory */
void ABasePlayer::DisableInventory()
{
	// If we are able to access the inventory we disable it
	if (pInventoryComponent->CanAccessInventory())
	{
		//Disable inventory and unset the active weapon
		pInventoryComponent->DisableInventory();
		pActiveWeapon = nullptr;
	}
	SetCanAccessInventory(false);
}

void ABasePlayer::EnableInventory()
{
	//If we can't access the inventory we enable it
	if (!pInventoryComponent->CanAccessInventory())
	{
		//Enable the inventory and get the last active weapon
		pInventoryComponent->EnableInventory();
		pActiveWeapon = pInventoryComponent->GetLastActiveWeapon();
	}
	SetCanAccessInventory(true);
}


void ABasePlayer::SetCanAccessInventory(bool bAccess)
{
	if(pInventoryComponent->CanAccessInventory() != bAccess)
		pInventoryComponent->SetCanAccessInventory(bAccess);
}


void ABasePlayer::InitializeInventoryForPlay()
{
	if (pInventoryComponent != nullptr)
	{
		pActiveWeapon = pInventoryComponent->SwitchToGun();
	}
}


#undef print