// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../CaptainPlayer.h"
#include "Components/WidgetComponent.h"
#include "CaptainSpecialScreen.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UCaptainScreen : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UCaptainScreen();

	void ActivateSpecialMenu();
	void DeactivateSpecialMenu();

	virtual void BeginPlay() override;

	void SetCaptainToTeleport(int8 teleportInfo);

	void SetButtonCount(int32 numButtons);

	class ACaptainPlayer* GetOwningCaptain() { return pCaptainPlayer; }

protected:
	class ACaptainPlayer*        pCaptainPlayer = nullptr;
	class UCaptainSpecialWidget* pSpecialWidget = nullptr;

};
