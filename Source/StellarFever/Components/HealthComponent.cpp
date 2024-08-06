// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
	// Team Num = 255

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	DefaultHealth = 100;
	fDefaultShieldAmount = 100.f;
	bIsDead = false;
	fShieldAmount = fDefaultShieldAmount;

	bShieldRegenerating = false;

}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		//if (GetOwnerRole() == ROLE_Authority)
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}

	
	Health = DefaultHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bDisableShieldRegeneration && bShieldRegenerating)
	{
		fShieldAmount += DeltaTime * 16.f;
	}

	if (fShieldAmount > fDefaultShieldAmount)
	{
		fShieldAmount = fDefaultShieldAmount;
	}
}


void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StartHealing, this, &UHealthComponent::StartRegenerating, fRegenerateShieldDelay, false);
	bShieldRegenerating = false;


	if (fShieldAmount <= 0.f)
	{
		Health -= Damage;

		if (Health <= 0.0f)
		{
			bIsDead = true;
		}

		//OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	}

	if (fShieldAmount > 0)
	{
		fShieldAmount -= Damage;
		
		if (fShieldAmount < 0)
		{
			Health += fShieldAmount; //reverse, subtracts
			fShieldAmount = 0.f;
		}
	}
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);


	/*GetWorld()->GetTimerManager().SetTimer(TimerHandle_StartHealing, this, &UHealthComponent::StartRegenerating, fRegenerateShieldDelay, false);
	bShieldRegenerating = false;

	if (fShieldAmount > 0)
	{
		fShieldAmount = FMath::Clamp(fShieldAmount - Damage, 0.0f, fDefaultShieldAmount);
	}
	else
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

		UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

		bIsDead = Health <= 0.0f;

		OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	}*/

}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0 || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr); // if health is less than 0, then die
}

float UHealthComponent::GetHealth() const
{
	return Health;
}

float UHealthComponent::GetHealthPercentage() const
{
	return Health / DefaultHealth;
}

void UHealthComponent::StartRegenerating()
{
	bShieldRegenerating = true;
}

void UHealthComponent::DrainShield(float damage)
{
	fShieldAmount = FMath::Clamp(fShieldAmount - damage, 0.0f, fDefaultShieldAmount);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StartHealing, this, &UHealthComponent::StartRegenerating, fRegenerateShieldDelay, false);
	bShieldRegenerating = false;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, DefaultHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);

	DOREPLIFETIME(UHealthComponent, fShieldAmount);
	DOREPLIFETIME(UHealthComponent, fDefaultShieldAmount);
	//DOREPLIFETIME(UHealthComponent, fRegenerateShieldDelay);

	DOREPLIFETIME(UHealthComponent, bShieldRegenerating);
	DOREPLIFETIME(UHealthComponent, fRegenerateShieldDelay);

}


//
//Start of Revive Functionality
//

void UHealthComponent::ServerSimpleRevive_Implementation()
{
	//Set the health to 20% of the max health
	Health = DefaultHealth * 0.2f;
	//Subtract 1 from the current health
	Health -= 1.0f;
	//Heal 1 so that the OnHealthChanged function gets triggered
	Heal(1.0f);
	//Set bIsDead to false since we are healed again
	bIsDead = false;
}

