// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"


//A CPP to hold the functions that use the flag

void ABasePlayer::AddToPlayerStatusFlag(EPlayerStatus addStatus)
{
	Server_AddToPlayerStatusFlag(addStatus);
	UpdatePlayerSpeed();
}

void ABasePlayer::Server_AddToPlayerStatusFlag_Implementation(EPlayerStatus addStatus)
{
	//If the bit that we are trying to add isn't 1 we can add it
	if ((_PlayerStatusFlag & (uint32)addStatus) == 0)
	{
		_PlayerStatusFlag = FMath::Clamp<uint32>(_PlayerStatusFlag + (uint32)addStatus, 0, (uint32)EPlayerStatus::MAXVALUE);

#if WITH_EDITOR
		UE_LOG(LogTemp, Log, TEXT("We have added %u to the player status"), addStatus);
#endif
	}

#if WITH_EDITOR
	else
	{
		UE_LOG(LogTemp, Log, TEXT("We are trying to add %u while it has already been added"), addStatus);
	}
#endif

}

void ABasePlayer::RemoveFromPlayerStatusFlag(EPlayerStatus removeStatus)
{
	Server_RemoveFromPlayerStatusFlag(removeStatus);
	UpdatePlayerSpeed();
}

void ABasePlayer::Server_RemoveFromPlayerStatusFlag_Implementation(EPlayerStatus removeStatus)
{ 
	if ((_PlayerStatusFlag & (uint32)removeStatus) == (uint32)removeStatus)
	{
		_PlayerStatusFlag = FMath::Clamp<uint32>(_PlayerStatusFlag - (uint32)removeStatus, 0, (uint32)EPlayerStatus::MAXVALUE);
	
#if WITH_EDITOR
		UE_LOG(LogTemp, Log, TEXT("We have removed %u to the player status"), removeStatus);
#endif
	}

#if WITH_EDITOR
	else
	{
		UE_LOG(LogTemp, Log, TEXT("We are trying to remove %u while it wasn't previously added"), removeStatus);
	}
#endif

}

void ABasePlayer::ResetPlayerStatusFlag()
{
	Server_ResetPlayerStatusFlag();
}

void ABasePlayer::Server_ResetPlayerStatusFlag_Implementation()
{
	//Resets the player to a Normal Status
	_PlayerStatusFlag = 0;

	//Since this is one of the things that the player needs to be able to do right off the bat
	AddToPlayerStatusFlag(EPlayerStatus::CanUseSpecial);
}



void ABasePlayer::UpdatePlayerSpeed()
{
	Server_UpdatePlayerSpeed();
}

//Going off the assumption that the _PlayerStatusFlagUpdates
void ABasePlayer::Server_UpdatePlayerSpeed_Implementation()
{
	//Two constant speeds that won't change no matter what 
	if (GetIsPlayerDowned())
	{
		SetPlayerNewSpeed(playerInformation.fDownedMovementSpeed);
		return;
	}
	else if (GetIsPlayerDead() || GetIsPlayerInMenu())
	{
		SetPlayerNewSpeed(0);
		return;
	}

	if (GetIsPlayerCrouched())
	{
		SetPlayerNewSpeed(playerInformation.fCrouchSpeed);
		return;
	}

	if (GetIsEnraged())
	{
		SetPlayerNewSpeed(playerInformation.fRunSpeed * 1.25f);
		return;
	}

	if (GetIsPlayerRunning())
	{
		if ( ( GetIsFiring() && playerInformation.ePlayerClass == EPlayerClass::Gunner) )
		{
			SetPlayerNewSpeed(playerInformation.fWalkSpeed);
		}
		else
		{
			SetPlayerNewSpeed(playerInformation.fRunSpeed);
		}
	}
	else //The player isn't trying to run
	{
		if ( ( GetIsFiring() && playerInformation.ePlayerClass == EPlayerClass::Gunner) )
		{
			SetPlayerNewSpeed(playerInformation.fCarrySpeed);
		}
		else
		{
			SetPlayerNewSpeed(playerInformation.fWalkSpeed);
		}
	}
}
