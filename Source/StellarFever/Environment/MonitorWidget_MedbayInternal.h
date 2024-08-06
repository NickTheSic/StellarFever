// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MonitorWidget.h"
#include "MonitorWidget_MedbayInternal.generated.h"

class UCanvasPanel;
class UTextBlock;
class UImage;

UCLASS()
class STELLARFEVER_API UMonitorWidget_MedbayInternal : public UMonitorWidget
{
	GENERATED_BODY()
protected:
		virtual void OnMonitorStateChanged_Internal(class AMonitorState* monitor, int32 prevID, int32 newID) override;

		//Whether to draw the player panel
		bool bShowPlayer1 = false, bShowPlayer2 = false, bShowPlayer3 = false, bShowPlayer4 = false;
		
		//Player panels
		UPROPERTY(meta = (BindWidget))
			UCanvasPanel* Player1Panel = nullptr, *Player2Panel = nullptr, *Player3Panel = nullptr, *Player4Panel = nullptr;

		//Player name
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Name = nullptr, * Player2Name = nullptr, * Player3Name = nullptr, * Player4Name = nullptr;

		//Player class
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Class = nullptr, * Player2Class = nullptr, * Player3Class = nullptr, * Player4Class = nullptr;

		//Player health
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Health = nullptr, * Player2Health = nullptr, * Player3Health = nullptr, * Player4Health = nullptr;

		//Player shield
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Shield = nullptr, * Player2Shield = nullptr, * Player3Shield = nullptr, * Player4Shield = nullptr;

		//Player oxygen
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Oxygen = nullptr, * Player2Oxygen = nullptr, * Player3Oxygen = nullptr, * Player4Oxygen = nullptr;

		//Player ammo
		UPROPERTY(meta = (BindWidget))
			UTextBlock* Player1Ammo = nullptr, * Player2Ammo = nullptr, * Player3Ammo = nullptr, * Player4Ammo = nullptr;

		//Player grenade
		UPROPERTY(meta = (BindWidget))
			UImage* Player1Grenade = nullptr, * Player2Grenade = nullptr, * Player3Grenade = nullptr, * Player4Grenade = nullptr;

		//Tick
		virtual void NativeTick(const FGeometry&, float d);

		//Data structure used only for UpdatePlayerInfo. This is to reduce repetitive code.
		struct InternalData { UCanvasPanel* panel; UTextBlock* name; UTextBlock* _class;  UTextBlock* health; UTextBlock* shield; UTextBlock* oxygen; UTextBlock* ammo; UImage* grenade; };
		
		//Run every tick to update monitors
		void UpdatePlayerInfo(class APlayerState* player, bool isVisible, InternalData data);

};
