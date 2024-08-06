// Copyright Quarantine Games 2020


#include "BaseAIBoss.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "..\Player\BasePlayer.h"
#include "..\Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseAIBoss::ABaseAIBoss()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseAIBoss::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		//pHealthComp->OnHealthChanged.AddDynamic(this, &ABaseAIBoss::OnHealthChanged);
	}
}

void ABaseAIBoss::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
{
	Super::OnHealthChangedInternal(InHealthComp, Health, HealthDelta, DamageType2, InstigatedBy, DamageCauser);
}

void ABaseAIBoss::StrafeFunction()
{

}

void ABaseAIBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	
	FindNextTarget();

	if (controller)
	{
		bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");
		UObject* target = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		ABasePlayer* player = Cast<ABasePlayer>(target);

		if (player && !HealRun)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			controller->SetFocus(player);
		}
		if (HealRun)
		{
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}

		if (bStunned)
		{
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority); //DUNNO
		}
	}
}
//
//void ABaseAIBoss::OnHealthChanged(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
//{
//
//}
