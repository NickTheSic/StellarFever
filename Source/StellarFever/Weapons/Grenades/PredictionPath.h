// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PredictionPath.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UPredictionPath : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPredictionPath();

protected:

	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		TArray<class AGrenadePrediction*> GrenadePredictionArray;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(Server, Reliable)
	void SpawnPath(uint8 PredictNum, UStaticMesh* pGrenadePredictionExample, UMaterial* pPredictMaterial, class AGrenadeBase* grenade);

	UFUNCTION(Client, Reliable)
		void Client_ShowPath();
	UFUNCTION(Client, Reliable)
		void Client_ShowPathSingle(uint8 pathNum);
	UFUNCTION(Client, Reliable)
		void Client_HidePath();
	UFUNCTION(Client, Reliable)
		void Client_HidePathSingle(uint8 pathNum);

	TArray<class AGrenadePrediction*> GetPredictionArray();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
