// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "ClassWeaponTemplates.h"
#include "GameFramework/Character.h"
#include "PlayerInformation.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayer.generated.h"


UCLASS()
class STELLARFEVER_API ABasePlayer : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

/*
* The following classes are allowed to access the private and protected functions
* This is to keep other classes from being able to accidentally access the functions
*/
	friend class UTeleportActorComponent;
	friend class UPlayerAnimInstance;
	friend class ATeleporter;
	friend class ATutorialManager2;

/*
* Member variables start here
*/
protected:

	UPROPERTY(EditAnywhere, Category = "Player | Class Configuration")
		FPlayerInformation playerInformation;


	// All the components are added alphabetically here
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UAIPerceptionStimuliSourceComponent* pAIPerception                   = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UCameraComponent*                    pCameraComponent                = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UHealthComponent*                    pHealthComponent                = nullptr;
	UPROPERTY(Replicated, EditAnywhere, Category = "Player | Inventory Component")
		class UInventoryComponent*                 pInventoryComponent             = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Sound Component")
		class UPlayerSoundComponent*               pPlayerSoundComponent           = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Animations")
		class UPlayerActionMontageComponent*       pPlayerAnimComponent            = nullptr;
	UPROPERTY()
		class URaycastComponent*                   pRaycastComponent	           = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UStatusComponent*                    pOxygenComponent                = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")					                   
		class UStatusComponent*                    pStaminaComponent               = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class USkeletalMeshComponent*              pFirstPersonPlayerMeshComponent = nullptr;
	UPROPERTY()
		class USkeletalMeshComponent*              pThirdPersonPlayerMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Player | Components")
		class UWidgetInteractionComponent*         pWidgetInteractionComponent     = nullptr;


	//Pointers to interacting objects
	UPROPERTY(BlueprintReadOnly, Replicated)
		class AWeaponBase* pActiveWeapon  = nullptr;

	UPROPERTY(BlueprintReadOnly, Replicated)
		class APickUpItem* pPickupItem    = nullptr;


	//Timer Handles
	FTimerHandle TimerHandle_ActiveAbilityCooldown;
	FTimerHandle TimerHandle_ActiveAbilityEnd;
	FTimerHandle TimerHandle_GrenadeCooldown;
private:
	FTimerHandle TimerHandle_ReviveOrDie;

protected:
	UPROPERTY(Replicated)
		uint16 _PlayerStatusFlag;

public:
	UPROPERTY(EditAnywhere)
		FVector StandingCamera = FVector(-9.56f, 1.75f, 64.0f);


/*
* The following function definitions can be found in BasePlayer.cpp
*/
public:
	// Sets default values for this character's properties
	ABasePlayer(); 

	// Called every frame
	virtual void Tick(float DeltaTime) override; 
	virtual void PostInitializeComponents() override; 

	void StartInteraction();
	UFUNCTION(Server, Reliable)
		void Server_StartInteraction();

	void StopInteraction(); 
	UFUNCTION(Server, Reliable)
		void Server_StopInteraction();

	void ClassSpecial();
	virtual bool UseSpecial();

	UFUNCTION(NetMulticast, Reliable)
	void IgnorePlayerInput(bool bNewIgnore); 

	UFUNCTION(Client, Reliable)
		void SetupWidgetInteractionUserID();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
		virtual void TimerEndSpecial();

private:
	void HandlePlayerIsRunning(float delta);

	void HandleOxygenComponentTick(float DeltaTime);
	void HandleStaminaComponentTick(float DeltaTime);

	UFUNCTION()
		void SetCanUseSpecialAgain();

	FORCEINLINE bool HasPickupBeenPickedUp() { return pPickupItem != nullptr; }
	void HandlePickupPickUp();
	void HandlePickupSetDown();

public:
	void SetPlayerNewSpeed(float newSpeed);
	UFUNCTION(Client, Unreliable)
		void ClientPlayerNewSpeed(float newSpeed);


/*
* Contained in BasePlayer_MovementInputs.cpp  
*/
public:
	void StartRunning(); 
	void StopRunning(); 

	void Jump() override;
	void MoveForward(float val); 
	void MoveRight(float val);

