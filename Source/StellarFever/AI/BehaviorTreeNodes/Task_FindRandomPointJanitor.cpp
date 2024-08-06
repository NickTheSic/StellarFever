// Copyright Quarantine Games 2020


#include "Task_FindRandomPointJanitor.h"
#include "..\FriendlyAI\JanitorAI.h"
#include "..\PatrolPath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UTask_FindRandomPointJanitor::UTask_FindRandomPointJanitor(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Random Point Janitor C++");
}


EBTNodeResult::Type UTask_FindRandomPointJanitor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	AJanitorAI* aiPawn = Cast<AJanitorAI>(controller->GetPawn());

	int index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");

	if (aiPawn->get_patrol_path() && aiPawn->get_patrol_path()->num())
	{
		int no_of_points = aiPawn->get_patrol_path()->num() - 1;
		int random = FMath::RandRange(0, no_of_points);

		if (aiPawn->patrol_path)
		{
			FVector const point = aiPawn->get_patrol_path()->get_patrol_point(random);
			FVector const global_point = aiPawn->get_patrol_path()->GetActorTransform().TransformPosition(point);

			controller->GetBlackboardComponent()->SetValueAsVector("PatrolPathVector", global_point);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

//EBTNodeResult::Type UTask_FindRandomPointJanitor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
//{
//	AJanitorAI* aiPawn = Cast<AJanitorAI>(OwnerComp.GetAIOwner());
//	AAIController* controller = Cast<AAIController>(OwnerComp.GetAIOwner());
//
//	int index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");
//
//	if (aiPawn->get_patrol_path()->num())
//	{
//		int no_of_points = aiPawn->get_patrol_path()->num() - 1;
//		int random = FMath::RandRange(0, no_of_points);
//
//		if (aiPawn->get_patrol_path())
//		{
//			FVector const point = aiPawn->get_patrol_path()->get_patrol_point(random);
//			FVector const global_point = aiPawn->get_patrol_path()->GetActorTransform().TransformPosition(point);
//
//			controller->GetBlackboardComponent()->SetValueAsVector("PatrolPathVector", global_point);
//
//			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
//			return EBTNodeResult::Succeeded;
//		}
//	}
//
//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
//	return EBTNodeResult::Succeeded;
//}