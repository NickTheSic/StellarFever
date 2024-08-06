// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Weapon,
	Gun,
	Melee,
	Grenade,
}; typedef EWeaponType WeaponType;

//Current animation state
UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	Neutral,		//idle stance
	Charging,		//for when we're charging a weapon attack I.e. medic smg's alt fire
	Attacking,		//for when we attack using a weapon
	Equipping,		//for when we switch to a weapon
	CoolingDown,	//for abilities on cooldown I.e. captain's poison knife
	Reloading		//for guns only
};

UCLASS()
class STELLARFEVER_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))

	//	UAnimSequence* pAnimation; //For shooting/attacking/throwing animations

	UPROPERTY(Replicated, EditAnywhere)
		class USkeletalMeshComponent* pTPRenderedMesh; //Weapon Mesh
	UPROPERTY(Replicated, EditAnywhere)
		class USkeletalMeshComponent* pFPRenderedMesh; //Weapon Mesh
	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		class USkeletalMesh* pMesh; //Weapon Mesh
		//Will need to be USkeletalMeshComponent if artists want to do weapon animation i.e. reloading
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
		class UPhysicsAsset* pPhysAsset;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"), Replicated)	//replicated in case of weapon attachments
		float fDamage; //Float for Damage 

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"), Replicated)	//replicated in case of weapon attachments
		float fAttackRate; //Float for AttackTimer time

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		class UNiagaraSystem* pParticleEmitter; //For playing weapon particle effects

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"), Replicated)
		class UAudioComponent* pAudioComponent; //For various weapon sound cues
	
	FTimerHandle AttackTimer; //Base timer for weapons when attacking

	UPROPERTY(EditAnywhere, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UDamageType> DamageType; //Damage type for AI functionality

	bool bIsAttacking; //Bool for attacking or not
	bool bIsAltAttacking; //Bool for altattacking or not

	UPROPERTY(Replicated) //players should know if a weapon is active or inactive
		bool bIsActive; //Bool for if this weapon is active or not

	WeaponType Type; //What type of weapon

	EAnimationState AnimState; //What animation state we're in

	UPROPERTY(BlueprintReadOnly, Replicated)
	class ABasePlayer* pOwnerRef;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", DisplayName="Weapon Animations List")
		TMap<FString, class UAnimationAsset*> FAnimationsList;

	UPROPERTY(EditAnywhere)
	float fActivationTime;
	FTimerHandle activationTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//To get weapon's type
	FORCEINLINE virtual WeaponType GetWeaponType() { return Type; }
	//To get weapon's animation state
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAnimationState GetAnimationState() { return AnimState; }
	//To get weapon's mesh
	FORCEINLINE virtual USkeletalMeshComponent* GetTPMesh() { return pTPRenderedMesh; }
	FORCEINLINE virtual USkeletalMeshComponent* GetFPMesh() { return pFPRenderedMesh; }

	FORCEINLINE virtual bool GetIsAttacking() { return bIsAttacking; }
	FORCEINLINE virtual bool GetIsAltAttacking() { return bIsAltAttacking; }

	//Sets owner reference to replace casting from GetOwner()
	UFUNCTION(Server, Reliable)
	virtual void Server_SetWeaponOwner(ABasePlayer* wepOwner);

	//Get damage type (used in engineer bullet)
	FORCEINLINE virtual TSubclassOf<UDamageType> GetWeaponDamageType() { return DamageType; }

	//To enable weapons when active
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetMC_Enable();
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetMC_Activate();
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetMC_Equip();
	//To disable weapons when put away
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetMC_Disable();
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetMC_UnEquip();

	//When we left click
	virtual void AttackStart();
	//Attack code
	virtual void Attack();
	//When we release left click
	virtual void AttackStop();

	//When we right click
	virtual void AltAttackStart();
	//AltAttack code
	virtual void AltAttack();
	//When we release right click
	virtual void AltAttackStop();

	//mainly for debugging stuff
	virtual void ResetVars();

	UFUNCTION(NetMulticast, Reliable)
	virtual void NetMC_PlaySound(class USoundCue* sound);

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_StopSound();

	UFUNCTION(NetMulticast, Reliable)
	virtual void NetMC_PlaySoundSingle(class USoundCue* sound);

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayParticleEffectAttached(class UNiagaraSystem* particleEffect, class USceneComponent* mesh, FName boneName, FVector pos = FVector::ZeroVector, FVector scale = FVector::OneVector, FRotator rot = FRotator::ZeroRotator);
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMC_PlayGunParticleEffectAttached(class UNiagaraSystem* particleEffect, class USceneComponent* mesh, FName boneName, FVector pos = FVector::ZeroVector, FVector scale = FVector::OneVector, FRotator rot = FRotator::ZeroRotator);

	UFUNCTION(Client, Unreliable)
	void Client_PlayParticleEffectAttached(class UNiagaraSystem* particleEffect, class USceneComponent* mesh, FName boneName, FVector pos = FVector::ZeroVector, FVector scale = FVector::OneVector, FRotator rot = FRotator::ZeroRotator);

	UFUNCTION(NetMulticast, Reliable)
	void NetMC_PlayParticleEffectAtLocation(class UNiagaraSystem* particleEffect, FVector pos = FVector::ZeroVector);
	
	UFUNCTION(Client, Reliable)
	void Client_PlayParticleEffectAtLocation(class UNiagaraSystem* particleEffect, FVector pos = FVector::ZeroVector);

	//Update animations for base weapons, virtual for weapon specific animations
	virtual void UpdateAnimation();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	void SetupSocketAttachments(class USkeletalMeshComponent* FPMesh, class USkeletalMeshComponent* TPMesh, FString socket);








};
