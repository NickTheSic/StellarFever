// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
		
public:


	//Player status displays
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pHealthText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* pHealthBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pOxygenText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* pOxygenBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pHitShieldText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* pHitShieldBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* pGrenadeRechargeBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* pClassAbilityBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ACABarImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* GrenadeImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* HelmetImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* HealthShieldIcons = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* O2Icon = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* StaminaCircle = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* StaminaCircleInner = nullptr;

	UPROPERTY()
		class UMaterialInstanceDynamic* pStaminaCircleMaterial = nullptr;
	

	//Custom Text setting functions
	void UpdateHealthInfo();
	void UpdateOxygenInfo();
	void UpdateHitShieldInfo();
	void UpdateClassSpecialInfo();
	void UpdateGrenadeInfo();
	void UpdateStaminaInfo();


	//Matt stuff for boss fight
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
		class UProgressBar* BossBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
		class UImage* BossIcon = nullptr;
	UFUNCTION()
		void EnableBossbarForBossfight();
	UPROPERTY()
		class ABeenoDimeBoss* pBossPointer = nullptr;

protected:

	void SetupOwningPlayerAsBasePlayer();
	class ABasePlayer* pOwningPlayer = nullptr;

public:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetOwningBasePlayer(class ABasePlayer* player) { pOwningPlayer = player; }
	void SetPlayerTintColour(FColor InColour);


};
