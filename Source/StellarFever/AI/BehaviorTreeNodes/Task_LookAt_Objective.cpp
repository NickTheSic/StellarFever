// Copyright Quarantine Games 2020


#include "Task_LookAt_Objective.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"


UTask_LookAt_Objective::UTask_LookAt_Objective(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Look At Objective C++");
}

EBTNodeResult::Type UTask_LookAt_Objective::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());

	if (controller)
	{
		ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(controller->GetPawn());

		//Use UObject
		UObject* player = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()); //works
		if (player)
		{
			AActor* playerActor = Cast<AActor>(player);
			if (playerActor && AIPawn->bLookTowardsObjective == false)
			{
				controller->SetFocus(playerActor);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
