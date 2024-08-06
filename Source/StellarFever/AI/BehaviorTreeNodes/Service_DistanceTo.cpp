// Copyright Quarantine Games 2020


#include "Service_DistanceTo.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\..\Player/BasePlayer.h"

UService_DistanceTo::UService_DistanceTo()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Distance to C++");
}

void UService_DistanceTo::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* node_memory)
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	if (controller)
	{
		ABaseAICharacter* pawn = Cast<ABaseAICharacter>(controller->GetPawn());
		if (pawn)
		{
			//NEW CODE
			fMaxDistance = pawn->fAttackRangeDistance;

			UObject* player = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey());
			AActor* playerActor = Cast<AActor>(player);
			ABasePlayer* playerBase = Cast<ABasePlayer>(playerActor);

			//if (playerBase && playerBase->bAlive) //dunno COMEBACK
			if (playerActor)
			{
				float Distance = (pawn->GetActorLocation() - playerActor->GetActorLocation()).Size();

				if (Distance >= fMaxDistance) //switch maybe
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, true); ///moving
				}
				if (Distance < fMaxDistance && pawn->bPlayerDetected == false)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, true);

				}
				//if (Distance < MaxDistance && pawn->bPlayerDetected == true && playerBase->bAlive) //new
				//{
				//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, true);

				//}
				if (Distance < fMaxDistance && pawn->bPlayerDetected == true)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, false);
				}

				//if (Distance >= MaxDistance && pawn->bPlayerDetected == false) //switch maybe
				//{
				//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, false);
				//}

				//if (Distance < MaxDistance && pawn->bPlayerDetected == false)
				//{
				//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, false);
				//}

				//if (Distance < MaxDistance && pawn->bPlayerDetected == true)
				//{
				//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsWithinDistance.SelectedKeyName, true);
				//}
			}
		}
	}
}
