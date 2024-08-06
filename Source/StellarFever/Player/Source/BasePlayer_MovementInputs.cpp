// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "../../Components/StatusComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif

void ABasePlayer::StartRunning()
{
	if (GetIsPlayerRunning() || GetIsPlayerDowned() || GetIsPlayerInMenu() || GetIsEnraged())
		return;

	//null check the stamina component and check if it is empty
	if (pStaminaComponent && pStaminaComponent->IsEmpty())
	{
		StopRunning();
		return;
	}
	AddToPlayerStatusFlag(EPlayerStatus::Running);
}

void ABasePlayer::StopRunning()
{
	//If we are not running when we stop
	if (!GetIsPlayerRunning() || GetIsPlayerDowned() || GetIsEnraged())
		return;

	RemoveFromPlayerStatusFlag(EPlayerStatus::Running);
}

void ABasePlayer::Jump()
{
	if (GetIsPlayerInMenu())
	{
		//The gamepad jump is set to the button that I feel resembles interation with a menu
		StartInteraction();
		return;
	}

	if(GetIsInteracting() || GetIsPlayerDowned() || GetIsPlayerDead())
		return; //Let's not do anything while interacting

	if (!pPlayerSoundComponent->IsPlaying() && !GetMovementComponent()->IsFalling())
	{
		pPlayerSoundComponent->PlayJumpingGrunt();
	}

	Super::Jump();
}


void ABasePlayer::MoveForward(float val)
{
	if (GetIsPlayerInMenu())
	{	
		if (pWidgetInteractionComponent->IsOverInteractableWidget())
		{
			pWidgetInteractionComponent->ScrollWheel(val * 0.1); //Simulate scrolling on UI at a very small rate
		}
		return;//The player is in a menu, let's not move
	}

	AddMovementInput(GetActorForwardVector(), val);
	HandleMakeMovementNoise();
}

void ABasePlayer::MoveRight(float val)
{
	if (GetIsPlayerInMenu())
	{	//Currently only the captain has a menu
		return;//The player is in a menu, let's not move
	}

	AddMovementInput(GetActorRightVector(), val);
	HandleMakeMovementNoise();
}


void ABasePlayer::PlayerBeginCrouch()
{
	AddToPlayerStatusFlag(EPlayerStatus::Crouched);
	
	Crouch();
}

void ABasePlayer::PlayerEndCrouch()
{
	RemoveFromPlayerStatusFlag(EPlayerStatus::Crouched);

	UnCrouch();
}


//This gets called from a function that was called on the server
//That way the server changes the MaxWalkSpeed
//Unfortunately that variable isn't replicated so we need to add a function call
void ABasePlayer::SetPlayerNewSpeed(float newSpeed)
{
	//We are able to run so we change the MaxWalkSpeed 
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = newSpeed;
	}

	//Make sure that we do the same thing on the client
	ClientPlayerNewSpeed(newSpeed);
}

void ABasePlayer::ClientPlayerNewSpeed_Implementation(float newSpeed)
{
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = newSpeed;
	}
}


void ABasePlayer::HandleMakeMovementNoise()
{
	if (GetIsPlayerRunning() && !GetIsPlayerCrouched()) /// OR || GetIsHoldingPickup() ?
	{
		MakeNoise(playerInformation.fHeavyMovementNoise, this, GetActorLocation());
	}

	else if (GetIsPlayerCrouched())
	{
		MakeNoise(playerInformation.fCrouchedMovementNoise, this, GetActorLocation());
	}

	else
	{
		MakeNoise(playerInformation.fMovementNoise, this, GetActorLocation());
	}
}
