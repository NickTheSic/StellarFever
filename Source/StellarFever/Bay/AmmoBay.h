// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoBay.generated.h"

UCLASS()
class STELLARFEVER_API AAmmoBay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoBay();

	UPROPERTY(EditAnywhere)
		bool bTutorial;

	UPROPERTY(EditAnywhere, Category = "medic mesh")
		UStaticMeshComponent* pAmmoMesh;

	//amount of ammo refilled per click
	UPROPERTY(EditAnywhere, Category = "amount can refill")
		uint16 usbulletamount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "refill", Replicated)
		int RefillsAllowed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "refill")
		int MaxedRefills = 30;

	FTimerHandle RefillDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "refill")
		float AmmoRefillRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int softRefillCap = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UWidgetComponent* ReserveComp;


	void refillAmmo(class AGunBase* bullets);
	void StartRefillResource();
	void StopRefillResource();
	void IncreaseRefillAmount();
	void PartialRefill(int amount);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	virtual void BeginPlay() override;
};
