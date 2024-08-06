// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game starts
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);


	//UFUNCTION(Server, Reliable, WithValidation)
		//void Server_HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
			//class AController* InstigatedBy, AActor* DamageCauser);

public:

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category ="HealthComponent", Replicated)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", Replicated)
	float DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent", Replicated)
	bool bIsDead;


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() const;

	void TutorialSetHealth(float h) { Health = h; }


	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent", Replicated) //Add Replicated
		float fShieldAmount;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent", Replicated) //Add Replicated
		float fDefaultShieldAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", Replicated) //Add Replicated
		float fRegenerateShieldDelay = 5.f;

	FTimerHandle TimerHandle_StartHealing;

	UPROPERTY(Replicated)
	bool bShieldRegenerating;

	UFUNCTION()
	void StartRegenerating();

	void DrainShield(float damage);

	UPROPERTY()
		bool bDisableShieldRegeneration = false;

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	//static bool IsFriendly(AActor* ActorA, AActor* ActorB);


	// Called every frame
	///virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

	//
	//Start of Revive Functionality
	//

	//Handles setting the health to 20% of the Default Health
	//Simple so that we can get a working revive system implemented into the game
	UFUNCTION(Server, Reliable)
		void ServerSimpleRevive();

};
