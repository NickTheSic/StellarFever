// Copyright Quarantine Games 2020


#include "MonitorWidget_Medbay.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

void UMonitorWidget_Medbay::OnMonitorStateChanged_Internal(AMonitorState* state, int32 prevID, int32 newID)
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