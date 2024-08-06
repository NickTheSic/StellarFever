// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GunBase.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AGunBase : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGunBase();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		class UWidgetComponent* pGunHud; //Widget to display weapon stats

	//reloading variables
	bool bIsReloading;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fReloadTime;
	FTimerHandle ReloadTimer;

	//spread variables
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fMinBulletSpread;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fMaxBulletSpread;
	UPROPERTY(Replicated)
	float fCurrentBulletSpread;

	//shooting variables
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fWeaponRange;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fStartFalloff;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		float fEndFalloff;

	//Ammo variables
	UPROPERTY(Replicated)
		uint16 usMagAmmo;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		uint16 usMaxMagAmmo;
	UPROPERTY(Replicated)
		uint16 usReserveAmmo;
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		uint16 usMaxReserveAmmo; 
	UPROPERTY(EditAnywhere, Category = "Gun Stats", meta = (AllowPrivateAccess = "true"))
		uint16 usNumMags;

	//For AI detecting us
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float fItemLoudness;

	//Tracer target name
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gun Stats")
		FName TracerTargetName;
	//Tracer effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun Stats", Replicated)
		class UParticleSystem* pTracerEffect;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true")/*, Replicated*/)
		class UNiagaraSystem* pTracerImpact; //For playing weapon particle effects

	//Gun sounds
	UPROPERTY(EditAnywhere, Category = "Gun Stats")
		class USoundCue* pFireSound;
	UPROPERTY(EditAnywhere, Category = "Gun Stats")
		class USoundCue* pReloadSound;
	UPROPERTY(EditAnywhere, Category = "Gun Stats")
		class USoundCue* pNoAmmoSound;

	UPROPERTY(EditAnywhere)
		TSubclassOf <class UCameraShake> pCamShakeSubclass;

	UPROPERTY(EditAnywhere)
	class UHapticFeedbackEffect_Curve* haptic;

	UPROPERTY(EditAnywhere)
	class UForceFeedbackEffect* controllerForce;



public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ReloadStart();
	virtual void Reload();
	virtual void ReloadStop();

	virtual void AmmoRefill(uint16 ammo);

	virtual void NetMC_UnEquip() override;

	UFUNCTION(BlueprintCallable)
		virtual int32 GetMagAmmo() { return (int32)usMagAmmo; }
	UFUNCTION(BlueprintCallable)
		virtual int32 GetMaxMagAmmo() { return (int32)usMaxMagAmmo; }
	UFUNCTION(BlueprintCallable)
		virtual int32 GetReserveAmmo() { return (int32)usReserveAmmo; }
	UFUNCTION(BlueprintCallable)
		virtual int32 GetMaxReserveAmmo() { return (int32)usMaxReserveAmmo; }

	virtual bool DealDamage(const FHitResult* hit, float newDamage);
	virtual void MakeNoiseAI(const FHitResult* hit);

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool IsReloading() const { return bIsReloading; }

	UFUNCTION(NetMulticast, Unreliable)
	virtual void NetMC_FireExtras(FVector TracerBeginPoint, FVector TracerEndPoint, class UParticleSystem* pEffect, class UNiagaraSystem* pImpact = nullptr);

	UFUNCTION(Client, Unreliable)
	virtual void Client_FireExtras(FVector TracerBeginPoint, FVector TracerEndPoint, class UParticleSystem* pEffect, class UNiagaraSystem* pImpact = nullptr);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
		void PlayCameraShake();

	UFUNCTION(NetMulticast, Reliable)
		void NetMC_UpdateText_Firing();
	UFUNCTION(NetMulticast, Reliable)
		void NetMC_UpdateText(uint16 magAmmo, uint16 reserveAmmo);
};
