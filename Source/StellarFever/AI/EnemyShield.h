// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyShield.generated.h"

UCLASS()
class STELLARFEVER_API AEnemyShield : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyShield();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* pShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* pShieldCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fShieldHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fDefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fInitialHealth;

	void HurtShield(float DamageAmount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deactivate", Replicated)
	bool bDeactive;

	//bool bIsDead = false;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Enable();
	void Disable();
};
