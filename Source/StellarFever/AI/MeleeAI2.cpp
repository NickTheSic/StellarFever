// Copyright Quarantine Games 2020


#include "MeleeAI2.h"
#include "../Components/HealthComponent.h"
#include "../StellarFeverGameModeBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "..\Ship parts\ShipDoor.h"
#include "BaseAICharacter_AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BaseAIBoss.h"
#include "..\Player\BasePlayer.h"
#include "BossAI.h"
#include "BossAIMelee.h"
#include "Animation/AnimMontage.h"

AMeleeAI2::AMeleeAI2()
{
	PrimaryActorTick.bCanEverTick = true;
	bMeleeAttacker = true;
	fMovementDistance = 100.0;
	bBossAttacker = false;

	pHealthComp->DefaultHealth = 60;
	InitialHealth = pHealthComp->DefaultHealth;

	//pHealthComp->DefaultHealth *= fHealthIncrease;

	fAttackRangeDistance = 60.f;

	fAIMovementSpeed = 820;
	fOriginalWalkSpeed = fAIMovementSpeed;

	InitialSpeed = fAIMovementSpeed;
}

void AMeleeAI2::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Door", pGetAllDoors);

	//Get GameMode
	AStellarFeverGameModeBase* GameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		//AttackRangeDistance *= LevelDifficulty * fRangeIncrease; //Gamemode level difficulty
	}


	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller && !bChasePlayers && !bShipPatroller && !bShipBoarded) //if event chase, turn off focus
	{
		controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AMeleeAI2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	
	FindNextTarget();

	// ALL Following BossAI Stuff
	if (controller)
	{
		UObject* target = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		targetPlayer2 = Cast<AActor>(target);
		if (targetPlayer2)
		{
			DistanceTarget = (targetPlayer2->GetActorLocation() - GetActorLocation()).Size();
		}
	}

	if (controller)//Meant for Following RangeBoss /////
	{
		bool bPatrol = controller->GetBlackboardComponent()->GetValueAsBool("Patrol");
		bool bCanSeePlayer = controller->GetBlackboardComponent()->GetValueAsBool("CanSeePlayer");
		bool bBossProtect = controller->GetBlackboardComponent()->GetValueAsBool("BossProtect");
		bool bChasePlayer2 = controller->GetBlackboardComponent()->GetValueAsBool("ChasePlayer");
		FVector BossVector = controller->GetBlackboardComponent()->GetValueAsVector("BossVector");

		//if (bProtectingBoss && !bPatrol)
		if (bChasePlayers)
		{
			if (!bProtectingBoss)
			{
				controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true);
			}
			if (bProtectingBoss)
			{
				//controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
				controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", true);
				//controller->MoveToLocation(BossVector);
			}
		}

		if (bProtectingBoss)
		{
			//controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
			//controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", false);
			//controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
			controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", true);
			//bChasePlayers = true;
			//bShipPatroller = false;

		}
	}

	FindBossAIRange();

	if (bStunned)
	{
		controller->ClearFocus(EAIFocusPriority::LastFocusPriority); //DUNNO
	}
}

void AMeleeAI2::Reset()
{	
	Super::Reset();

	//fAIMovementSpeed = InitialSpeed; //?

}

