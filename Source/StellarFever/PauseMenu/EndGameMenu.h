// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../MenuUI/MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "../Miscellaneous/EndGameEnum.h"
#include "EndGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UEndGameMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* PlayAgainButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ReturnToMenuButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* EndGameText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ReasonText = nullptr;


	void NativeOnInitialized() override;

	UFUNCTION()
	void OnClickPlayButton();
	UFUNCTION()
	void OnClickReturnToMenu();

	void SetReasonForEndingGame(const EEndGameReason& Reason);

protected:
	void SetReasonText(const EEndGameReason& Reason);

public:

	UPROPERTY(EditAnywhere)
		FString GameOverTitleText = "Game Over";

	UPROPERTY(EditAnywhere)
		FString GameWonTitleText = "Congradulation! A Winner is you!";


	/*These are each of the end game texts*/
	UPROPERTY(EditAnywhere)
		FString WinGameText = FString("You and your crew were able to deliver all the vaccines!  Now get back to work, we don't pay you to take breaks.");

	UPROPERTY(EditAnywhere)
		FString PlayersDeadText = FString("Your crewmates and you have died.  A dead employee is not an ideal employee.  You are all fired");

	UPROPERTY(EditAnywhere)
		FString VaccinesLostText = FString("You ran out of vaccines? BEFORE they were delivered.  Now we have to buy some back from those pirates, and it is coming out of your pay");

	UPROPERTY(EditAnywhere)
		FString DefaultGameOverText = FString("SPS won't be happy with this");
};
