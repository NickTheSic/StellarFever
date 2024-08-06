// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineerBullet.generated.h"

UCLASS()
class STELLARFEVER_API AEngineerBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEngineerBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UStaticMeshComponent* pMesh; //Weapon Mesh

	UPROPERTY()
	class UCapsuleComponent* pBulletCapsule;

	UPROPERTY()
	class USphereComponent* pExplosionRadius;

	UPROPERTY(Replicated)
	class UAudioComponent* pAudioComponent;

	uint8 ucShotOrder;

	UPROPERTY()
	class AAIEnemy* pAIWeHitRef;

	//lifetime for bullet
	float fTimeBeforeExploding;
	FTimerHandle EndOfLifeExplosionTimer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShake> cameraShake;

	bool bAttachedToShield;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void Server_Fire(const FHitResult& hit, const FQuat& dir, float BurnDamage, float BurnDuration, float HitsPerSecond);

	void EndOfLifeExplode() { Server_Explode(1.0f); }

	UFUNCTION(Server, Reliable)
	void Server_Explode(float ExplosionDamage);

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayExplodeSound();

	FORCEINLINE void SetShotOrder(uint8 shotorder) { ucShotOrder = shotorder; }
	FORCEINLINE uint8 GetShotOrder() { return ucShotOrder; }

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayWorldCameraShake();

	void SetCameraShake(TSubclassOf<class UCameraShake> shake) { cameraShake = shake; }

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
