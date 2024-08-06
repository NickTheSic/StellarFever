// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Grenades/GrenadePrediction.h"
#include "GrenadeBase.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGrenadeBase : public AWeaponBase
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AGrenadeBase();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* pTrajMovement;

	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		class URadialForceComponent* pExplosionForce;

	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* pSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* pGrenadePredictionExample;
	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		class UMaterial* pPredictMaterial;
	//UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"), Replicated)
	//	TArray<class AGrenadePrediction*> GrenadePredictionArray;
	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		class UPredictionPath* PredictPath;
	FPredictProjectilePathParams predictParams;
	FPredictProjectilePathResult predictResult;
	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		uint8 ucPredictNum;

	//removed num grenade stats for
	bool bHasGrenade;
	bool bHasExploded;
	bool bIsAiming;

	UPROPERTY(EditAnywhere, Category = "Grenade Stats", meta = (AllowPrivateAccess = "true"))
		float fGrenadeRange;

	UPROPERTY(EditAnywhere, Category = "Grenade Stats")
		class USoundCue* pThrowSound;
	UPROPERTY(EditAnywhere, Category = "Grenade Stats")
		class USoundCue* pExplodeSound;

	UPROPERTY(Replicated)
	FTimerHandle RechargeTimer;
	UPROPERTY(EditAnywhere, Category = "Grenade Stats")
	float fRechargeTime;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShake> cameraShake;

public:
	UFUNCTION(Server, Reliable)
	void SpawnPredictionPath();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//When we right click
	virtual void AltAttackStart() override;
	//AltAttack code
	virtual void AltAttack() override;
	//When we release right click
	virtual void AltAttackStop() override;

	virtual void Explode();
	virtual void GiveGrenade();
	UFUNCTION(Client, Reliable)
	void Client_StartGrenadeTimer();

	virtual void NetMC_Equip() override;
	virtual void NetMC_UnEquip() override;

	virtual void ResetVars() override;

	virtual void Throw();
	
	UFUNCTION(Client, Reliable)
	void Client_ShowPath();
	UFUNCTION(Client, Reliable)
	void Client_ShowPathSingle(uint8 pathNum);
	UFUNCTION(Client, Reliable)
	void Client_HidePath();
	UFUNCTION(Client, Reliable)
	void Client_HidePathSingle(uint8 pathNum);

	virtual void ExplosionRaycast(TArray<class AActor*> enemies);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool HasGrenade() { return bHasGrenade; }

	FORCEINLINE bool IsAiming() { return bIsAiming; }

	FORCEINLINE float GetGrenadeTimerPercentage()
	{
		float Percentage = fRechargeTime;
	
		if (GetWorldTimerManager().IsTimerActive(RechargeTimer))
		{
			Percentage -= GetWorldTimerManager().GetTimerRemaining(RechargeTimer);
		}
	
		return Percentage / fRechargeTime;
	}

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayWorldCameraShake();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
