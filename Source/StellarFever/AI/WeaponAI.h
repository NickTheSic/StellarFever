// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponAI.generated.h"

class UStaticMeshComponent;
class UDamageType;
class USoundBase;
class UParticleSystem;

UCLASS()
class STELLARFEVER_API AWeaponAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponAI();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float fItemLoudness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* pMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* pMuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* pTracerEffect;

	bool bFireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float InitialDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float fBaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fTimeBetweenShots;

		float fCurrentAmmo;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		//float fMaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		float fClipSizeAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		float fWeaponRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float fBulletSpread;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* pFireSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* pReloadSound;

	float fFireTimer;

	//FTimerHandle TimerHandle_TimeBetweenShots;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void FireSingle();
	virtual void FireSingleAI();

	//virtual void FireAuto(float val);
	virtual void FireAutoAI();

	void Enable();
	void Disable();

};
