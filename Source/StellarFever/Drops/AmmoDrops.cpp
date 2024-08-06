// Copyright Quarantine Games 2020


#include "AmmoDrops.h"
#include "Components/BoxComponent.h"
#include "../Player/BasePlayer.h"
#include "..\Weapons/GunBase.h"
#include "..\Components/InventoryComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

void AAmmoDrops::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (pItemCollider != nullptr && bgrabed == false)
		{
			TArray<AActor*> CollectedActors;
			pItemCollider->GetOverlappingActors(CollectedActors);

			for (auto& actor : CollectedActors)
			{
				if (ABasePlayer* player = Cast<ABasePlayer>(actor))
				{
					//player->ReplenishAmmo(amountAmmo); 
					AGunBase* weapon = Cast<AGunBase>(player->GetInventoryComponent()->GetGun());
					if (weapon)
					{
						bgrabed = true;
						float newAmmo = weapon->GetMaxReserveAmmo() / 4;
						weapon->AmmoRefill(newAmmo);
						DespawnSelf();
					}
				}
			}
		}

	}

}

