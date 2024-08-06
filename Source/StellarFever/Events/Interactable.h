// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class STELLARFEVER_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//The mesh that the players interact with so to get ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* pMesh;

	//Display Text just to say AMMO REFILL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
		class UTextRenderComponent* pMainText;

	//Display text for the ammount of uses we have left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
		UTextRenderComponent* pUsesText;

public:
	/////ALL FUNCTIONS BELOW/////

};
