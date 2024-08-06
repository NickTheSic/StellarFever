
#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/InventoryComponent.h"
#include "StellarFever/Player/SF_PlayerController.h"

#include "StellarFever/Weapons/GunBase.h"
#include "StellarFever/Weapons/MeleeBase.h"
#include "StellarFever/Weapons/GrenadeBase.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"

BEGIN_DEFINE_SPEC(InventoryHandlingTest, "StellarFever.InventoryHandlingTest",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	ASF_PlayerController* pc;
	ABasePlayer* player;
	UWorld* world;
	UInventoryComponent* inventory;

END_DEFINE_SPEC(InventoryHandlingTest)


void InventoryHandlingTest::Define()
{
	Describe("A Spec to test the Character can get his weapons", [this]()
	{
			BeforeEach([this]()
				{
					//Open a test map
					AutomationOpenMap("/Game/NickTest/FunctionalTests/TestLevels/InventoryWeaponsTest");
					//Get World
					world = GEngine->GetWorldContextFromPIEInstance(0)->World();
					TestNotNull("Check if we have world", world);

					pc = Cast<ASF_PlayerController>(world->GetFirstPlayerController());
					TestNotNull("Check we have a player controller", pc);

					player = Cast<ABasePlayer>(pc->GetPawn());
					TestNotNull("Check that we have our player", player);

					inventory = player->GetInventoryComponent();
					TestNotNull("Check that we have our inventory", inventory);

			});

		It("Test that the player Gets the proper Gun when we press NumKeyOne", [this]
		{
				pc->NumKeyOne(); //This is the gun slot
				TestTrue( "Check that the player has gotten the gun", Cast<AGunBase>(player->GetActiveWeapon()) != nullptr);	
		});

		It("Test that the player Gets the proper Gun when we press NumKeyOne and the owner is the player", [this]
			{
				pc->NumKeyOne(); //This is the gun slot
				TestTrue("Check that the gun owner is the player", player->GetActiveWeapon()->GetOwner() == player);
			});

		It("Test that the player Gets the proper melee when we press NumKeyTwo", [this]
			{
				pc->NumKeyTwo(); //This is the melee slot
				TestTrue("Check that the player has a melee weapon", Cast<AMeleeBase>(player->GetActiveWeapon()) != nullptr);
			});

		It("Test that the player Gets the Grenade when we press NumKeyThree", [this]
			{
				pc->NumKeyThree(); //This is the grenade slot
				TestTrue("Check that the player has a grenade", Cast<AGrenadeBase>(player->GetActiveWeapon()) != nullptr);
			});


		AfterEach([this]
		{
				player->ResetPlayerStatusFlag(); //In case we change it at somepoint
				pc->ConsoleCommand(TEXT("Exit"), true); 
		});

	});
}

#endif