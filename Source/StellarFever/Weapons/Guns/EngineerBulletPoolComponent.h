// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EngineerBulletPoolComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UEngineerBulletPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEngineerBulletPoolComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		TArray<class AEngineerBullet*> Bullets;

	uint8 ucActiveBullets;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
		void Server_Init(uint8 PoolSize, class AEngineerGun* engineer);
	UFUNCTION(Server, Reliable)
		void Server_Fire(const FHitResult& hit, const FQuat& dir, float BurnDamage, float BurnDuration, float HitsPerSecond, float ExplosionDamage);
	UFUNCTION(Server, Reliable)
		void Server_ExplodeBullets(float ExplosionDamage);

	TArray<FVector> GetBulletLocations();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
