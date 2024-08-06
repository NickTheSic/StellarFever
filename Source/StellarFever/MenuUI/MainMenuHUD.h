// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */


UCLASS()
class STELLARFEVER_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	/*UI Templates*/
	UPROPERTY(EditAnywhere, Category = "UI Templates")
		TSubclassOf<class UMainMenuSwitcher> MainMenuSwitcherTemplate;

	UPROPERTY()
		class UMainMenuSwitcher* pMainMenuObject = nullptr;

	
	UFUNCTION(Exec)
		void SetEnablePlay();

	void GoBack();


protected:
	virtual void BeginPlay() override;

};
