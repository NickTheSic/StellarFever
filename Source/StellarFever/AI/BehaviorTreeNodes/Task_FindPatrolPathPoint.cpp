// Copyright Quarantine Games 2020


#include "Task_FindPatrolPathPoint.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\PatrolPath.h"

UTask_FindPatrolPathPoint::UTask_FindPatrolPathPoint(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Patrol Path Point C++");
}

EBTNodeResult::Type UTask_FindPatrolPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get the AI controller for the patrolling NPC
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* aiPawn = Cast<ABaseAICharacter>(controller->GetPawn());

	if (aiPawn->bShipPatroller == true)
	{
		int const index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");
		int const no_of_points = aiPawn->get_patrol_path()->num();

		// use the index to get the current patrol path from the NPC's reference to the patrol path
		if (aiPawn->get_patrol_path())
		{
			FVector const point = aiPawn->get_patrol_path()->get_patrol_point(index);
			// transform this point to a global position using its parent
			FVector const global_point = aiPawn->get_patrol_path()->GetActorTransform().TransformPosition(point);

			// write the current global path point to the blackboard
			controller->GetBlackboardComponent()->SetValueAsVector("PatrolPathVector", global_point);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}

	// get the current patrol path index from the blackboard
	// finish with success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
