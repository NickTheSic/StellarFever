// Copyright Quarantine Games 2020


#include "Service_SelectTargetActor.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "..\..\Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\Player/BasePlayer.h"

UService_SelectTargetActor::UService_SelectTargetActor()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Select Target Actor C++");
}

void UService_SelectTargetActor::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* targetMechanism = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BB_TargetMechanism.SelectedKeyName);

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* AIpawn = Cast<ABaseAICharacter>(controller->GetPawn());

	fNearestTargetDistance = 1000000.f;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), pOutActors);

	for (int i = 0; i < pOutActors.Num(); i++)
	{
		ABasePlayer* player = Cast<ABasePlayer>(pOutActors[i]);

		//if (player && player->GetHealthComponent()->Health != 0) //doesnt work, change to HEALTH != 0
		if (player && player->GetIsPlayerDowned() == false) //doesnt work, change to HEALTH != 0
		{
			float Distance = (AIpawn->GetActorLocation() - pOutActors[i]->GetActorLocation()).Size();
			if (Distance < fNearestTargetDistance)
			{
				fNearestTargetDistance = Distance;
				pBestTargetActor = pOutActors[i];

				OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), pBestTargetActor); //TargetActor

				//bool Patrolling = OwnerComp.GetBlackboardComponent()->GetValueAsBool("Patrol");
				//if (Patrolling == false)
				{
					//controller->SetFocus(pBestTargetActor);//new code, check
				}
			}
		}
	}
}