void AMeleeAI2::FindBossAIRange()
{
	if (!bProtectsShieldEnabled)
	{
		return;
	}

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller)
	{
		fNearestBossAIDistance = 10000000.f;

		//UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BossAI", pOutBossAI);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BossRange2", pOutBossAI2);

		for (int i = 0; i < pOutBossAI2.Num(); i++)
		{
			ABossAI* bossAI = Cast<ABossAI>(pOutBossAI2[i]);

			// && DistanceTarget < 3200.f removed
			if (bossAI && bossAI->bDisabled == false) //TODO?? Range of BossAI merging
			{
				float Distance = (GetActorLocation() - bossAI->GetActorLocation()).Size();
				if (Distance < fNearestBossAIDistance)
				{
					fNearestBossAIDistance = Distance;
					pBestBossAI = bossAI;

					bossAI2 = Cast<ABossAI>(pBestBossAI);

					bool bChasePlayer2 = controller->GetBlackboardComponent()->GetValueAsBool("ChasePlayer");

					//if (Distance <= 3200.0f && bossAI2 && !bChasePlayer2 && !bStunned) //Distance from Boss
					if (Distance <= 3200.0f && bossAI2 && !bStunned) //Distance from Boss
					{
						bProtectingBoss = true;

						coverPosition = bossAI2->ProtectInfrontPosition->GetComponentLocation();
						controller->SetFocus(targetPlayer2);
						//controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
						controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", true);
						//controller->GetBlackboardComponent()->SetValueAsVector("BossVector", bossAI->GetActorLocation());
						controller->GetBlackboardComponent()->SetValueAsVector("BossVector", coverPosition);
						bWithinBossRange = true;

						//UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, pBestBossAI->GetActorLocation()); //?

						if (Distance < 670.0f)
						{
							UnCrouch();
							bIsCrouching = false;

						}
					}

					if (Distance >= 1140.0f && !bChasePlayer2) //Distance from Boss
					{
						ReturnToRegularMode();
					}

					//if (Distance < 1140.0f && DistanceTarget < 1140.0f)
					if (Distance < 1140.0f && DistanceTarget < 1840.0f)
					{
						bProtectingBoss = false;
						controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true);
					}

				}
			}
			//else if (bossAI2 && bossAI2->bDisabled == true && bWithinBossRange && targetPlayer2 && targetPlayer2->GetHealthComponent()->bIsDead == false)
			else if (bossAI2 && bossAI2->bDisabled == true && bWithinBossRange)
			{
				UnCrouch();
				bIsCrouching = false;
				//bProtectingBoss = false;
				ReturnToRegularMode();
			}
		}

	}
}




void AMeleeAI2::Fire()
{
	Super::Fire();

	/*if (Attack1 && Attack2)
	{
		int randAttack = FMath::RandRange(0, 1);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (randAttack == 0)
		{
			AnimInstance->Montage_Play(Attack1, 1.f);
		}
		if (randAttack == 1)
		{
			AnimInstance->Montage_Play(Attack2, 1.f);
		}
	}*/
}

//OLD DOOR TICK

//void AMeleeAI2::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	//ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
//
//	//if (bGetDoors && bDoorReset)
//	//{
//	//	FindRandomDoor();
//	//	bDoorReset = false;
//	//}
//	//if (bGetDoors && CurrentDoor)
//	//{
//	//	UAIBlueprintHelperLibrary::SimpleMoveToActor(controller, CurrentDoor); //go after green door
//	//}
//
//	//if (CurrentDoor && CurrentDoor->GetLockedStatus() == true && !bReturnToBattle && bEndDoor)
//	//{
//	//	bReturnToBattle = true;
//	//	bEndDoor = false;
//	//	bGetDoors = false;
//
//	//	AfterEventsAreGone();
//	//}
//
//	//if (controller->GetBlackboardComponent()->GetValueAsBool("CanSeePlayer") && bGetDoors)
//	//{
//	//	bReturnToBattle = true;
//	//	bEndDoor = false;
//	//	bGetDoors = false;
//
//	//	//AfterEventsAreGone();
//	//	FindNextTarget();
//	//}
//}



//void AMeleeAI2::FindRandomDoor()
//{
//	//bChaseEvent = false;
//
//	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
//	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Door", pGetAllDoors);
//
//	bDoorProceed = false;
//
//	for (auto i : pGetAllDoors)
//	{
//		if (Cast<AShipDoor>(i)->GetLockedStatus() == false) //if any green
//		{
//			bDoorProceed = true; //if any green
//		}
//	}
//
//	if (pGetAllDoors.Num() > 0 && bDoorProceed)
//	{
//
//		//int eventRand = FMath::RandRange(0, FMath::Max(pGetAllEvents.Num() - 1, 0)); //Num need -1?
//		int doorRand = FMath::RandRange(0, pGetAllDoors.Num() - 1); //Num need -1?
//
//			//ABaseEvent* mechanism = Cast<ABaseEvent>(pGetAllEvents[eventRand]);
//		CurrentDoor = Cast<AShipDoor>(pGetAllDoors[doorRand]);
//
//		if (CurrentDoor && CurrentDoor->GetLockedStatus() && controller) //active means broken. red
//		{
//			GetWorldTimerManager().SetTimer(TimerHandle_DoorReset, this, &AMeleeAI2::FindRandomDoor, 0.1f, true); //checking without tick
//		}
//
//		if (CurrentDoor && CurrentDoor->GetLockedStatus() == false && controller) //green
//		{
//			GetWorldTimerManager().ClearTimer(TimerHandle_DoorReset);
//			bEndDoor = true;
//		}
//	}
//
//	if (!bEventProceed && !bShipPatroller) //if ALL doors become red
//	{
//		//AfterEventsAreGone();
//	}
//}
