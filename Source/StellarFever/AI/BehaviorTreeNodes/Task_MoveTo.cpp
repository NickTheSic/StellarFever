// Copyright Quarantine Games 2020


#include "Task_MoveTo.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

UTask_MoveTo::UTask_MoveTo(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Move to C++");
}

EBTNodeResult::Type UTask_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(controller->GetPawn());

	fMovementSpeed = AIPawn->fAIMovementSpeed;

	bool chase = OwnerComp.GetBlackboardComponent()->GetValueAsBool(WithinRangeKey.SelectedKeyName);
	UObject* target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(target);

	if (chase && TargetActor)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, TargetActor->GetActorLocation());
		//controller->MoveTo(TargetActor->GetActorLocation());
		AIPawn->GetCharacterMovement()->MaxWalkSpeed = fMovementSpeed;
	}
	else if (!chase && TargetActor)
	{
		AIPawn->GetCharacterMovement()->StopMovementImmediately();
		AIPawn->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
