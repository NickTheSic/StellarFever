// Copyright Quarantine Games 2020


#include "CurrencyDrop.h"
#include "../Miscellaneous/ShipState.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Components/BoxComponent.h"
#include "../Player/BasePlayer.h"
#include "../StellarFeverGameState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#define print(msg, val) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg + FString::FromInt(val));
void ACurrencyDrop::Tick(float DeltaTime)
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
					bgrabed = true;
					AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
					AShipState* shipstate = gameState->GetShipState();
					shipstate->AddMoney(iCurrency);
					print("ship currency value is: ", shipstate->GetMoney())
					//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue,"ship currency value is: " + FString::FromInt(shipstate->GetMoney()));
						DespawnSelf();
				}
			}
		}

	}

}