public:
	void PlayerBeginCrouch();
	void PlayerEndCrouch(); 

protected:
	void HandleMakeMovementNoise(); 


/* 
* Contained in BasePlayer_WeaponHandling.cpp  
*/
public:
	UFUNCTION(Server, Reliable)
		void Server_Reload();

	virtual void Fire(); 
	UFUNCTION(Server, Unreliable)
		void Server_Fire();

	virtual void StopFire();
	UFUNCTION(Server, Unreliable)
		void Server_StopFire();

	virtual void AltFire();
	UFUNCTION(Server, Unreliable)
		void Server_AltFire();

	virtual void StopAltFire();
	UFUNCTION(Server, Unreliable)
		void Server_StopAltFire();

	void Throw();
	UFUNCTION(Server, Unreliable)
		void Server_Throw();

	UFUNCTION(Server, Reliable)
	void ReplenishAmmo(uint16 ammo);
	

/* 
*Contained in BasePlayer_StatusComponentHandling   
*/
public:
	//Component wrapper type functions
	void DrainOxygen(float val);
	void ReplenishOxygen(float val); 
	void DrainMaxOxygen(float val);
	void ResetMaxOxygen(float val);

	void DrainStamina(float val); 
	void ReplenishStamina(float val); 

	void RestoreHealth(float healAmount);
	void DrainHealth(float drainAmount); 

	void DrainHitShield(float drainAmount);


	UFUNCTION()
		void OnCharacterNoHealth();
	UFUNCTION(Client, Reliable)
		void  Client_OnCharacterNoHealth();
	UFUNCTION(Server, Reliable)
		void Server_OnCharacterNoHealth();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnCharacterNoHealth();

	UFUNCTION() //delegate
		virtual void OnHealthChanged(UHealthComponent* InHealthComp, float Health, float HealthDelta, 
							const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION()
		void OnCharacterDeath();
	UFUNCTION(Server, Reliable)
		void Server_OnCharacterDeath();


	//Since we are dealing with a death timer we need to pause while being revived
	//We need to tell the player when they start and if they stop early
	void StartRevive();

	UFUNCTION(Server, Reliable)
		void Server_StartRevive();

	void StopRevive();

	void OnCharacterRevived();
	UFUNCTION(Client, Reliable)
		void  Client_OnCharacterRevived();
	UFUNCTION(Server, Reliable)
		void Server_OnCharacterRevived();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnCharacterRevived();


/*
*  The following functions are contained in BasePlayer_InventoryHandling.cpp
*/ 
public:
	void MouseWheelUp(); 
	UFUNCTION(Server, Reliable)
		void Server_MouseWheelUp();

	void MouseWheelDown();
	UFUNCTION(Server, Reliable)
		void Server_MouseWheelDown();

	void SwitchToSlotOne(); 
	UFUNCTION(Server, Reliable)
		void Server_SwitchToSlotOne();

	void SwitchToSlotTwo(); 
	UFUNCTION(Server, Reliable)
		void Server_SwitchToSlotTwo();

	void SwitchToSlotThree(); 
	UFUNCTION(Server, Reliable)
		void Server_SwitchToSlotThree();

	void SetCanAccessInventory(bool bAccess);

	//Called in multiplayer
	//Used to make sure all players have switched to the gun
	void InitializeInventoryForPlay();

protected:
	void DisableInventory(); 
	void EnableInventory();  
	

/*
* The following functions are defined in BasePlayer_StatusFlagHandling.cpp
*/
public:
	void ResetPlayerStatusFlag();
	UFUNCTION(Server, Reliable)
		void Server_ResetPlayerStatusFlag();

	void AddToPlayerStatusFlag(EPlayerStatus addStatus);
	UFUNCTION(Server, Reliable)
		void Server_AddToPlayerStatusFlag(EPlayerStatus addStatus);
protected:
	void RemoveFromPlayerStatusFlag(EPlayerStatus removeStatus);
	UFUNCTION(Server, Reliable)
		void Server_RemoveFromPlayerStatusFlag(EPlayerStatus removeStatus);

	void UpdatePlayerSpeed();
	UFUNCTION(Server, Reliable)
		void Server_UpdatePlayerSpeed();


/*
* Functions to handle timer setting and percentages
*/
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	float GetClassSpecialPercentageCharged()
	{
		float Percentage = playerInformation.fActiveAbilityCooldown;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle_ActiveAbilityCooldown))
			Percentage -= GetWorldTimerManager().GetTimerRemaining(TimerHandle_ActiveAbilityCooldown);

		return Percentage / playerInformation.fActiveAbilityCooldown;
	}

	void StartGrenadeTimer()
	{
		GetWorldTimerManager().SetTimer(TimerHandle_GrenadeCooldown, this, &ABasePlayer::StopGrenadeTimer, 30.0f, false);
	}
	void StopGrenadeTimer()
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_GrenadeCooldown);
	}
	FORCEINLINE	float GetGrenadePercentage()
	{
		float Percentage = 30.0f;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle_GrenadeCooldown))
			Percentage -= GetWorldTimerManager().GetTimerRemaining(TimerHandle_GrenadeCooldown);

		return Percentage / 30.0f;
	}


