// Copyright Quarantine Games 2020


#include "MonitorWidget_HallwayInternal.h"
#include "Blueprint/WidgetTree.h"
#include "MonitorState.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameState.h"
#include "Monitor.h"
#include "../Events/EventsManager.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UMonitorWidget_HallwayInternal::OnMonitorStateChanged_Internal(AMonitorState* state, int32 prevID, int32 newID)
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

void UMonitorWidget_HallwayInternal::NativeTick(const FGeometry& geo, float deltatime)
{
	UpdateStates();
}

void UMonitorWidget_HallwayInternal::UpdateStates()
{

	auto UpdateColor = [](bool check, UBorder* border) -> void
	{
		if (border != nullptr)
			border->SetBrushColor(check ? FLinearColor(1.0f, 0.05385f, 0.0f, 0.2f) : FLinearColor(0.116421f, 1.0f, 0.0f, 0.2f));
	};

	if (GetMonitorState() != nullptr && GetMonitorState()->GetOwner() != nullptr)
	{
		const FEventsState& state = Cast<AMonitor>(GetMonitorState()->GetOwner())->GetEventsState();

		UpdateColor(state.greenhouse, Highlight_Greenhouse);
		UpdateColor(state.boarding, Highlight_Airlock);
		UpdateColor(state.bridge, Highlight_Bridge);
		UpdateColor(state.depressure, Highlight_Depressure);
		UpdateColor(state.enemyship, Highlight_Armory);
		UpdateColor(state.lengine, Highlight_LEngine);
		UpdateColor(state.rengine, Highlight_REngine);
		UpdateColor(state.lifesupport, Highlight_Medbay);
		UpdateColor(state.oxygen, Highlight_Oxygen);
		UpdateColor(state.power, Highlight_Engineering);
		UpdateColor(state.bathroom, Highlight_Bathroom);

		if (state.playershield == false)
		{
			if (Shield_Icon_ON)
				Shield_Icon_ON->SetVisibility(ESlateVisibility::Visible);
			if (Shield_Icon_OFF)
				Shield_Icon_OFF->SetVisibility(ESlateVisibility::Hidden);
			if (Shield_Text)
				Shield_Text->SetText(FText::FromString("Shields Online"));
		}
		else
		{

			if (Shield_Icon_ON)
				Shield_Icon_ON->SetVisibility(ESlateVisibility::Hidden);
			if (Shield_Icon_OFF)
				Shield_Icon_OFF->SetVisibility(ESlateVisibility::Visible);
			if (Shield_Text)
				Shield_Text->SetText(FText::FromString("Shields Offline"));
		}
	}
}