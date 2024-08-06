// Copyright Quarantine Games 2020


#include "JanitorAI_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "JanitorAI.h"
#include "Net/UnrealNetwork.h"

AJanitorAI_AIController::AJanitorAI_AIController(FObjectInitializer const& object_initializer /*= FObjectInitializer::Get()*/)
{
	PrimaryActorTick.bCanEverTick = false; //TICK IS TURNED OFF

	SetReplicateMovement(true);
	SetReplicates(true);
	bAlwaysRelevant = true;
}

void AJanitorAI_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AJanitorAI_AIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (AJanitorAI* AIJanitor = Cast<AJanitorAI>(pawn))
		{
			if (UBehaviorTree* Tree = AIJanitor->BehaviorTree)
			{
				UseBlackboard(Tree->BlackboardAsset, BlackboardComponent);
				RunBehaviorTree(Tree);
			}
		}
	}
}

void AJanitorAI_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AJanitorAI_AIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJanitorAI_AIController, BlackboardComponent);
}