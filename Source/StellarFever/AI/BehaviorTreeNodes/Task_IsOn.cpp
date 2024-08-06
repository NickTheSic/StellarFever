// Copyright Quarantine Games 2020


#include "Task_IsOn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UTask_IsOn::UTask_IsOn(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Turn bool ON C++");
}

EBTNodeResult::Type UTask_IsOn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
