// Copyright Quarantine Games 2020


#include "ExploderAI_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ExploderAI.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "..\Player\BasePlayer.h"

AExploderAI_AIController::AExploderAI_AIController(FObjectInitializer const& object_initializer /*= FObjectInitializer::Get()*/)
{
	PrimaryActorTick.bCanEverTick = false; //TICK IS TURNED OFF

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
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AExploderAI_AIController::On_Target_Detected); //delegate to sight seeing!!
}

void AExploderAI_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AExploderAI_AIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);

	if (AExploderAI* AIExploder = Cast<AExploderAI>(pawn))
	{
		if (UBehaviorTree* Tree = AIExploder->BehaviorTree)
		{
			UseBlackboard(Tree->BlackboardAsset, BlackboardComponent);
			RunBehaviorTree(Tree);
		}
	}
}

void AExploderAI_AIController::Tick(float DeltaSeconds)
{	
	Super::Tick(DeltaSeconds);
}

void AExploderAI_AIController::On_Target_Detected(AActor* actor, FAIStimulus const stimulus)
{
	if (ABasePlayer* ch = Cast<ABasePlayer>(actor))
	{
		if (ch && ch->GetHealthComponent()->bIsDead == false)
		{
			GetBlackboardComponent()->SetValueAsBool("SecuringExploder", false);
			GetBlackboardComponent()->SetValueAsBool("AttackMode", true);
		}
	}
}
