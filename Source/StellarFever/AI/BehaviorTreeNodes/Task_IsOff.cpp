// Copyright Quarantine Games 2020


#include "Task_IsOff.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UTask_IsOff::UTask_IsOff(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Turn bool OFF C++");
}

EBTNodeResult::Type UTask_IsOff::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
