// Copyright Quarantine Games 2020


#include "SFButton.h"

USFButton::USFButton()
{
	OnHovered.AddDynamic(this, &USFButton::OnHoveredFunction);
}

void USFButton::OnHoveredFunction()
{
	SetUserFocus(GetOwningPlayer());
}
