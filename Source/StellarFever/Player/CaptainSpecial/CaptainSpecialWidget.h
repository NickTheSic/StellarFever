// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CaptainSpecialWidget.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UCaptainSpecialWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UCaptainSpecialWidget(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

	UFUNCTION()
		void ExitMenu();

	void CreateButtons(int32 numButtons);

	void SetCaptainPlayer(class ACaptainPlayer* captain) { pCaptain = captain; }
	void SetCaptainScreen(class UCaptainScreen* screen)  { pScreen  = screen;  }

protected:

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCaptainTeleportButton> buttonTemplate;

	UPROPERTY(meta = (BindWidget))
		class UButton*    exitButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* titleText  = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* buttonList = nullptr;

private:

	class ACaptainPlayer* pCaptain = nullptr;
	class UCaptainScreen* pScreen = nullptr;

};