/*
* Blueprint callable get functions so other classes are able to get the player components
*/
public:
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UCameraComponent*              GetCamera()                   { return pCameraComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class USkeletalMeshComponent*        GetFirstPersonMesh()          { return pFirstPersonPlayerMeshComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class USkeletalMeshComponent*        GetThirdPersonMesh()          { return pThirdPersonPlayerMeshComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class UHealthComponent*              GetHealthComponent()          { return pHealthComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class UInventoryComponent*           GetInventoryComponent()       { return pInventoryComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class UStatusComponent*              GetOxygenComponent()          { return pOxygenComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class UStatusComponent*              GetStaminaComponent()         { return pStaminaComponent; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPlayerActionMontageComponent* GetPlayerAnimationComponent() { return pPlayerAnimComponent; }
												         							    
	UFUNCTION(BlueprintPure)					         							    
		FORCEINLINE class UPlayerSoundComponent*         GetPlayerSoundComponent()     { return pPlayerSoundComponent; }
												         							    
	UFUNCTION(BlueprintPure)					         							    
		FORCEINLINE class URaycastComponent*             GetRaycastComponent()         { return pRaycastComponent; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE class AWeaponBase*                   GetActiveWeapon()             { return pActiveWeapon; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE       EPlayerClass			         GetPlayerClass()              { return playerInformation.ePlayerClass; }
													       					       	   
	UFUNCTION(BlueprintPure)						       					       	   
		FORCEINLINE       FPlayerInformation	         GetPlayerInformation()        { return playerInformation; }


public: 
/*
* These functions are to check with the status flag in a human readable way
*/
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerDead()        
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::Dead) == (uint16)EPlayerStatus::Dead; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerDowned()      
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::Downed) == (uint16)EPlayerStatus::Downed; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerRunning()     
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::Running) == (uint16)EPlayerStatus::Running; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerCrouched()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::Crouched) == (uint16)EPlayerStatus::Crouched; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerHoldingItem() 
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::HoldingPickup) == (uint16)EPlayerStatus::HoldingPickup; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetSpecialIsActive()     
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::SpecialIsActive) == (uint16)EPlayerStatus::SpecialIsActive; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetCanUseSpecial()       
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::CanUseSpecial) == (uint16)EPlayerStatus::CanUseSpecial; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsPlayerInMenu()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::PlayerInMenu) == (uint16)EPlayerStatus::PlayerInMenu; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsInteracting()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::IsInteracting) == (uint16)EPlayerStatus::IsInteracting; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsFiring()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::IsFiring) == (uint16)EPlayerStatus::IsFiring; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsEnraged()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::Enraged) == (uint16)EPlayerStatus::Enraged; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsOnEnemyShip()
			{ return (_PlayerStatusFlag & (uint16)EPlayerStatus::OnEnemyShip) == (uint16)EPlayerStatus::OnEnemyShip;}


	class APickUpItem* GetPickupItem() const { return pPickupItem; }

};
