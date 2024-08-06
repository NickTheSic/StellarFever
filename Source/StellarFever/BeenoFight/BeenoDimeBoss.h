// Copyright Quarantine Games 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeenoDimeBoss.generated.h"

UCLASS()
class STELLARFEVER_API ABeenoDimeBoss : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeenoDimeBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
		class USkeletalMeshComponent* pMesh = nullptr;
	UPROPERTY(EditInstanceOnly, Category = "Boss Stats")
		float fMaxBossHealth = 250.f;
	UPROPERTY()
		float fBossHealth = 250.f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE float GetBossHealthPercentage() const { return fBossHealth / fMaxBossHealth; }

	void DealDamage(AActor* Instigator, float Damage);

};
