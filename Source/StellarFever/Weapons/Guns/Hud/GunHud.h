// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GunHud.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UGunHud : public UUserWidget
{
	GENERATED_BODY()

protected:
	class AGunBase* pGun;

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UCanvasPanel* pCanvas = nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* pAmmoText = nullptr;

	void UpdateText(uint16 MagAmmo, uint16 ReserveAmmo);
	FORCEINLINE void SetGun(AGunBase* gun) { pGun = gun; }
};
