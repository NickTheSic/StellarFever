// Copyright Quarantine Games 2020


#include "../PlayerHUDWidget.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/StatusComponent.h"
#include "../../Player/BasePlayer.h"
#include "../../Player/SF_PlayerController.h"
#include "../../Weapons/GunBase.h"
#include "../../Weapons/GrenadeBase.h"
#include "../../Weapons/WeaponBase.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "../../BeenoFight/BeenoDimeBoss.h"
#include "Kismet/GameplayStatics.h"


void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupOwningPlayerAsBasePlayer();
	EnableBossbarForBossfight();
	UObject* obj = StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Game/Temporary/MAT_Circle"));
	if (obj)
	{
		pStaminaCircleMaterial = UMaterialInstanceDynamic::Create(Cast<UMaterial>(obj), this);
		StaminaCircle->SetBrushFromMaterial(pStaminaCircleMaterial);
	}
	else
		StaminaCircle->SetVisibility(ESlateVisibility::Hidden);
}


void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (pOwningPlayer == nullptr)
	{
		//We don't have an owning base player. Maybe we could find one
		UE_LOG(LogTemp, Warning, TEXT("We didn't have a base player in the HUD"));
		//SetupOwningPlayerAsBasePlayer();
		return;
	}

	UpdateHealthInfo();
	UpdateOxygenInfo();
	UpdateHitShieldInfo();
	UpdateClassSpecialInfo();
	UpdateGrenadeInfo();
	UpdateStaminaInfo();

	if (pBossPointer)
	{
		BossBar->SetPercent(pBossPointer->GetBossHealthPercentage());
	}
}

void UPlayerHUDWidget::SetupOwningPlayerAsBasePlayer()
{
	if (ASF_PlayerController* pc = GetOwningPlayer<ASF_PlayerController>())
	{
		if (ABasePlayer* player = Cast<ABasePlayer>(pc->GetCharacter()))
		{
			pOwningPlayer = player;

			//Set the player colour just in case it didn't get set on Begin Play
			SetPlayerTintColour(pOwningPlayer->GetPlayerInformation().ClassColour);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Couldn't cast the Player Controller character as a player"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Couldn't cast the owning player controller as a SF_PlayerController"));
}


void UPlayerHUDWidget::UpdateHealthInfo()
{
	if (pHealthText)
	{
		FString str = "";

		if (pOwningPlayer)
		{
			float healthPercent = pOwningPlayer->GetHealthComponent()->GetHealthPercentage();
			int iHealthPercent = healthPercent * 100.0f; //Round it off and set it between 1 and 100
			str.AppendInt(iHealthPercent);
		}

		str.Append("%");

		FText HealthText = FText::FromString(str);
		pHealthText->SetText(HealthText);
	}


	if (pHealthBar)
	{
		if (pOwningPlayer)
		{
			float healthPercent = pOwningPlayer->GetHealthComponent()->GetHealthPercentage();
			pHealthBar->SetPercent(healthPercent);
		}
	}

}


void UPlayerHUDWidget::UpdateOxygenInfo()
{
	if (pOxygenText)
	{
		FString str = "";


		if (pOwningPlayer)
		{
			float oxygenPercent = pOwningPlayer->GetOxygenComponent()->GetPercentage();
			int iOxygenPercent = oxygenPercent * 100.0f;
			str.AppendInt(iOxygenPercent);
		}


		str.Append("%");

		FText OxygenText = FText::FromString(str);
		pOxygenText->SetText(OxygenText);
	}


	if (pOxygenBar)
	{
		if (pOwningPlayer)
		{
			float oxygenPercent = pOwningPlayer->GetOxygenComponent()->GetPercentage();
			pOxygenBar->SetPercent(oxygenPercent);
		}
	}

}


void UPlayerHUDWidget::UpdateHitShieldInfo()
{
	if (pHitShieldText && pHitShieldBar)
	{
		FString str = "";

		if (pOwningPlayer)
		{

			float percentage = 
				pOwningPlayer->GetHealthComponent()->fShieldAmount / 
				pOwningPlayer->GetHealthComponent()->fDefaultShieldAmount;


			pHitShieldBar->SetPercent(percentage);

			int iPercent = percentage * 100.0f;

			str.AppendInt(iPercent);
		}


		str.Append("%");

		FText HitShieldText = FText::FromString(str);
		pHitShieldText->SetText(HitShieldText);
	}
}

void UPlayerHUDWidget::UpdateClassSpecialInfo()
{
	if (pClassAbilityBar)
	{
		if (pOwningPlayer)
		{
			if (pOwningPlayer->GetSpecialIsActive())
			{
				pClassAbilityBar->SetIsMarquee(true);
			}
			else
			{
				pClassAbilityBar->SetIsMarquee(false);
			}

			pClassAbilityBar->SetPercent(pOwningPlayer->GetClassSpecialPercentageCharged());
		}
	}
}

void UPlayerHUDWidget::UpdateGrenadeInfo()
{
	if(pGrenadeRechargeBar)
	{
		if (pOwningPlayer)
		{
			pGrenadeRechargeBar->SetPercent(pOwningPlayer->GetGrenadePercentage());
		}
	}
}

void UPlayerHUDWidget::UpdateStaminaInfo()
{
	if (pStaminaCircleMaterial)
	{
		if (pOwningPlayer)
		{
			float staminaPercent = pOwningPlayer->GetStaminaComponent()->GetPercentage();
			pStaminaCircleMaterial->SetScalarParameterValue("Angle", staminaPercent * 360.f);
			StaminaCircle->SetOpacity(1.f - staminaPercent);
		}
	}
}


void UPlayerHUDWidget::SetPlayerTintColour(FColor InColour)
{
	//Set the colour to the class specific colour
	
	//Images
	ACABarImage->SetColorAndOpacity(InColour);
	GrenadeImage->SetColorAndOpacity(InColour);
	HealthShieldIcons->SetColorAndOpacity(InColour);
	HelmetImage->SetColorAndOpacity(InColour);
	O2Icon->SetColorAndOpacity(InColour);
	StaminaCircleInner->SetColorAndOpacity(InColour);


	//Progress Bars
	pClassAbilityBar->SetFillColorAndOpacity(InColour);
	pGrenadeRechargeBar->SetFillColorAndOpacity(InColour);
	pHealthBar->SetFillColorAndOpacity(InColour);
	pHitShieldBar->SetFillColorAndOpacity(InColour);
	pOxygenBar->SetFillColorAndOpacity(InColour);
	if (pStaminaCircleMaterial)
	{
		pStaminaCircleMaterial->SetVectorParameterValue("Color", InColour);
	}

	//Text - ?
	//FSlateColor  textColor;


}






void UPlayerHUDWidget::EnableBossbarForBossfight()
{
	//GetWorld()->Find
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABeenoDimeBoss::StaticClass(), actors);
	if (actors.Num() > 0)
	{
		pBossPointer = static_cast<ABeenoDimeBoss*>(actors[0]);
		BossBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		BossIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}