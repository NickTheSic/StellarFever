// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropsBase.generated.h"

UCLASS()
class STELLARFEVER_API ADropsBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADropsBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Mesh", Replicated)
		class USkeletalMeshComponent* pItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item Collider")
		class UBoxComponent* pItemCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iamountHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iamountAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iEngAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iCapAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iMedicAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iGunnerAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iCurrency = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Effects", Replicated)
		class UParticleSystemComponent* pParticleSystem;

	FTimerHandle FireTimer;
	FTimerHandle destroyTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float DespawnTimer = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimInstance* pDropAnimation;

	UPROPERTY(EditAnywhere)
		class UAudioComponent* pAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Effect")
		class USoundBase* pEffect;

	bool bgrabed = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Unreliable)
		void DespawnSelf();

	UFUNCTION()
		void DestroySelf();

	UFUNCTION(NetMulticast, reliable)
		void PickupSound();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
