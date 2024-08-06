// Copyright Quarantine Games 2020


#include "Task_HealSelf.h"
#include "..\BaseAICharacter_AIController.h"
#include "..\BaseAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UTask_HealSelf::UTask_HealSelf(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Heal Self C++");
}

EBTNodeResult::Type UTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	if (controller)
	{
		ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(controller->GetPawn());
		if (AIPawn)
		{
			AIPawn->HealSelfInitialize();
		}
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
