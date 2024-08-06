// Coded by Nick Lemmon

#include "../EngineerPlayer.h"
#include "../../AI/AIEnemy.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

AEngineerPlayer::AEngineerPlayer()
{
	playerInformation.ePlayerClass = EPlayerClass::Engineer;

	playerInformation.fActiveAbilityLength = 0.05f;
	playerInformation.fActiveAbilityCooldown = 60.0f;

	Tags.Add("Engineer");
}

bool AEngineerPlayer::UseSpecial()
{
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), GetActorLocation(), fTargetStunRadius, 12, FColor::Emerald, false, 10.0f);
#endif

	Server_FindAndStunEnemies();

	return true;
}


void AEngineerPlayer::Server_FindAndStunEnemies_Implementation()
{
	TArray<AActor*> enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", enemies);

	FVector playerLoc = GetActorLocation();
	FVector enemyLoc = FVector::ZeroVector;

	int iEnemiesHit = 0;

	for (AActor* enemy : enemies)
	{
		enemyLoc = enemy->GetActorLocation();

		float distance = FVector::Distance(playerLoc, enemyLoc);

		if (distance < fTargetStunRadius)
		{
			AAIEnemy* aiEnemy = Cast<AAIEnemy>(enemy);
			if (aiEnemy != nullptr)
			{
				aiEnemy->Stunned();
				++iEnemiesHit;
			}
		}
	}

	#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "Engineer hit " + FString::FromInt(iEnemiesHit) + "Enemies");
	#endif

	if (iEnemiesHit >= 3)
	{
		Multicast_SuccesfulStunCallout();
	}
}

void AEngineerPlayer::Multicast_SuccesfulStunCallout_Implementation()
{
	if (pPlayerSoundComponent != nullptr)
	{
		pPlayerSoundComponent->PlayKillstreak();

		#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "Engineer is playing a special sound for hitting 3 enemies");
		#endif
	}
}
