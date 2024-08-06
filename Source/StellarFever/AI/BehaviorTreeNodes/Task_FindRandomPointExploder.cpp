// Copyright Quarantine Games 2020


#include "Task_FindRandomPointExploder.h"
#include "..\ExploderAI.h"
#include "..\ExploderAI_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\PatrolPath.h"

UTask_FindRandomPointExploder::UTask_FindRandomPointExploder(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Random Point Exploder C++");
}

EBTNodeResult::Type UTask_FindRandomPointExploder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AExploderAI_AIController* controller = Cast<AExploderAI_AIController>(OwnerComp.GetAIOwner());
	AExploderAI* aiPawn = Cast<AExploderAI>(controller->GetPawn());

	int index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");
	//make index random
	if (aiPawn->get_patrol_path()->num())
	{
		int no_of_points = aiPawn->get_patrol_path()->num() - 1;
		int random = FMath::RandRange(0, no_of_points);

		//use the index to get the current patrol path from the NPC's reference to the patrol path
		if (aiPawn->get_patrol_path())
		{
			FVector const point = aiPawn->get_patrol_path()->get_patrol_point(random);
			//transform this point to a global position using its parent
			FVector const global_point = aiPawn->get_patrol_path()->GetActorTransform().TransformPosition(point);

			//write the current global path to the blackboard
			controller->GetBlackboardComponent()->SetValueAsVector("PatrolPathVector", global_point);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}

	//get the current patrol path index from the blackboard
	//finish with success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}