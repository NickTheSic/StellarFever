// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmergencyLight.generated.h"

UCLASS()
class STELLARFEVER_API AEmergencyLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEmergencyLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//The glass mesh that holds the light
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* pMesh;

	//The rotating light
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EmergencyLight")
		class USpotLightComponent* pSpotLight;

	//Just a basic bool used in tick to spin the light or not
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "isActive")
		bool bisActive;

public:
	/////ALL FUNCTIONS BELOW/////

	//Activates the light when called
	UFUNCTION()
		void Activate();

	//Deactivates the light when called
	UFUNCTION()
		void Deactivate();
};
