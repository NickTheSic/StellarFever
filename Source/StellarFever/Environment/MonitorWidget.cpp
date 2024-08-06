// Copyright Quarantine Games 2020


#include "MonitorWidget.h"
#include "MonitorState.h"

void UMonitorWidget::SetMonitorState(AMonitorState* state)
{
	//Unbind if we're bound already
	if (pMonitorState != nullptr)
		pMonitorState->onMonitorStateChanged.Remove(onMonitorStateChangedHandle);
		//pMonitorState->onMonitorStateChanged.RemoveDynamic(this, &UMonitorWidget::OnMonitorStateChanged);

	//Bind to new state
	pMonitorState = state;
	onMonitorStateChangedHandle = pMonitorState->onMonitorStateChanged.AddUFunction(this, "OnMonitorStateChanged");
	//pMonitorState->onMonitorStateChanged.AddDynamic(this, &UMonitorWidget::OnMonitorStateChanged);
}
AMonitorState* UMonitorWidget::GetMonitorState()
{
	return pMonitorState;
}

void UMonitorWidget::OnMonitorStateChanged_Internal(AMonitorState* state, int32 prevId, int32 newID)
{

}

void UMonitorWidget::OnMonitorStateChanged_Implementation(AMonitorState* state, int32 prevID, int32 newID)
{
	OnMonitorStateChanged_Internal(state, prevID, newID);
}