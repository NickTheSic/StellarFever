// Copyright Quarantine Games 2020


#include "Decorator_LowHealth.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\..\Components/HealthComponent.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"

bool UDecorator_LowHealth::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	if (controller)
	{
		ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(controller->GetPawn());

		UHealthComponent* healthComp = Cast<UHealthComponent>(AIPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (healthComp && healthComp->GetHealth() <= fLowestHealth)
		{
			return true;
		}
	}

	return false;
}
