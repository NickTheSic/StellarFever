

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

BEGIN_DEFINE_SPEC(PlayerSpeedSpec, "StellarFever.PlayerSpeedSpec",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	ASF_PlayerController* pc;
	ABasePlayer* player;
	UWorld* world;
	FPlayerInformation playerinfo;

END_DEFINE_SPEC(PlayerSpeedSpec)


void PlayerSpeedSpec::Define()
{
	Describe("A Spec to test the Character movement speeds", [this]()
	{
			BeforeEach([this]()
				{
					//Open a test map
					AutomationOpenMap("/Game/NickTest/FunctionalTests/TestLevels/FTEST_PlayerSpeeds");
					//Get World
					world = GEngine->GetWorldContextFromPIEInstance(0)->World();
					TestNotNull("Check if we have world", world);

					pc = Cast<ASF_PlayerController>(world->GetFirstPlayerController());
					TestNotNull("Check we have a player controller", pc);

					player = Cast<ABasePlayer>(pc->GetPawn());
					TestNotNull("Check that we have our player", player);

					player->ResetPlayerStatusFlag();
					playerinfo = player->GetPlayerInformation();
			});

		It("Test that the player runs at full run speed", [this]
		{
				pc->StartRunning();

				TestTrue( "Check that the player is running at full run speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fRunSpeed);
				
		});

		It("Test that the player runs at walk speed when holding item", [this]
			{
				pc->StartInteraction();
				player->AddToPlayerStatusFlag(EPlayerStatus::HoldingPickup);
				pc->StartRunning();

				TestTrue("Check that the player is running at walk speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fWalkSpeed);

			});

		It("Test that the player runs at walk speed when we start", [this]
			{
				TestTrue("Check that the player is walk at walk speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fWalkSpeed);
			});

		It("Test that the player runs at walk speed when we release the key", [this]
			{
				pc->StartRunning();
				pc->StopRunning();
				TestTrue("Check that the player is walk at walk speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fWalkSpeed);
			});

		It("Test that the player walks at carry speed when carrying an item", [this]
			{
				player->AddToPlayerStatusFlag(EPlayerStatus::HoldingPickup);
				pc->StartInteraction();
				TestTrue("Check that the player is walk at walk speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fCarrySpeed);
			});

		It("Test that the player walks at downed speed when downed an item", [this]
			{
				player->DrainHealth(500.0f); //A Really big health number
				TestTrue("Check that the player is walk at walk speed", Cast<UCharacterMovementComponent>(player->GetMovementComponent())->MaxWalkSpeed == playerinfo.fDownedMovementSpeed);
			});

		AfterEach([this]
		{
				pc->StopRunning();
				pc->StopInteraction();
				player->ResetPlayerStatusFlag();

				pc->ConsoleCommand(TEXT("Exit"), true); 
		});

	});
}

#endif