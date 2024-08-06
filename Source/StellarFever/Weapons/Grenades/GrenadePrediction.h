// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadePrediction.generated.h"

UCLASS()
class STELLARFEVER_API AGrenadePrediction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenadePrediction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	class UStaticMeshComponent* pMesh; //Weapon Mesh

public:	

	UFUNCTION(Server, Reliable)
		void SetMeshComp(UStaticMesh* mesh);
	UFUNCTION(Server, Reliable)
		void SetMeshMat(UMaterial* mat);
	
	UFUNCTION(Client, Reliable)
		void HidePath();

	UFUNCTION(Client, Reliable)
		void ShowPath();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
