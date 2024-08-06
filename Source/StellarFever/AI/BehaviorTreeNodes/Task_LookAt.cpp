// Copyright Quarantine Games 2020


#include "Task_LookAt.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "Kismet/KismetMathLibrary.h"

UTask_LookAt::UTask_LookAt(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Look At C++");
}

EBTNodeResult::Type UTask_LookAt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// LOOK AT ACTOR
	UObject* player = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()); //works
	if (player)
	{
		AActor* playerActor = Cast<AActor>(player);
		if (playerActor)
		{
			ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
			controller->SetFocus(playerActor);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}

	// LOOK AT VECTOR
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* aiPawn = Cast<ABaseAICharacter>(controller->GetPawn());

	//FVector destination = OwnerComp.GetBlackboardComponent()->GetValueAsVector(bLookAtVector.SelectedKeyName); //works
	//FVector viewDir = destination - aiPawn->GetActorLocation();
	////FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(aiPawn->GetActorLocation(), destination);
	//FRotator Rot = FRotationMatrix::MakeFromX(viewDir).Rotator();
	//aiPawn->SetActorRotation(Rot);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
