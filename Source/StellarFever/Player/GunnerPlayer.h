// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "../Interfaces/RaycastNotifyable.h"
#include "BasePlayer.h"
#include "Engine/EngineTypes.h"
#include "GunnerPlayer.generated.h"

/**
 * This is the outline for the gunner player
 * We inherit from the base player to have a single networked class, and IRaycastNotifyable to assist with the gunners special
 * We override the UseSpecial() function and add the needed code to make it work here
 */
UCLASS()
class STELLARFEVER_API AGunnerPlayer : public ABasePlayer, public IRaycastNotifyable
{
	GENERATED_BODY()
	
public:
	AGunnerPlayer();

	bool UseSpecial() override;

	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class USphereComponent*     pShieldSphere  = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UStaticMeshComponent* pShieldingMesh = nullptr;

	/**
	 *The value will be out of 100
	 *I will randomly generate an int from 1- 100
	 *if it is Less than this value then we get 1 bullet
	 */
	UPROPERTY(EditAnywhere, Category = "Player | Gunner Config")
		int iChanceToRetrieveBullet = 6;


	UPROPERTY(EditAnywhere, Category = "Player | Gunner Shield Collision")
		FName ActiveCollisionProfileName = "OverlapAll";
	UPROPERTY(EditAnywhere, Category = "Player | Gunner Shield Collision")
		TEnumAsByte<ECollisionEnabled::Type> CollisionEnabledType = ECollisionEnabled::QueryOnly;


	virtual void Fire();
	virtual void StopFire();
	virtual void AltFire(); 
	virtual void StopAltFire(); 

	//Functions for the Functional Test
	bool ShieldMeshIsActive();
	bool ShieldColliderIsActive();

	using AActor::NotifyHit;
	virtual void NotifyHit(AActor* instigator, const FHitResult* hit = nullptr) override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, 
							class AController* InstigatedBy, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;

	virtual void TimerEndSpecial() override;

private:
	void DisableShield();
	UFUNCTION(Server, Reliable)
		void Server_DisableShield();

	void EnableShield();
	UFUNCTION(Server, Reliable)
		void Server_EnableShield();
};
