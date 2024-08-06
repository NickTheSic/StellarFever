// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StellarFeverTutorialGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AStellarFeverTutorialGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

    virtual void BeginPlay() override;

public:

    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
        void CheckActiveEvents();

    UFUNCTION()
        void Init();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEvents")
        TArray<class ABaseEvent*> pMainEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door1")
        class ADoorObject* pDoor1;

    bool bDoor1isOpen;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door2")
        ADoorObject* pDoor2;

    bool bDoor2isOpen;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door3")
        ADoorObject* pDoor3;

    bool bDoor3isOpen;

    FTimerHandle CheckActiveEventTimer;
    FTimerHandle InitTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveEvents")
        int iNumOfActiveEvents;


    class UHealthComponent* pPlayerHealthComp = nullptr;

};
