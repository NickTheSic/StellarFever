// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmokeGrenadeAI.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class USoundCue;
class UParticleSystem;
class UParticleSystemComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class STELLARFEVER_API ASmokeGrenadeAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmokeGrenadeAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* pCapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* pMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UProjectileMovementComponent* pProjectileMovement;


	UFUNCTION()
		void Explode();

	FTimerHandle TimerHandle_Explode;
 
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundCue* pExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UParticleSystem* pExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Particles")
		UNiagaraSystem* pExplosionNiagara;

		UNiagaraComponent* pNiagaraEffect;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastSelfDestruct();


	UFUNCTION()
		void DestroyGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Effects")
		UParticleSystemComponent* pParticleSystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
