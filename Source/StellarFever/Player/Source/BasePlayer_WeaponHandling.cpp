// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "../Animation/PlayerActionMontageComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Weapons/WeaponBase.h"
#include "../../Weapons/GunBase.h"


#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif

void ABasePlayer::Server_Reload_Implementation()
{
	print("Player reloading");
	if (pActiveWeapon == nullptr) return;

	AGunBase* gun = Cast<AGunBase>(pActiveWeapon);
	if (gun)
	{
		gun->ReloadStart();
	}
}


void ABasePlayer::Fire()
{
	if (GetIsPlayerDowned() || GetIsPlayerDead())
		return;

	if (HasPickupBeenPickedUp())
	{
		HandlePickupSetDown();
		return;
	}
	else if (GetIsPlayerInMenu())
	{
		StartInteraction();
		return; //Return out of the function since we are in a menu and we don't want to actually fire - Mainly in the Captian
	}

	Server_Fire();
}

void ABasePlayer::Server_Fire_Implementation()
{
	if (pActiveWeapon != nullptr)
	{
		pActiveWeapon->AttackStart();
	}
}


void ABasePlayer::StopFire()
{
	if (GetIsPlayerInMenu())
	{
		StopInteraction();
		return;
	}

	Server_StopFire();
}

void ABasePlayer::Server_StopFire_Implementation()
{
	if (pActiveWeapon != nullptr)
	{
		//if we are using a grenade
		if (pActiveWeapon->GetWeaponType() == EWeaponType::Grenade)
		{
			AGrenadeBase* g = static_cast<AGrenadeBase*>(pActiveWeapon);
			//and are aiming that grenade
			if (g->IsAiming())
			{
				//throw it and switch to the gun
				pActiveWeapon->AttackStop();
				pActiveWeapon = GetInventoryComponent()->SwitchToGun();

				if (pPlayerAnimComponent != nullptr)
				{
					pPlayerAnimComponent->PlayWeaponSwitchAnimation();
				}
			}
		}
		//otherwise attackstop normally
		else
		{
			pActiveWeapon->AttackStop();
		}
	}
}


void ABasePlayer::AltFire()
{
	if (GetIsPlayerDowned() || GetIsPlayerDead())
		return;

	if (HasPickupBeenPickedUp())
	{
		HandlePickupSetDown();
		return;
	}
	else if (GetIsPlayerInMenu())
	{
		StartInteraction();
		return;
	}

	Server_AltFire();
}

void ABasePlayer::Server_AltFire_Implementation()
{
	if (pActiveWeapon != nullptr)
	{
		pActiveWeapon->AltAttackStart();
	}
}


void ABasePlayer::StopAltFire()
{
	if (GetIsPlayerInMenu())
	{
		StopInteraction();
		return;
	}

	Server_StopAltFire();
}

void ABasePlayer::Server_StopAltFire_Implementation()
{
	if (pActiveWeapon != nullptr)
	{
		pActiveWeapon->AltAttackStop();
	}
}


void ABasePlayer::Throw()
{
	if (GetIsPlayerInMenu() || GetIsPlayerDowned() || GetIsPlayerDead())
	{
		return;
	}

	print("Player Throwing Grenade");

	if (HasPickupBeenPickedUp())
	{
		HandlePickupSetDown();
		return;
	}

	Server_Throw();
}

void ABasePlayer::Server_Throw_Implementation()
{

	if (playerInformation.ePlayerClass == EPlayerClass::Gunner)
	{
		if (pInventoryComponent->CanAccessInventory())
		{
			//and you've thrown your grenade
			if (pInventoryComponent->GetGrenade()->HasGrenade() == false)
			{
				//just call equip to detonate it, don't switch weapons
				pInventoryComponent->GetGrenade()->NetMC_Equip();

				return;
			}
		}
	}

	if (pInventoryComponent != nullptr)
	{
		pInventoryComponent->ThrowGrenade();

		if (pPlayerAnimComponent != nullptr)
		{
			pPlayerAnimComponent->PlayGrenadeThrow();
		}

	}
}


void ABasePlayer::ReplenishAmmo_Implementation(uint16 ammo)
{
	if (pInventoryComponent)
	{
		pInventoryComponent->ReplenishAmmo(ammo);
	}
}

#undef print