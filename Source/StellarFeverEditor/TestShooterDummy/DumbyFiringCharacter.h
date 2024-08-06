// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StellarFever/Components/RaycastComponent.h"
#include "DumbyFiringCharacter.generated.h"

UCLASS()
class ADumbyFiringCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADumbyFiringCharacter();

	URaycastComponent* raycast = nullptr;

	float timer = 0.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount,FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) 
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		SayOof(); return 0.0f;
	}

	void SayOof()
	{
		UE_LOG(LogTemp, Error, TEXT("OOOOOOOOF"));
		UE_LOG(LogTemp, Error, TEXT("Reflecting Hit works"));
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
