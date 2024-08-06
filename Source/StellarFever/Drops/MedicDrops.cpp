// Copyright Quarantine Games 2020


#include "MedicDrops.h"
#include "Components/BoxComponent.h"
#include "../Player/BasePlayer.h"
#include "../Components/HealthComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

void AMedicDrops::Tick(float DeltaTime)
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
					if (player->GetHealthComponent()->Health != player->GetHealthComponent()->DefaultHealth)
					{
						bgrabed = true;
						PickupSound();
						float newHealth = player->GetHealthComponent()->DefaultHealth;
						newHealth /= 3.f;
						player->RestoreHealth(newHealth);

						DespawnSelf();
						pAudioComp->SetSound(pEffect);
						pAudioComp->Play();
					}
				}
			}
		}
	}
}