// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MedicBay.generated.h"

UCLASS()
class STELLARFEVER_API AMedicBay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMedicBay();

	UPROPERTY(EditAnywhere, Category = "medic mesh")
		UStaticMeshComponent* pMedicMesh;


	class ABasePlayer* playerHealing;

	FTimerHandle HealDelay;
	FTimerHandle HealRefill;

	UPROPERTY(EditAnywhere)
		bool bTutorial;

	//amount player heals at
	UPROPERTY(EditAnywhere)
		float HealAmount = 10.0f;

	//total heal reserves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "available resource", Replicated)
	int HealAvailable = 600;

	//rate player heals at
	UPROPERTY(EditAnywhere)
		float HealingSpeed= 0.5f;

	//rate player heals at
	UPROPERTY(EditAnywhere)
		float HealingRate = 10.0f;

	//rate heal amount available refills by
	UPROPERTY(EditAnywhere)
		float RefillRate = 10.0f;

	//speed heal reserves refill at
	UPROPERTY(EditAnywhere)
		float RefillSpeed = 3.0f;

	//heal available at 100%
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "available resource")
		int iDefaultHealthAvailable = 600;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UWidgetComponent* ReserveComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int softRefillCap = 0;


public:	
	void StartHeal(class ABasePlayer* player);
	void Heal();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	void StopHealing();
	void RefillingAvailableHealth();
	void StartRefillingMedBAy();
	void PartialRefill(int amount);
	void PartialRefillPercentage(float amount); //write in 0.05 to increase at 5% for example
	void ActivateAutoRefill();

protected:
	virtual void BeginPlay() override;
};
