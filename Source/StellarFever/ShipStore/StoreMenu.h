// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "StoreMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UStoreMenu : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UStoreMenu();

	void ActivateStoreMenu();
	void DeactivateStoreMenu();

	virtual void BeginPlay() override;

	void SetToPurchase(int8 PurchaseInfo);

	void SetButtonCount(int32 numButtons);

protected:

	UPROPERTY()
	class AStoreManager*    pStoreManager = nullptr;

	UPROPERTY()
	class UStoreMenuWidget* pMenuWidget = nullptr;
};
