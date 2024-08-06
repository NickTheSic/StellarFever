// Copyright Quarantine Games 2020


#include "BaseAICharacter_AIController.h"
#include <Perception/AISenseConfig_Sight.h>
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseAICharacter.h"
#include "Engine/GameEngine.h"
#include "..\Player/BasePlayer.h"
#include "..\Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseAICharacter_AIController::ABaseAICharacter_AIController(FObjectInitializer const& object_initializer /*= FObjectInitializer::Get()*/)
{
	PrimaryActorTick.bCanEverTick = true;
	PerceptionComponent2 = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Sight Perception"));

	//Setup_Perception_System();
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	Sight_Config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	Sight_Config->SightRadius = 3200;
	Sight_Config->LoseSightRadius = 4000;
	Sight_Config->PeripheralVisionAngleDegrees = 270;
	Sight_Config->SetMaxAge(0.f);
	//Sight_Config->AutoSuccessRangeFromLastSeenLocation = -1.f;
	Sight_Config->DetectionByAffiliation.bDetectEnemies = true;
	Sight_Config->DetectionByAffiliation.bDetectNeutrals = true;
	Sight_Config->DetectionByAffiliation.bDetectFriendlies = true;

	Hearing_Config = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	Hearing_Config->HearingRange = 3200;
	Hearing_Config->DetectionByAffiliation.bDetectEnemies = true;
	Hearing_Config->DetectionByAffiliation.bDetectNeutrals = true;
	Hearing_Config->DetectionByAffiliation.bDetectFriendlies = true;

	Damage_Config = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));

	GetPerceptionComponent()->ConfigureSense(*Hearing_Config);
	GetPerceptionComponent()->SetDominantSense(*Hearing_Config->GetSenseImplementation());

	//GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ABaseAICharacter_AIController::On_Updated);
	GetPerceptionComponent()->ConfigureSense(*Sight_Config);
	GetPerceptionComponent()->ConfigureSense(*Damage_Config);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAICharacter_AIController::On_Target_Detected); //delegate to sight seeing!!

	bReassign = true;
}


void ABaseAICharacter_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAICharacter_AIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);

	if (ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(pawn))
	{
		if (UBehaviorTree* Tree = AICharacter->BehaviorTree) // DO NOT CHANGE NAME BehaviorTree (will crash)
		{
			UseBlackboard(Tree->BlackboardAsset, BlackboardComponent);
			RunBehaviorTree(Tree);
		}
	}
}

void ABaseAICharacter_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ABaseAICharacter* baseAI = Cast<ABaseAICharacter>(GetPawn());
}

void ABaseAICharacter_AIController::On_Target_Detected(AActor* actor, FAIStimulus const stimulus)
{

	if (ABasePlayer* ch = Cast<ABasePlayer>(actor))
	{
		//if (ch && ch->GetHealthComponent()->bIsDead == false && bReassign) //NEW
		if (ch && ch->GetIsPlayerDowned() == false && bReassign) //NEW
		{
			ABaseAICharacter* baseAI = Cast<ABaseAICharacter>(GetPawn());
			baseAI->bLookTowardsObjective = false;

			baseAI->bDipRange = true; //turn on when enemy detected

			bReassign = false;
			baseAI->FindNextTarget(); //COMEBACK?

			GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", stimulus.WasSuccessfullySensed());
			GetBlackboardComponent()->SetValueAsBool("Patrol", false);

			GetBlackboardComponent()->SetValueAsObject("TargetActor", ch); //DUNNO

			baseAI->bStandStill = false;
			baseAI->bChaseEvent = false;
			baseAI->bPlayerDetected = true;

			GetWorldTimerManager().SetTimer(TimerHandle_ReassignTarget, this, &ABaseAICharacter_AIController::Timer_Retarget, 0.1, false); //0.1 default

			ch->MakeNoise(0.5f); //In BaseAICharacter->Fire aswell

			if (baseAI->bShipPatroller) // if detected, stop investigating
			{
				GetBlackboardComponent()->SetValueAsBool("Investigating", false);
				GetBlackboardComponent()->SetValueAsBool("Securing", false);
			}

			baseAI->GetCharacterMovement()->MaxWalkSpeed = baseAI->fAIMovementSpeed;

			baseAI->bProtectingBoss = false;

		}
	}
}

void ABaseAICharacter_AIController::Timer_Retarget()
{
	bReassign = true;

}

void ABaseAICharacter_AIController::On_Seen(AActor* actor, FAIStimulus const stimulus) //GARBAGE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
}

void ABaseAICharacter_AIController::DipEverySoOften()
{
}
