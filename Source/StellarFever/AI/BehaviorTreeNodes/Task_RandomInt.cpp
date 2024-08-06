// Copyright Quarantine Games 2020


#include "Task_RandomInt.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UTask_RandomInt::UTask_RandomInt(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Random Int C++");
}

EBTNodeResult::Type UTask_RandomInt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	int random = FMath::RandRange(0, 1);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(GetSelectedBlackboardKey(), random);


	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
