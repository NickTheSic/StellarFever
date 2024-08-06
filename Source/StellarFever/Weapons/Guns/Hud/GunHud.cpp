// Copyright Quarantine Games 2020


#include "GunHud.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "../../GunBase.h"
#include "Styling/SlateColor.h"

void UGunHud::NativeConstruct()
{
    Super::NativeConstruct();

    //SetIsReplicatedByDefault(true);

    if (pAmmoText)
    {
        if (pGun)
        {
            pAmmoText->SetText(FText::FromString(FString::FromInt(pGun->GetMagAmmo()) + " / " + FString::FromInt(pGun->GetReserveAmmo())));
        }
        else
        {
            pAmmoText->SetText(FText::FromString("0/0"));
        }

        pAmmoText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    }
}

void UGunHud::UpdateText(uint16 MagAmmo, uint16 ReserveAmmo)
{
    if (pAmmoText)
    {
        pAmmoText->SetText(FText::FromString(FString::FromInt(MagAmmo) + " / " + FString::FromInt(ReserveAmmo)));
    }
}