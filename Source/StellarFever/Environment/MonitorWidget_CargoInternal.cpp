// Copyright Quarantine Games 2020


#include "MonitorWidget_CargoInternal.h"
#include "Blueprint/WidgetTree.h"
#include "../StellarFeverGameState.h"
#include "../Miscellaneous/ShipState.h"
#include "Components/TextBlock.h"

void UMonitorWidget_CargoInternal::OnMonitorStateChanged_Internal(AMonitorState* state, int32 prevID, int32 newID)
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

void UMonitorWidget_CargoInternal::NativeConstruct()
{
	Super::NativeConstruct();

	//Update every second, as the information isn't crucial
	GetWorld()->GetTimerManager().SetTimer(timer, this, &UMonitorWidget_CargoInternal::UpdateInformation, 1.f, true);
}
void UMonitorWidget_CargoInternal::UpdateInformation()
{
	AStellarFeverGameState* state = GetWorld()->GetGameState<AStellarFeverGameState>();
	//Make sure game state exists
	if (state != nullptr)
	{
		AShipState* shipState = state->GetShipState();
		//Make sure ship state exists
		if (shipState != nullptr)
		{
			//Update text

			if (VaccineCount != nullptr)
				VaccineCount->SetText(FText::FromString("Vaccines: " + FString::FromInt(shipState->GetVaccines())));

			if (LifeCount != nullptr)
				LifeCount->SetText(FText::FromString("Team lives: " + FString::FromInt(shipState->GetTeamLives())));

			if (FuelCount != nullptr)
				FuelCount->SetText(FText::FromString("Fuel remaining: " + FString::SanitizeFloat(shipState->GetFuelAmount()) + "L"));

			if (GloobelCount != nullptr)
				GloobelCount->SetText(FText::FromString("Gloobels: " + FString::FromInt(shipState->GetMoney())));
		}
	}
}