// Copyright Quarantine Games 2020


#include "RangeAI.h"
#include "../Components/HealthComponent.h"
#include "../StellarFeverGameModeBase.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ShieldAI.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/HealthComponent.h"
#include "EnemyShield.h"
#include "../Player/BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BossAI.h"
#include "../Components/RaycastComponent.h"
#include "BaseAIBoss.h"
#include "BossAIMelee.h"

ARangeAI::ARangeAI()
{
	PrimaryActorTick.bCanEverTick = true;

	bMeleeAttacker = false;
	fMovementDistance = 320.f;
	bBossAttacker = false;

	pHealthComp->DefaultHealth = 60.f;
	InitialHealth = pHealthComp->DefaultHealth;

	//pHealthComp->DefaultHealth *= fHealthIncrease;
	pRaycastComponent2 = CreateDefaultSubobject<URaycastComponent>("RaycastComponent2"); //Matt temp

	fAttackRangeDistance = 800.f;

	fAIMovementSpeed = 700;
	InitialSpeed = fAIMovementSpeed;
}

void ARangeAI::BeginPlay()
{
	Super::BeginPlay();

	//Get GameMode
	AStellarFeverGameModeBase* GameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		//AttackRangeDistance *= LevelDifficulty * fRangeIncrease; //Gamemode level difficulty
	}
}

void ARangeAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

		if (controller)
		{
			UObject* target = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
			targetPlayer = Cast<ABasePlayer>(target);
			if (targetPlayer)
			{
				DistanceTarget = (GetActorLocation() - targetPlayer->GetActorLocation()).Size();
			}
		}



		if (controller && controller->GetBlackboardComponent()->GetValueAsBool("BossProtect")) //may not work
		{
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
		}

		//FindBossAIMelee();
		FindShieldAI();

		if (controller)
		{
			bool bPatrol = controller->GetBlackboardComponent()->GetValueAsBool("Patrol");
			bool bCanSeePlayer = controller->GetBlackboardComponent()->GetValueAsBool("CanSeePlayer");

			if (bProtectingBoss && !bPatrol)
			{
				controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", true);
			}
			if (bProtectingBoss == false)
			{
				controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", false);
			}

			bool RunAway2 = controller->GetBlackboardComponent()->GetValueAsBool("RunAway2");
			bool StrafeAttack = controller->GetBlackboardComponent()->GetValueAsBool("StrafeAttack");

			//CHASE PLAYER
			if (controller && !RunAway2 && !StrafeAttack && bChasePlayers) //RunAway = false
			{
				controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true); //ChasePlayer all times
			}
		}
	}

	
}

void ARangeAI::FindShieldAI()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller)
	{		
		fNearestShieldAIDistance = 10000000.f;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShieldAI::StaticClass(), pOutShieldAI);


		for (int i = 0; i < pOutShieldAI.Num(); i++)
		{
			AShieldAI* shieldAI = Cast<AShieldAI>(pOutShieldAI[i]);

			if (shieldAI && targetPlayer && targetPlayer->GetHealthComponent()->bIsDead == false && DistanceTarget < 1630.f)
			{
				float Distance = (GetActorLocation() - pOutShieldAI[i]->GetActorLocation()).Size();
				if (Distance < fNearestShieldAIDistance)
				{
					fNearestShieldAIDistance = Distance;
					pBestShieldAI = pOutShieldAI[i];

					if (Distance < 1040.0f) //? change?
					{
						if (controller != nullptr)
						{
							AShieldAI* shieldAI2 = Cast<AShieldAI>(pBestShieldAI);

							if (shieldAI2)
							{
								bBehindAI = true;

								hidePosition = shieldAI2->HideBehindPosition->GetComponentLocation();

								if (shieldAI->pCurrentShield && shieldAI->pCurrentShield->fShieldHealth > 0)
								{
									//controller->MoveTo(hidePosition); BULLSHIT

									//UnCrouch();
									//bIsCrouching = false;
									controller->SetFocus(targetPlayer);
									UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, hidePosition); //?
									if (Distance < 620.0f)
									{
										UnCrouch();
										bIsCrouching = false;
									}
								}
								else
								{
									//Shield breaks
									bBehindAI = true;
								}
							}

						}
					}

				}
			}
			else if (bBehindAI && targetPlayer && targetPlayer->GetHealthComponent()->bIsDead == false)
			{
				//controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
				UnCrouch();
				bIsCrouching = false;
				ReturnToRegularMode();

			}
		}
	}
}

void ARangeAI::FindBossAIMelee()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller)
	{
		fNearestBossAIDistance = 10000000.f;

		//UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BossAI", pOutBossAI);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BossMelee", pOutBossAI);

		for (int i = 0; i < pOutBossAI.Num(); i++)
		{
			ABossAIMelee* bossAI = Cast<ABossAIMelee>(pOutBossAI[i]);

			//if (bossAI && bossAI->bDisabled == false && targetPlayer && targetPlayer->GetHealthComponent()->bIsDead == false && DistanceTarget < 3200.f) //TODO?? Range of BossAI merging
			if (bossAI && bossAI->bDisabled == false) //TODO?? Range of BossAI merging
			{
				float Distance = (GetActorLocation() - bossAI->GetActorLocation()).Size();
				if (Distance < fNearestBossAIDistance)
				{
					fNearestBossAIDistance = Distance;
					pBestBossAI = bossAI;

					bossAI2 = Cast<ABossAIMelee>(pBestBossAI);

					if (Distance < 1140.0f) //Distance from Boss
					{
						bProtectingBoss = true;
 						
						coverPosition = bossAI2->ProtectInfrontPosition->GetComponentLocation();
						controller->SetFocus(targetPlayer);
						controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
						controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", true);
						//controller->GetBlackboardComponent()->SetValueAsVector("BossVector", bossAI2->GetActorLocation());
						controller->GetBlackboardComponent()->SetValueAsVector("BossVector", coverPosition);
						bWithinBossRange = true;

						//UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, pBestBossAI->GetActorLocation()); //?

						if (Distance < 670.0f)
						{
							UnCrouch();
							bIsCrouching = false;

						}
					}
				}
			}
			//else if (bossAI2 && bossAI2->bDisabled == true && bWithinBossRange && targetPlayer && targetPlayer->GetHealthComponent()->bIsDead == false)
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

void ARangeAI::CoverAttack()
{

}