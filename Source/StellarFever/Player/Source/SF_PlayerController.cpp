// Coded by Nick Lemmon


#include "../SF_PlayerController.h"
#include "../BasePlayer.h"
#include "../SFHUD.h"
#include "../../Miscellaneous/LobbyGamemode.h"
#include "../../StellarFeverGameInstance.h"
#include "../../GameModes/StellarFeverSinglePlayerGameMode.h"
#include "../../GameModes/StellarFeverMultiplayerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "../SF_PlayerState.h"
#include "../../MessageSystem/MessageQueue.h"


ASF_PlayerController::ASF_PlayerController()
{
	bInPauseMenu = false;
}

void ASF_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(InPawn != nullptr)
		pPlayer = Cast<ABasePlayer>(InPawn);
	if(pPlayer)
		pPlayer->SetupWidgetInteractionUserID();

}


void ASF_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;

	FInputModeGameOnly game;
	SetInputMode(game);


	FCanUnpause unpausable;
	unpausable.BindLambda([]() { return true; });
	SetPause(false, unpausable);
}

void ASF_PlayerController::UpdateLookSpeed(float speed)
{
	InputPitchScale = -(InputYawScale = InputRollScale = (speed / 2.f));
}

void ASF_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	//Bind Jump functionality
	InputComponent->BindAction( "Jump", IE_Pressed , this, &ASF_PlayerController::Jump);
	InputComponent->BindAction( "Jump", IE_Released , this, &ASF_PlayerController::StopJumping);

	//Bind Interact functionality
	InputComponent->BindAction("Interact", IE_Pressed, this, &ASF_PlayerController::StartInteraction);
	InputComponent->BindAction("Interact", IE_Released , this, &ASF_PlayerController::StopInteraction);

	//Bind the Special
	InputComponent->BindAction("Special", IE_Pressed, this, &ASF_PlayerController::ClassSpecial);

	//Weapon functionality
	InputComponent->BindAction("Fire", IE_Pressed, this, &ASF_PlayerController::Fire);
	InputComponent->BindAction("Fire", IE_Released, this, &ASF_PlayerController::StopFire);
	InputComponent->BindAction("AltFire", IE_Pressed, this, &ASF_PlayerController::AltFire);
	InputComponent->BindAction("AltFire", IE_Released, this, &ASF_PlayerController::StopAltFire);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ASF_PlayerController::Reload);
	InputComponent->BindAction("Throw", IE_Pressed, this, &ASF_PlayerController::Throw);

	//Bind movement functionality
	InputComponent->BindAction("Run", IE_Pressed, this, &ASF_PlayerController::StartRunning);
	InputComponent->BindAction("Run", IE_Released, this, &ASF_PlayerController::StopRunning);

	InputComponent->BindAxis("MoveForward", this, &ASF_PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight",   this, &ASF_PlayerController::MoveRight);

	UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>();
	if (sfInstance)
		UpdateLookSpeed(sfInstance->fLookSensitivity);

	InputComponent->BindAxis("Turn", this, &ASF_PlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &ASF_PlayerController::AddPitchInput);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ASF_PlayerController::PlayerBeginCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &ASF_PlayerController::PlayerEndCrouch);

	//Bind Inventory functionality
	InputComponent->BindAction("MouseWheelUp",   IE_Pressed, this, &ASF_PlayerController::MouseWheelUp);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &ASF_PlayerController::MouseWheelDown);
	InputComponent->BindAction("NumKeyOne", IE_Pressed, this, &ASF_PlayerController::NumKeyOne);
	InputComponent->BindAction("NumKeyTwo", IE_Pressed, this, &ASF_PlayerController::NumKeyTwo);
	InputComponent->BindAction("NumKeyThree", IE_Pressed, this, &ASF_PlayerController::NumKeyThree);

	InputComponent->BindAction("Quit", IE_Pressed, this, &ASF_PlayerController::PauseMenu);

	//Voice chat
	InputComponent->BindAction("VoiceToggle", IE_Pressed, this, &ASF_PlayerController::StartVoice);
	InputComponent->BindAction("VoiceToggle", IE_Released, this, &ASF_PlayerController::EndVoice);

}

void ASF_PlayerController::StartVoice()
{
	ConsoleCommand("ToggleSpeaking 1");
}

void ASF_PlayerController::EndVoice()
{
	ConsoleCommand("ToggleSpeaking 0");
}

void ASF_PlayerController::PauseMenu()
{
	if (bInGameOver) return;

	if (bInPauseMenu == false)
	{
		OpenPauseMenu();
	}
	else if(bInPauseMenu == true)
	{
		ClosePauseMenu();
	}
}

void ASF_PlayerController::OpenPauseMenu()
{
	if (ASFHUD* hud = GetHUD<ASFHUD>())
	{
		hud->OpenPauseMenu();

		bInPauseMenu = true;
		bShowMouseCursor = true;

		FInputModeGameAndUI input;
		SetInputMode(input);

		SetPause(true);
	}
}

void ASF_PlayerController::ClosePauseMenu()
{
	if (ASFHUD* hud = GetHUD<ASFHUD>())
	{
		hud->ClosePauseMenu();

		bInPauseMenu = false;
		bShowMouseCursor = false;

		FInputModeGameOnly input;
		SetInputMode(input);

		SetPause(false);
	}
}

