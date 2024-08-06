// Copyright Quarantine Games 2020


#include "MonitorState.h"

void AMonitorState::SetPage(int32 page)
{
	//By doing it this way, we can guarantee that "this" is updated before the event is broadcast
	int32 old = iPageID;
	iPageID = page;
	onMonitorStateChanged.Broadcast(this, old, page);
}

int32 AMonitorState::GetPage()
{
	return iPageID;
}