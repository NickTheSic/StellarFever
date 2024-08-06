// Copyright Quarantine Games 2020


#include "ShieldAI.h"
#include "..\Components/HealthComponent.h"
#include "EnemyShield.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../StellarFeverGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "SmokeGrenadeAI.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\Player\BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GeneratedCodeHelpers.h"
#include "ScavengerAI.h"
#include "..\Components/HealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AShieldAI::AShieldAI()
{
	PrimaryActorTick.bCanEverTick = true;

	//bMeleeAttacker = true;
	//fMovementDistance = 100.0f;
	//bBossAttacker = false;
	//GetCharacterMovement()->MaxWalkSpeed = 111.0;

	pHealthComp->DefaultHealth = 60;
	InitialHealth = pHealthComp->DefaultHealth;

	//pHealthComp->DefaultHealth *= fHealthIncrease;s

	HideBehindPosition = CreateDefaultSubobject<USceneComponent>("FollowPoint");
	HideBehindPosition->SetupAttachment(GetRootComponent());
	HideBehindPosition->ComponentTags.Add(FName("HideBehindPosition"));

	fAIMovementSpeed = 500; //refer back to movement speed restart
	InitialSpeed = fAIMovementSpeed;

	bShieldEnemy = true;

	//Gravity = 320.f;
	//Velocity = 1620.f;
}

void AShieldAI::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (GetLocalRole() == ROLE_Authority) //OOHHH REMEMBER make local authority so not more then 1 spawns
	{
		pCurrentShield = GetWorld()->SpawnActor<AEnemyShield>(StarterShieldClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (pCurrentShield)
		{
			pCurrentShield->SetOwner(this);
			pCurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "ShieldSocket");
		
		}
	}

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		UBehaviorTree* Tree = BehaviorTreeShield;
		if (Tree) // DO NOT CHANGE NAME BehaviorTree (will crash)
		{
			controller->UseBlackboard(Tree->BlackboardAsset, controller->BlackboardComponent);
			controller->RunBehaviorTree(Tree);
		}
	}

	//GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AShieldAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (pCurrentShield != nullptr && pCurrentShield->bDeactive)
	{
		pCurrentShield->SetActorHiddenInGame(true);
		//bIsUsedAsCover = false;

		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
		if (controller)
		{
			//if (UBehaviorTree* Tree = BehaviorTree) // DO NOT CHANGE NAME BehaviorTree (will crash)
			{
				controller->UseBlackboard(BehaviorTree->BlackboardAsset, controller->BlackboardComponent);
				controller->RunBehaviorTree(BehaviorTree);

				fAIMovementSpeed = 900.f; //important will anyone notice?
				//fAIMovementSpeed = InitialSpeed + (0.25f * pGameMode->GetDifficulyValue());

			}
		}

		if (bThrownGrenade == false)
		{
			ShotGrenade();
			bThrownGrenade = true;
		}
	}

	//FindingScavengerClosest(); //Protect nearest Scavenger

	//if (basePlayer && basePlayer->GetHealthComponent()->bIsDead && bReturnToRegular == false)
	//{
	//	ReturnToRegularMode(); //TODO!!!
	//	bReturnToRegular = true;

	//	if (scavenger)
	//		scavenger = nullptr;
	//}

	//IF SCAVENGER SUCCEEDS, Return this ShieldAI to regular triggers
	//if (scavenger && scavenger->bVaccineRetrieved && bReturnToRegular == false)
	//if (scavenger && scavenger->bVaccineRetrieved)
	//{
	//	ReturnToRegularMode();
	//	scavenger = nullptr;
	//	//bReturnToRegular = true;
	//}

	if (pCurrentShield && pCurrentShield->bDeactive == false)
	{
		pCurrentShield->pShieldMesh->SetVisibility(true);
		pCurrentShield->SetActorHiddenInGame(false);
		pCurrentShield->SetActorTickEnabled(true);
	}

	if (bDied)
	{
		pCurrentShield->Disable();
	}
}

void AShieldAI::Activate() //Switch to shield mode
{
	Super::Activate();

	//fAIMovementSpeed = 500.0f; //important
}

void AShieldAI::Reset()
{
	Super::Reset();
	bThrownGrenade = false;
}

void AShieldAI::ShotGrenade()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	FVector position = GetActorLocation();
	//position.X += 32.f;
	position.Z += 52.f;
	FRotator forwardRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//AFragGrenadeAI* FragGrenade = GetWorld()->SpawnActor<AFragGrenadeAI>(FragGrenadeClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	ASmokeGrenadeAI* SmokeGrenade = GetWorld()->SpawnActor<ASmokeGrenadeAI>(SmokeGrenadeClass, position, forwardRotation, SpawnParams);
}