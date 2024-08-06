// Copyright Quarantine Games 2020


#include "EmergencyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"

// Sets default values
AEmergencyLight::AEmergencyLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//The mesh that holds the light
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = pMesh;

	//The spotlight that will spin :)
	pSpotLight = CreateDefaultSubobject<USpotLightComponent>("Light");
	pSpotLight->SetLightColor(FLinearColor::Red);
	pSpotLight->SetRelativeRotation(FRotator(0.0f, 60.0f, 0.0f));
	pSpotLight->SetupAttachment(RootComponent);

	bisActive = false;

}

// Called when the game starts or when spawned
void AEmergencyLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEmergencyLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bisActive == true)
	{
		pSpotLight->SetRelativeRotation(FRotator(0.0f, 0.0f, 360.0f * DeltaTime));
	}

}

void AEmergencyLight::Activate()
{
	//Sets the bool to true, used in tick
	bisActive = true;

	//Allows the light to be visible
	pSpotLight->SetVisibility(true);
}

void AEmergencyLight::Deactivate()
{
	//Should turn off the spinning
	bisActive = false;

	//Hides the light
	pSpotLight->SetVisibility(false);
}

