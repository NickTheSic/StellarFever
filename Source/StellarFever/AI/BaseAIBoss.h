// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "BaseAICharacter.h"
#include "AIEnemy.h"
#include "..\Components/HealthComponent.h"
#include "BaseAIBoss.generated.h"

class USceneComponent;
class UHealthComponent;
class UDamageType;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABaseAIBoss : public ABaseAICharacter
{
	GENERATED_BODY()
public:
	ABaseAIBoss();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION() //delegate
	virtual void OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2,
		class AController* InstigatedBy, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowPoint")
		USceneComponent* ProtectInfrontPosition;


	UFUNCTION()
	virtual void StrafeFunction();

	bool bStrafeAway = false;
	FTimerHandle TimerHandle_Strafe;
};
