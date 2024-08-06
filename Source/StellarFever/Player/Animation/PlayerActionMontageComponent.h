// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Weapons/WeaponBase.h"
#include "PlayerActionMontageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UPlayerActionMontageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerActionMontageComponent();

	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* GunAttackMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* GunAttackMontage_FirstPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* AltGunAttackMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* AltGunAttackMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		TArray<class UAnimMontage*> MeleeAttackMontage_ThirdPerson;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* MeleeAttackMontage_FirstPerson;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* AltMeleeAttackMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* AltMeleeAttackMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* GrenadeThrowMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* GrenadeThrowMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* GunReloadMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* GunReloadMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* WeaponSwitchMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* WeaponSwitchMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* RevivingPlayerMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* RevivingPlayerMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* EventInteractionMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* EventInteractionMontage_FirstPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* PlanetSelectMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* PlanetSelectMontage_FirstPerson = nullptr;


	UPROPERTY(EditAnywhere, Category = "Third Person")
		class UAnimMontage* ClassAbilityMontage_ThirdPerson = nullptr;

	UPROPERTY(EditAnywhere, Category = "First Person")
		class UAnimMontage* ClassAbilityMontage_FirstPerson = nullptr;


protected:

	UPROPERTY(VisibleAnywhere)
	class ABasePlayer* pOwningPlayer = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UAnimInstance* FirstPersonAnimInstance = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UAnimInstance* ThirdPersonAnimInstance = nullptr;

public:	

	void SetupOwningBasePlayer(class ABasePlayer* owner);


	void PlayAttackAnimation(EWeaponType weaponType, bool bIsAltFire = false);


	void PlayGunAttack(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, UnReliable)
		void NMC_PlayGunAttack(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayGunAttack(float PlayRate = 1.0f);

	void PlayAltGunAttack(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayAltGunAttack(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayAltGunAttack(float PlayRate = 1.0f);


	void PlayMeleeAttack(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayMeleeAttack(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayMeleeAttack(float PlayRate = 1.0f);


	void PlayAltMeleeAttack(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayAltMeleeAttack(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayAltMeleeAttack(float PlayRate = 1.0f);


	void PlayGrenadeThrow(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayGrenadeThrow(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayGrenadeThrow(float PlayRate = 1.0f);


	void PlayWeaponSwitchAnimation(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayWeaponSwitchAnimation(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayWeaponSwitchAnimation(float PlayRate = 1.0f);


	void PlayReloadAnimation(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayReloadAnimation(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayReloadAnimation(float PlayRate = 1.0f);


	void PlayEventInteractionAnimation(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayEventInteractionAnimation(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayEventInteractionAnimation(float PlayRate = 1.0f);


	void PlayPlanetSelectAnimation(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayPlanetSelectAnimation(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayPlanetSelectAnimation(float PlayRate = 1.0f);


	void PlayClassAbilityAnimation(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_PlayClassAbilityAnimation(float PlayRate = 1.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayClassAbilityAnimation(float PlayRate = 1.0f);
	
};
