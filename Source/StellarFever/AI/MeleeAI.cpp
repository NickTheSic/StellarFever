//// Copyright Quarantine Games 2020
//
//
//#include "MeleeAI.h"
//#include "../Components/HealthComponent.h"
//#include "../StellarFeverGameModeBase.h"
//
//AMeleeAI::AMeleeAI()
//{
//	//PrimaryActorTick.bCanEverTick = false;
//	bMeleeAttacker = true;
//	fMovementDistance = 100.0;
//	bBossAttacker = false;
//
//	pHealthComp->DefaultHealth = 300;
//	pHealthComp->DefaultHealth *= fHealthIncrease;
//
//	fAttackRangeDistance = 50.f;
//	fAIMovementSpeed = 550.f;
//
//}
//
//void AMeleeAI::BeginPlay()
//{
//	Super::BeginPlay();
//
//	//Get GameMode
//	AStellarFeverGameModeBase* GameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
//	if (GameMode)
//	{
//		//AttackRangeDistance *= LevelDifficulty * fRangeIncrease; //Gamemode level difficulty
//	}
//}
//
