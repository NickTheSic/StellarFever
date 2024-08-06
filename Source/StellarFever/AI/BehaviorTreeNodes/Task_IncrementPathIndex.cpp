// Copyright Quarantine Games 2020


#include "Task_IncrementPathIndex.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\PatrolPath.h"
#include <cmath>

UTask_IncrementPathIndex::UTask_IncrementPathIndex(FObjectInitializer const& object_initializer)
{	
	NodeName = TEXT("Increment Path Index C++");
}

EBTNodeResult::Type UTask_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get the AI controller
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* aiPawn = Cast<ABaseAICharacter>(controller->GetPawn());
	int const no_of_points = aiPawn->get_patrol_path()->num();
	int const min_index = 0;
	int const max_index = no_of_points - 1;

	bidirectional = aiPawn->bBiDirectional;

	 // get and set the blackboard indSex key
	 int index = controller->GetBlackboardComponent()->GetValueAsInt("PatrolPathIndex");
	 if (bidirectional)
	 {
		 if (index >= max_index && direction == EDirectionType::Forward)
		 {
			 direction = EDirectionType::Reverse;
		 }
		 else if (index == min_index && direction == EDirectionType::Reverse)
		 {
			 direction = EDirectionType::Forward;
		 }
	 }

	 controller->GetBlackboardComponent()->SetValueAsInt("PatrolPathIndex", 
		(direction == EDirectionType::Forward ? std::abs(++index) : std::abs(--index)) % no_of_points);
	
	aiPawn->ChangeDirectionIncrement();

	//finish with success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