void ASF_PlayerController::AddYawInput(float val)
{
	if (GetIsInAMenu()) return;
	if (UStellarFeverGameInstance* inst = GetGameInstance<UStellarFeverGameInstance>())
	{
		//Check if we have inverted X axis
		if ((int)inst->eInvertAxis & (int)EInvertControllerAxis::Invert_Axis_X)
			val = -val;
	}

	Super::AddYawInput(val);
}

void ASF_PlayerController::AddPitchInput(float val)
{
	if (GetIsInAMenu()) return;
	if (UStellarFeverGameInstance* inst = GetGameInstance<UStellarFeverGameInstance>())
	{
		//Check if we have inverted Y axis
		if ((int)inst->eInvertAxis & (int)EInvertControllerAxis::Invert_Axis_Y)
			val = -val;
	}
	Super::AddPitchInput(val);
}

void ASF_PlayerController::Jump()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->Jump();
	}
}

void ASF_PlayerController::StopJumping()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StopJumping();
	}
}

void ASF_PlayerController::StartInteraction()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StartInteraction();
	}
}

void ASF_PlayerController::StopInteraction()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StopInteraction();
	}
}

void ASF_PlayerController::StartRunning()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StartRunning();
	}
}

void ASF_PlayerController::StopRunning()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StopRunning();
	}
}

void ASF_PlayerController::MoveForward(float val)
{
	if (val == 0.0f || GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->MoveForward(val);
	}
}

void ASF_PlayerController::MoveRight(float val)
{
	if (val == 0.0f || GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->MoveRight(val);
	}
}

void ASF_PlayerController::PlayerBeginCrouch()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->PlayerBeginCrouch();
	}
}

void ASF_PlayerController::PlayerEndCrouch()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->PlayerEndCrouch();
	}
}

void ASF_PlayerController::Reload()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->Server_Reload();
	}
}

void ASF_PlayerController::Fire()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->Fire();
	}
}

void ASF_PlayerController::StopFire()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StopFire();
	}
}

void ASF_PlayerController::AltFire()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->AltFire();
	}
}

void ASF_PlayerController::StopAltFire()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->StopAltFire();
	}
}

void ASF_PlayerController::ClassSpecial()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->ClassSpecial();
	}
}

void ASF_PlayerController::Throw()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->Throw();
	}
}

void ASF_PlayerController::MouseWheelUp()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->MouseWheelUp();
	}
}

void ASF_PlayerController::MouseWheelDown()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->MouseWheelDown();
	}
}

void ASF_PlayerController::NumKeyOne()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->SwitchToSlotOne();
	}
}

void ASF_PlayerController::NumKeyTwo()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->SwitchToSlotTwo();
	}
}

void ASF_PlayerController::NumKeyThree()
{
	if (GetIsInAMenu()) return;

	if (pPlayer)
	{
		pPlayer->SwitchToSlotThree();
	}
}


void ASF_PlayerController::TriggerEndGame(const EEndGameReason& endGameReason)
{
	bInGameOver = true;

	Client_EnableEndGameMenu(endGameReason);
}

void ASF_PlayerController::Client_EnableEndGameMenu_Implementation(const EEndGameReason& endGameReason)
{
	FInputModeGameAndUI endGameInput;
	SetInputMode(endGameInput);

	bShowMouseCursor = true;

	if (ASFHUD* hud = GetHUD<ASFHUD>())
	{
		hud->OpenEndGameMenu();
		hud->SetEndGameInformation(endGameReason);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get HUD which means the HUD couldn't open EndGame Menu"));
	}
}

void ASF_PlayerController::PlayerReturnToMenu()
{
	Server_PlayerReturnToMenu();
}

void ASF_PlayerController::Server_PlayerReturnToMenu_Implementation()
{
	if (AStellarFeverMultiplayerGameMode* sfMulti = GetWorld()->GetAuthGameMode<AStellarFeverMultiplayerGameMode>())
	{
		//Log out
	}
}

void ASF_PlayerController::PlayerPlayAgain()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "Player wanted to play again - Local Controller");
	Server_PlayerPlayAgain();
}

void ASF_PlayerController::Server_PlayerPlayAgain_Implementation()
{
	if (AStellarFeverSinglePlayerGameMode* sfSingle = GetWorld()->GetAuthGameMode<AStellarFeverSinglePlayerGameMode>())
	{
		if (ASFHUD* hud = GetHUD<ASFHUD>())
		{
			hud->OpenPlayerSelectMenu();
		}
		else
		{
			UGameplayStatics::OpenLevel(this, FName("PlayerSelect"));
		}

		return;
	}

	if (AStellarFeverMultiplayerGameMode* sfMulti = GetWorld()->GetAuthGameMode<AStellarFeverMultiplayerGameMode>())
	{
		sfMulti->PlayerMadeEndGameChoice(true);
	}
}


void ASF_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASF_PlayerController, pPlayer);
	DOREPLIFETIME(ASF_PlayerController, bInGameOver);
}

void ASF_PlayerController::SendMessage(FString message)
{
	SendMessageToServer(message, PlayerState->GetPlayerName());
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "Sent: " + message);
}

void ASF_PlayerController::SendMessageToServer_Implementation(const FString& message, const FString& sender)
{
	Cast<ASF_PlayerState>(PlayerState)->pMessageQueue->Push(message, sender);
}