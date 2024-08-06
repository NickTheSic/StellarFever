// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FragGrenadeAI.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class USphereComponent;
class USoundCue;
class UParticleSystem;
class UParticleSystemComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UMaterialInterface;

UCLASS()
class STELLARFEVER_API AFragGrenadeAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFragGrenadeAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCapsuleComponent* pCapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* pMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UProjectileMovementComponent* pProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* pSphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* pGreenRadius;

	UFUNCTION()
	void Explode();

	FTimerHandle TimerHandle_Explode;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
		float fExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
		float fExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Exploder")
		float fExplodeTime;


	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		USoundCue* pExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		UParticleSystem* pExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Effects")
		UParticleSystemComponent* pParticleSystem;

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void MulticastSelfDestruct();

	//EXPLODER BOT SMOKE GRENADE STUFF
	UPROPERTY(EditAnywhere, Category = "Particles")
		UNiagaraSystem* pExplosionNiagara;

	UNiagaraComponent* pNiagaraEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
