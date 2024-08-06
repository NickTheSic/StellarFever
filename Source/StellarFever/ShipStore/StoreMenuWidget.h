// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class STELLARFEVER_API UStoreMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UStoreMenuWidget(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void ExitMenu();

	void CreateButtons(int32 numButtons);

	void SetStoreManager(class AStoreManager* StoreManager) { pStoreManager = StoreManager; }
	void SetStoreMenu(class UStoreMenu* menu) { pMenu = menu; }

protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UHorizontalBox> horizontalTemplate;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UbuttonLayout> buttonTemplate;

	UPROPERTY(meta = (BindWidget))
		class UButton* exitButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* titleText = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* buttonList = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UImage* CoinImage = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrencyText = nullptr;

private:
	class AStoreManager* pStoreManager = nullptr;
	class UStoreMenu* pMenu = nullptr;
	class ButtonLayout* pLayout = nullptr;
};