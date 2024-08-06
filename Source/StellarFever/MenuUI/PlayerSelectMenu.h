// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "PlayerSelectMenuBase.h"
#include "../Player/PlayerInformation.h"
#include "PlayerSelectMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerSelectMenu : public UPlayerSelectMenuBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* BackButton = nullptr;

public:

	void NativeOnInitialized() override;  //I use as begin play

	UFUNCTION()  //A Function bound to the back button 
		void GoBack();

	virtual void SetPlayerClass(const EPlayerClass& playerClass);

	
	virtual void SelectPlayer() override;

};
