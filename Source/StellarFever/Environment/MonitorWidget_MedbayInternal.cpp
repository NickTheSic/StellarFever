// Copyright Quarantine Games 2020


#include "MonitorWidget_MedbayInternal.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "GameFramework/GameState.h"
#include "../Player/SF_PlayerState.h"
#include "../Player/BasePlayer.h"
#include "../Components/HealthComponent.h"
#include "../Components/StatusComponent.h"
#include "../Components/InventoryComponent.h"

void UMonitorWidget_MedbayInternal::OnMonitorStateChanged_Internal(AMonitorState* state, int32 prevID, int32 newID)
{
	//Disable old page
	FString page = "Page_";
	page.AppendInt(prevID);

	UWidget* widget = WidgetTree->FindWidget(FName(page));
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Hidden);

	//Enable new page
	page = "Page_";
	page.AppendInt(newID);

	widget = WidgetTree->FindWidget(FName(page));
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Visible);
}

void UMonitorWidget_MedbayInternal::UpdatePlayerInfo(APlayerState* player, bool isVisible, InternalData data)
{
	//If the panel is null, exit. We can't do anything
	if (data.panel == nullptr)
		return;

	//Make panel visible/invisible if a player is in the slot or not
	if (isVisible && !data.panel->IsVisible())
		data.panel->SetVisibility(ESlateVisibility::Visible);
	else if (!isVisible && data.panel->IsVisible())
		data.panel->SetVisibility(ESlateVisibility::Hidden);

	//Get player's name, and truncate it to 9 characters and ellipses if the name is longer than 12 characters.
	//This is so it fits on screen
	FString name = player->GetPlayerName();
	data.name->SetText(FText::FromString(name.Len() > 12 ? (name.LeftChop(name.Len() - 9) + "...") : name));

	//Get player pawn
	ABasePlayer* ply = Cast<ABasePlayer>(player->GetPawn());

	if (ply)
	{
		//Get player class text
		switch (ply->GetPlayerClass())
		{
			case EPlayerClass::Captain:
				data._class->SetText(FText::FromString("Captain"));
				break;
			case EPlayerClass::Engineer:
				data._class->SetText(FText::FromString("Engineer"));
				break;
			case EPlayerClass::Gunner:
				data._class->SetText(FText::FromString("Gunner"));
				break;
			case EPlayerClass::Medic:
				data._class->SetText(FText::FromString("Medic"));
				break;
			default:
				data._class->SetText(FText::FromString("Error"));
				break;
		}
		//Set health, shield and oxygen on panel
		data.health->SetText(FText::FromString(FString::FromInt(ply->GetHealthComponent()->Health)));
		data.shield->SetText(FText::FromString(FString::FromInt(ply->GetHealthComponent()->fShieldAmount)));
		data.oxygen->SetText(FText::FromString(FString::FromInt(ply->GetOxygenComponent()->GetValue()) + "%"));

		//Get gun and ammo for gun
		AGunBase* gun = ply->GetInventoryComponent()->GetGun();
		if (gun != nullptr)
			data.ammo->SetText(FText::FromString(FString::FromInt(gun->GetMagAmmo()) + "/" + FString::FromInt(gun->GetMaxReserveAmmo())));

		//If we have a grenade, show the grenade icon, or else hide it.
		if (ply->GetInventoryComponent()->GetGrenade() != nullptr)
			data.grenade->SetVisibility(ply->GetInventoryComponent()->GetGrenade()->HasGrenade() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UMonitorWidget_MedbayInternal::NativeTick(const FGeometry& g, float deltaTime)
{
	Super::NativeTick(g, deltaTime);

	//Get the player array for later
	TArray<APlayerState*>& playerArray = GetWorld()->GetGameState()->PlayerArray;

	//Set player bools
	bShowPlayer1 = playerArray.Num() > 0;
	bShowPlayer2 = playerArray.Num() > 1;
	bShowPlayer3 = playerArray.Num() > 2;
	bShowPlayer4 = playerArray.Num() > 3;

	GEngine->AddOnScreenDebugMessage(5, -1.f, FColor::Purple, "Number of players: " + FString::FromInt(playerArray.Num()));


	//If we don't do a check before the method, it crashes due to out of bounds check on array

	if (bShowPlayer1)
		UpdatePlayerInfo(playerArray[0], bShowPlayer1, { Player1Panel, Player1Name, Player1Class, Player1Health, Player1Shield, Player1Oxygen, Player1Ammo, Player1Grenade });

	if (bShowPlayer2)
		UpdatePlayerInfo(playerArray[1], bShowPlayer2, { Player2Panel, Player2Name, Player2Class, Player2Health, Player2Shield, Player2Oxygen, Player2Ammo, Player2Grenade });
	
	if (bShowPlayer3)
		UpdatePlayerInfo(playerArray[2], bShowPlayer3, { Player3Panel, Player3Name, Player3Class, Player3Health, Player3Shield, Player3Oxygen, Player3Ammo, Player3Grenade });
	
	if (bShowPlayer4)
		UpdatePlayerInfo(playerArray[3], bShowPlayer4, { Player4Panel, Player4Name, Player4Class, Player4Health, Player4Shield, Player4Oxygen, Player4Ammo, Player4Grenade });

}