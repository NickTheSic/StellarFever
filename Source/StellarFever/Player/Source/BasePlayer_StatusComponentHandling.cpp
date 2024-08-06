// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Components/StatusComponent.h"
#include "../../StellarFeverGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif


/*Oxygen*/
void ABasePlayer::DrainOxygen(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pOxygenComponent != nullptr)
	{
		pOxygenComponent->SubtractFromValue(val);

		if (pPlayerSoundComponent != nullptr && pPlayerSoundComponent->IsPlaying() == false)
		{

			if (pOxygenComponent->GetPercentage() < 0.92f && 
				pOxygenComponent->GetPercentage() > 0.9f)
			{
				pPlayerSoundComponent->PlayLosingOxygenEvent();
			}
		}
	}
}

void ABasePlayer::ReplenishOxygen(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pOxygenComponent != nullptr)
	{
		pOxygenComponent->AddToValue(val);
	}
}

void ABasePlayer::DrainMaxOxygen(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pOxygenComponent != nullptr)
	{
		pOxygenComponent->SubtractFromMaxValue(val);
	}
}
void ABasePlayer::ResetMaxOxygen(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pOxygenComponent != nullptr)
	{
		pOxygenComponent->SetMaxValue(playerInformation.fMaxLungSize);
	}
}


/*Stamina*/
void ABasePlayer::DrainStamina(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pStaminaComponent != nullptr)
	{
		pStaminaComponent->SubtractFromValue(val);
	}
}

void ABasePlayer::ReplenishStamina(float val)
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	if (pStaminaComponent != nullptr)
	{
		pStaminaComponent->AddToValue(val);
	}
}


/*Health*/

void ABasePlayer::DrainHitShield(float drainAmount)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pHealthComponent)
	{
		pHealthComponent->DrainShield(drainAmount);
	}
}

void ABasePlayer::RestoreHealth(float healAmount)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (pHealthComponent != nullptr)
	{
		pHealthComponent->Heal(healAmount);
	}
}

void ABasePlayer::DrainHealth(float drainAmount)
{
	if (GetLocalRole() < ROLE_Authority) return;

	if (!ensure(pHealthComponent != nullptr)) return;

	if (pHealthComponent->Health > 0)
	{

		if (pPlayerSoundComponent != nullptr && pPlayerSoundComponent->IsPlaying() == false)
		{
			if (pHealthComponent->GetHealthPercentage() < 0.91f &&
				pHealthComponent->GetHealthPercentage() > 0.88f)
			{
				pPlayerSoundComponent->PlayLosingHealthEvent();
			}
		}

		pHealthComponent->Health -= drainAmount;
		if (pHealthComponent->Health <= 0 && !GetIsPlayerDead() && !GetIsPlayerDowned())
		{
			pHealthComponent->Health = 0;
			OnCharacterNoHealth();
		}
	}
}


void ABasePlayer::OnHealthChanged(UHealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && (!GetIsPlayerDowned() || !GetIsPlayerDead()))
	{
		UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
		OnCharacterNoHealth();
	}

	//Only play the sound when the player has no shield and has taken damage
	if (HealthDelta > 0.0f && pHealthComponent->fShieldAmount <= 0.1f)
	{
		if (pPlayerSoundComponent != nullptr && pPlayerSoundComponent->IsPlaying() == false)
		{
			pPlayerSoundComponent->PlayBeingHurt();
		}
	}
}


void ABasePlayer::OnCharacterNoHealth()
{
	//We only want to trigger these effects if we aren't already downed
	if (GetIsPlayerDowned() || GetIsPlayerDead())
		return;


	//Disable inventory so we can't use it while we are downed
	if (pInventoryComponent != nullptr)
	{
		pInventoryComponent->DisableInventory();
	}

	//Add the Downed flag for status checking purposes
	AddToPlayerStatusFlag(EPlayerStatus::Downed);

	//Start the revive timer
	//GetWorldTimerManager().SetTimer(TimerHandle_ReviveOrDie, this, &ABasePlayer::OnCharacterDeath, playerInformation.fKnockedoutTimer, false);

	//Change movement on the player
	UpdatePlayerSpeed();
	GetMovementComponent()->StopMovementImmediately();
	
	Client_OnCharacterNoHealth();
	Server_OnCharacterNoHealth();
}

void  ABasePlayer::Client_OnCharacterNoHealth_Implementation()
{
	//Make the player look like they have fallen down
	if (pCameraComponent != nullptr)
	{
		pCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
}

void ABasePlayer::Server_OnCharacterNoHealth_Implementation()
{
	if (HasPickupBeenPickedUp())
	{
		HandlePickupSetDown();
	}


	//If we are the last player and we go into revive mode we just die instead
	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		//Decrememnt the player amount since there is once less player that is able to revive
		--pGameMode->PlayerAmount;

		if (pGameMode->PlayerAmount < 1)
		{
			OnCharacterDeath();
		}
	}

	Multicast_OnCharacterNoHealth();
}

void ABasePlayer::Multicast_OnCharacterNoHealth_Implementation()
{
	if (pPlayerSoundComponent != nullptr)
	{
		pPlayerSoundComponent->PlayGoingDown();
	}
}

void ABasePlayer::OnCharacterDeath()
{
	Server_OnCharacterDeath();

	AddToPlayerStatusFlag(EPlayerStatus::Dead);

	IgnorePlayerInput(true);
}

void ABasePlayer::Server_OnCharacterDeath_Implementation()
{
	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		pGameMode->WhenPlayersAllDie();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABasePlayer::StartRevive()
{
	Server_StartRevive();
	//GetWorldTimerManager().PauseTimer(TimerHandle_ReviveOrDie);
}

void ABasePlayer::Server_StartRevive_Implementation()
{
	//Stuff that revives the player here
	UE_LOG(LogTemp, Log, TEXT("Revive function is incomplete, instantly reviving player"));

	pHealthComponent->ServerSimpleRevive();
	OnCharacterRevived();

	//GetWorldTimerManager().PauseTimer(TimerHandle_ReviveOrDie);
	//pHealthComponent->StartRevive();
}

void ABasePlayer::StopRevive()
{
	//GetWorldTimerManager().UnPauseTimer(TimerHandle_ReviveOrDie);
	//pHealthComponent->StopRevive()
}


void ABasePlayer::OnCharacterRevived()
{
	Client_OnCharacterRevived();
	Server_OnCharacterRevived();
}

void ABasePlayer::Client_OnCharacterRevived_Implementation()
{
	//Make the player look like they have fallen down
	if (pCameraComponent != nullptr)
	{
		pCameraComponent->SetRelativeLocation(StandingCamera);
	}
}

void ABasePlayer::Server_OnCharacterRevived_Implementation()
{
	RemoveFromPlayerStatusFlag(EPlayerStatus::Downed);
	RemoveFromPlayerStatusFlag(EPlayerStatus::Dead);


	//Re enable the inventory now that we are revived again
	if (pInventoryComponent != nullptr)
	{
		pInventoryComponent->EnableInventory();
	}


	//Now that we are healed, tell the gamemode that we are are available again
	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		//Increment the Player amount since this player has been revived
		++pGameMode->PlayerAmount;
	}

	Multicast_OnCharacterRevived();
}

void ABasePlayer::Multicast_OnCharacterRevived_Implementation()
{
	//Play a thank you sound effect
	if (pPlayerSoundComponent != nullptr)
	{
		pPlayerSoundComponent->PlayThankRevive();
	}
}


#undef print