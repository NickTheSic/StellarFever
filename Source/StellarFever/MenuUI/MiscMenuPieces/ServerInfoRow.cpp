// Copyright Quarantine Games 2020


#include "ServerInfoRow.h"
#include "../JoinMenu.h"
#include "Components/Button.h"

void UServerInfoRow::Setup(UJoinMenu* parent, uint32 index)
{
	pJoinMenu = parent;
	uiIndex = index;
	RowButton->OnClicked.AddDynamic(this, &UServerInfoRow::OnClickRowButton);
}

void UServerInfoRow::OnClickRowButton()
{
	pJoinMenu->SelectIndex(uiIndex);
	pJoinMenu->JoinButton->SetFocus();
}
