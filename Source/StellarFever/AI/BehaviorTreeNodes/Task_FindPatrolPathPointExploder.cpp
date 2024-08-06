// Copyright Quarantine Games 2020


#include "Task_FindPatrolPathPointExploder.h"
#include "..\ExploderAI.h"
#include "..\ExploderAI_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\PatrolPath.h"

UTask_FindPatrolPathPointExploder::UTask_FindPatrolPathPointExploder(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Patrol Path Point EXPLODER C++");
}

EBTNodeResult::Type UTask_FindPatrolPathPointExploder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AExploderAI_AIController* controller = Cast<AExploderAI_AIController>(OwnerComp.GetAIOwner());
	AExploderAI* aiPawn = Cast<AExploderAI>(controller->GetPawn());

	int const index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");
	int const no_of_points = aiPawn->get_patrol_path()->num();

	if (aiPawn->get_patrol_path())
	{
		FVector const point = aiPawn->get_patrol_path()->get_patrol_point(index);
		// tranform this point to a global position using its parent
		FVector const global_point = aiPawn->get_patrol_path()->GetActorTransform().TransformPosition(point);

		// write the current global path point to the background
		controller->GetBlackboardComponent()->SetValueAsVector("PatrolPathVector", global_point);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	// get the current patrol path index from the blackboard
	// finish with success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
