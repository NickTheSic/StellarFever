// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"
#include "../Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BaseAICharacter_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "..\Player/BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "..\Components/HealthComponent.h"
#include "WeaponAI.h"
#include "EnemyRifleWeapon.h"
#include "EnemyShotgunWeapon.h"
#include "EnemyMeleeWeapon.h"
#include "../Components/RaycastComponent.h"
#include "PatrolPath.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\StellarFeverGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "..\Events/BaseEvent.h"
#include "AIEnemy.h"
#include "EnemyShield.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/Actor.h"
#include "..\Ship parts\ShipDoor.h"
#include "ScavengerAI.h"
#include "Animation/AnimBlueprintGeneratedClass.h"


// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bStopFiring = false;

	bLookTowardsObjective = true;

	bShipBoarded = false;
	bStandStill = true;

	fReturnToEventDistance = 4200.f;

	pRaycastComponent = CreateDefaultSubobject<URaycastComponent>("RaycastComponent"); //Matt temp

	bWithinRange = false;
	bChaseEvent = false;

	//SetReplicates(true);
	//SetReplicateMovement(true);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseAICharacter::OnPawnSeen);

	bMeleeAttacker = false;
	bBiDirectional = false; //patrols in a circle
}

// Called when the game starts or when spawned
void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		//pHealthComp->OnHealthChanged.AddDynamic(this, &ABaseAICharacter::OnHealthChanged);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	pCurrentWeapon = GetWorld()->SpawnActor<AWeaponAI>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (pCurrentWeapon)
	{
		AEnemyRifleWeapon* rifle = Cast<AEnemyRifleWeapon>(pCurrentWeapon);
		if (rifle)
		{
			pCurrentWeapon->SetOwner(this);
			pCurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "RifleSocket");
		}
		AEnemyShotgunWeapon* shotgun = Cast<AEnemyShotgunWeapon>(pCurrentWeapon);
		if (shotgun)
		{
			pCurrentWeapon->SetOwner(this);
			pCurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "ShotgunSocket");
		}
		AEnemyMeleeWeapon* melee = Cast<AEnemyMeleeWeapon>(pCurrentWeapon);
		if (melee)
		{
			pCurrentWeapon->SetOwner(this);
			pCurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "MeleeHitPoint");
		}
	}

	fOriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;


	if (!bShipBoarded)
	{
		bChaseEvent = true;
	}

	// Sets patrolling ai to investigating at startup (when boarded their ship)
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (bShipPatroller && bShipBoarded && controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("Investigating", true);
		GetCharacterMovement()->bOrientRotationToMovement = true; //TODOS

	}
	if (bShipPatroller && !bShipBoarded && controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("Securing", true);
		GetCharacterMovement()->bOrientRotationToMovement = true; //TODOS
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "MainEvent", pGetAllEvents); //goes before GetRandomEvent
	GetRandomEvent();

	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("StrafeAttack", true);
	}

	if (bChasePlayers && controller) //dont limit distance if chase ??NOT WORKING - DOESNT RUNAWAY
	{
		UObject* object = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		ABasePlayer* player = Cast<ABasePlayer>(object);
		//controller->SetFocus(player);
		controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
		controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
	}

	if (controller && (controller->GetBlackboardComponent()->GetValueAsObject("Patrol") || controller->GetBlackboardComponent()->GetValueAsObject("Securing")))
	{
		controller->ClearFocus(EAIFocusPriority::Gameplay);
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	//Protect Scavenger Chance
	//int randomChance = FMath::RandRange(0, 2);
	int randomChance = FMath::RandRange(0, 0);
	if (controller)
	{
		if (randomChance == 0)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("ProtectScavengerChance", true);
		}
		else
		{
			controller->GetBlackboardComponent()->SetValueAsBool("ProtectScavengerChance", false);
		}
	}
}

void ABaseAICharacter::OnPawnSeen(APawn* SeenPawn) //Does RangeAI/BossAI stuff
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (bMeleeAttacker)
	{
		FindNextTarget();
		controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
		controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Seen");

	FindNextTarget(); //????

	if (ABasePlayer* ch = Cast<ABasePlayer>(SeenPawn))
	{
		//if (ch && ch->GetHealthComponent()->bIsDead == false) //if player is dead
		if (ch && ch->GetIsPlayerDowned() == false) //if player is dead
		{
			//Chooses closest character
			FindNextTarget();

			bool bIsInCover = controller->GetBlackboardComponent()->GetValueAsBool("InCover"); //NEW

			if ((!bIsCrouching || !bIsCrouched) && !bBossAttacker && !bStandupPeriod && bIsInCover) //Get rid of BossAttacker bool IF you want crouching!!! REMEMBER TODO!
			{
				int randCrouch = FMath::RandRange(1, 3);
				GetWorldTimerManager().SetTimer(TimerHandle_StandupPeriod, this, &ABaseAICharacter::StandupPeriod, randCrouch, false);
				bStandupPeriod = true;
			}

			if ((bIsCrouching || bIsCrouched) && bIsInCover)
			{
				bStandupPeriod = true;
				GetWorldTimerManager().ClearTimer(TimerHandle_StandupPeriod);

				controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
				controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
				bIsCrouching = false;
				UnCrouch();
			}

			if ((!bIsCrouching || !bIsCrouched) && !bIsInCover) //if false  NEW DONT KNOW YET
			{
				controller->GetBlackboardComponent()->SetValueAsBool("StrafeAttack", true); //test strafing
			}
			else
			{
				controller->GetBlackboardComponent()->SetValueAsBool("StrafeAttack", false); //test strafing

			}
		}

	}
}


void ABaseAICharacter::GetRandomEvent()
{

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), "MainEvent", pGetAllEvents);

	bEventProceed = false;

	for (auto i : pGetAllEvents)
	{
		if (Cast<ABaseEvent>(i)->GetIsActive() == false) //if any green
		{
			bEventProceed = true; //if any green
		}
	}


	if (pGetAllEvents.Num() > 0 && bEventProceed)
	{
		int eventRand = FMath::RandRange(0, pGetAllEvents.Num() - 1); //Num need -1?

			//ABaseEvent* mechanism = Cast<ABaseEvent>(pGetAllEvents[eventRand]);
		CurrentMechanism = Cast<ABaseEvent>(pGetAllEvents[eventRand]);

		if (CurrentMechanism && CurrentMechanism->GetIsActive() && controller) //active means broken. red
		{
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);
			GetWorldTimerManager().SetTimer(TimerHandle_ResetAI, this, &ABaseAICharacter::GetRandomEvent, 0.1f, true); //checking without tick

			//FindNextTarget();
		}

		if (CurrentMechanism && CurrentMechanism->GetIsActive() == false && controller) //green
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_ResetAI);

			UObject* mechanismObject = Cast<UObject>(CurrentMechanism);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);
			controller->GetBlackboardComponent()->SetValueAsObject("TargetMechanism", mechanismObject);
		}
	}

	if (!bEventProceed && !bShipPatroller)
	{
		AfterEventsAreGone();
	}

}

void ABaseAICharacter::AfterEventsAreGone()
{

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller)
	{
		//What to do after all events are destroyed
		if (SecurePaths.Num() > 0) //Secure 100% if SecurePath does exist
		{
			controller->GetBlackboardComponent()->SetValueAsBool("Securing", true);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);

			bChasePlayers = false;
			bShipBoarded = false;
			bShipPatroller = true;
			patrol_path = Cast<APatrolPath>(SecurePaths[0]);
		}
		if (SecurePaths.Num() == 0) //Chase Player
		{
			//Chase Players
			bChasePlayers = true;
			bShipBoarded = false;
			bShipPatroller = false;


			controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
			FindNextTarget();
		}

	}
}


void ABaseAICharacter::Fire()
{
	if (bStopFiring || bStunned || bDontFire)
	{
		return;
	}

	//Player makes noise when enemy fires (others detect from around)
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	UObject* playerObject = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
	if (playerObject)
	{
		ABasePlayer* playerBase = Cast<ABasePlayer>(playerObject);
		if (playerBase)
		{
			playerBase->MakeNoise(0.5f, playerBase, GetActorLocation());
			//IMPORTANT: Enemies detection = 0.5f (Inside BaseAICharacter Fire AND BaseAICharacter_AIController On_target_detected

			UHealthComponent* healthComp = Cast<UHealthComponent>(playerBase->GetComponentByClass(UHealthComponent::StaticClass()));
			if (healthComp && healthComp->bIsDead == true)
			{
				return; //dunno yet
			}
		}
	}

	AEnemyRifleWeapon* rifle = Cast<AEnemyRifleWeapon>(pCurrentWeapon);
	if (rifle)
	{
		pCurrentWeapon->FireAutoAI();
	}

	AEnemyShotgunWeapon* shotgun = Cast<AEnemyShotgunWeapon>(pCurrentWeapon);
	if (shotgun)
	{
		pCurrentWeapon->FireSingleAI();
	}

	AEnemyMeleeWeapon* melee = Cast<AEnemyMeleeWeapon>(pCurrentWeapon);
	if (melee)
	{
		pCurrentWeapon->FireSingleAI();

		/*if (CurrentWeapon->bFireAnim)
		{
			if (MeleeAnimation != NULL)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(MeleeAnimation, 1.f);
				}
			}
		}*/
	}
}

void ABaseAICharacter::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::SanitizeFloat(Health));

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller)
	{
		ABasePlayer* playerWhoShotYou = Cast<ABasePlayer>(DamageCauser);

		if (playerWhoShotYou)
		{
			controller->GetBlackboardComponent()->SetValueAsObject("TargetActor", playerWhoShotYou);
		}

		WithinRangeDip();

		bool bHealingBoss = controller->GetBlackboardComponent()->GetValueAsBool("Healing");

		//if (!bMeleeAttacker && bIsCrouching && !bBossAttacker)
		if (!bMeleeAttacker && bIsCrouching) //Sitting down and shot (behind wall) CHANGE TODO if boss crouches!
		{
			bIsCrouching = false;
			UnCrouch();
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false); //delete? TODO
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true); //delete? TODO

		}
		if (bBossAttacker && !bHealingBoss)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
		}

		//DUNNO! TEST!!!!!!!!!!!!!!!!!!
		if (!bMeleeAttacker && !bBossAttacker)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
		}


		if (Health > 0 && !bDied && bStunned)
		{
			StunnedRelease();
		}

		if (Health <= 0 && !bDied)
		{
			DropItem();

			GetWorldTimerManager().ClearTimer(TimerHandle_DipTimer);
			UGameplayStatics::PlaySoundAtLocation(this, pDeathSounds, GetActorLocation());
			bDied = true;

			//DetachFromControllerPendingDestroy();

			//////////////////////////////////////////////////////
			SetActorHiddenInGame(true);
			//SetActorEnableCollision(false);
			int randomX = FMath::FRandRange(0, 1000); //50 percent to drop item
			int randomY = FMath::FRandRange(0, 1000);
			SetActorLocation(FVector(randomX, randomY, -9000.f));
			SetActorTickEnabled(false);
			//////////////////////////////////////////////////////

			//If following and dies, follow Scavenger = nullptr (reset in plane below enabled)
			if (FollowScavenger)
			{
				FollowScavenger->GetCharacterMovement()->MaxWalkSpeed = FollowScavenger->fOriginalWalkSpeed;
				FollowScavenger = nullptr;
			}

			Deactivate();
			if (pCurrentWeapon)
			{
				pCurrentWeapon->Disable();
			}
			if (pCurrentShield)
			{
				pCurrentShield->Disable();
			}
		}

	}
}


void ABaseAICharacter::ResetShotInCover()
{
	// NOPE
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("ShotInCover", false);
		controller->GetBlackboardComponent()->SetValueAsBool("ShotInCover", true);
	}
}

// Called every frame
void ABaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FindNextTarget(); //??


	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		bool Healing = controller->GetBlackboardComponent()->GetValueAsBool("Healing");
		bool Reloading = controller->GetBlackboardComponent()->GetValueAsBool("Reloading");


		if (!Healing && !Reloading) //
		{
			IsAbleToFire_Code();
		}

		if (bStunned)
		{
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
		}

	}

	if (!bChasePlayers && controller)
	{
		UObject* mechanimsObject = controller->GetBlackboardComponent()->GetValueAsObject("TargetMechanism");
		AActor* mechanismActor = Cast<AActor>(mechanimsObject);

		if (bChaseEvent && bEventProceed) //new codeS
		{
			controller->GetBlackboardComponent()->SetValueAsBool("WithinRange", true); //dunno
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);

			bool bBossProtect = controller->GetBlackboardComponent()->GetValueAsBool("BossProtect");

			//if (!bShipPatroller && !bProtectingBoss) //ADDED bProtectingBoss - code below chases mechanism
			if (!bShipPatroller && !bBossProtect) //ADDED bProtectingBoss - code below chases mechanism
			{
				UAIBlueprintHelperLibrary::SimpleMoveToActor(controller, mechanismActor); //go after event //here or there
			}
			if (!bShipBoarded) //on our ship
			{
				bPlayerDetected = false; //comeback
			}
			bLookTowardsObjective = true;

			bStandStill = true;
		}
	}

	if (bShipBoarded)
	{
		if (bStandStill && !bShipPatroller) //boarded but doesnt move
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
	}

	if (!bChasePlayers && !bShipBoarded && controller) //If AI are on OUR ship / patrollers
	{
		UObject* target = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		AActor* targetActor = Cast<AActor>(target);

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), pAllPlayers);

		for (int i = 0; i < pAllPlayers.Num() - 1; i++)
		{
			if (targetActor)
			{
				float Distance = (targetActor->GetActorLocation() - GetActorLocation()).Size();
				bool Trigger = controller->GetBlackboardComponent()->GetValueAsBool("Triggered");
				if (Distance > fReturnToEventDistance && Trigger) //arbitrary value TODOS 3200
				{
					bProtectingBoss = false;
					controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true); //release player and make em chase events
				}
			}
		}
	}

	DetectPlayer();
	WithinRangeDip();


	if (bDipRange) //Set dip timer when attacked
	{
		float randomStart = FMath::RandRange(0.0f, 0.f);
		float randomDelay = FMath::RandRange(2.f, 2.f);

		GetWorldTimerManager().SetTimer(TimerHandle_DipTimer, this, &ABaseAICharacter::EnemyDipTimer, 2.f, true, randomStart);
		bDipRange = false;
	}

	if (CurrentMechanism && CurrentMechanism->GetIsActive() == true) //if red
	{
		GetRandomEvent();
	}

	//Protect Scavenger stuff!
	if (!bBossAttacker)
	{
		FindingScavengerClosest();
	}

	if (basePlayer && basePlayer->GetHealthComponent()->bIsDead && bReturnToRegular == false)
	{
		ReturnToRegularMode();
	}



	//SET FOCUS CODE - WHEN MELEE AI ATTACKS
	//if (bShipPatroller || bChasePlayers) //SET FOCUS OFF
	if (controller)
	{
		UObject* target = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		ABasePlayer* player = Cast<ABasePlayer>(target);

		bool RunAway2 = controller->GetBlackboardComponent()->GetValueAsBool("RunAway2");
		if (RunAway2 && !bStunned)
		{
			controller->SetFocus(player);
		}
		else
		{
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}

	//if (FollowScavenger == nullptr && bChasePlayers && controller)
	//{
	//	controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true); //??
	//}


	//IF SCAVENGER SUCCEEDS, Return this AI to regular triggers
	//if (scavenger && scavenger->bVaccineRetrieved && bReturnToRegular == false)
	//if (scavenger && scavenger->bVaccineRetrieved)
	//{
	//	ReturnToRegularMode();
	//	//bReturnToRegular = true;
	//	scavenger = nullptr;
	//}




	// FIND RANDOM DOOR CODE
	//if (bGetDoors && bDoorReset)
	//{
	//	FindRandomDoor();
	//	bDoorReset = false;
	//}
	//if (bGetDoors && CurrentDoor && bEnableDoorMovement)
	//{
	//	UAIBlueprintHelperLibrary::SimpleMoveToActor(controller, CurrentDoor); //go after green door
	//}

	/*if (CurrentDoor && CurrentDoor->GetLockedStatus() == true && !bReturnToBattle && bEndDoor)
	{
		bReturnToBattle = true;
		bEndDoor = false;
		bGetDoors = false;
		bEnableDoorMovement = false;

		ReturnToRegularMode();
	}*/

	/*if (controller && controller->GetBlackboardComponent()->GetValueAsBool("CanSeePlayer") && bGetDoors)
	{
		bEnableDoorMovement = false;

		FindNextTarget();
	}*/

	//if (controller)
	//{
	//	UObject* targetActor = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
	//	ABasePlayer* basePlayer = Cast<ABasePlayer>(targetActor);
	//	if (basePlayer && (basePlayer->GetIsPlayerDowned()) && bGetDoors && !bEnableDoorMovement)
	//	{
	//		//Return enemy to chasing doors when Player is downed
	//		bEnableDoorMovement = true;
	//	}
	//}
}


void ABaseAICharacter::UnstickAI()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
}

// Called to bind functionality to input
void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseAICharacter::FindNextTarget()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	fNearestTargetDistance = 10000000.f;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), pOutActors);

	for (int i = 0; i < pOutActors.Num(); i++)
	{
		UHealthComponent* healthComp = Cast<UHealthComponent>(pOutActors[i]->GetComponentByClass(UHealthComponent::StaticClass())); //if actor has a health component
		ABasePlayer* basePlayer2 = Cast<ABasePlayer>(pOutActors[i]);

		//if (basePlayer && (basePlayer->GetHealthComponent()->bIsDead == false))
		if (basePlayer2 && (basePlayer2->GetIsPlayerDowned() == false))
		{
			float Distance = (GetActorLocation() - pOutActors[i]->GetActorLocation()).Size();
			if (Distance < fNearestTargetDistance)
			{
				fNearestTargetDistance = Distance;
				pBestTargetActor = pOutActors[i];

				//if (Distance < 3200.f.f){ //if within distance, not working JUSTIN IMPORTANT ???
				if (Distance < 22200.f) //seen // change if ChasePlayer?
				{
					if (controller != nullptr) //R1 - A check to make sure that the controller isn't null
					{
						//controller->SetFocus(pBestTargetActor);
						controller->GetBlackboardComponent()->SetValueAsObject("TargetActor", pBestTargetActor); //TargetActor
					}
				}
				if (bChasePlayers) //dont limit distance if chase
				{
					if (controller != nullptr) //R1 - A check to make sure that the controller isn't null
					{
						//controller->SetFocus(pBestTargetActor);
						controller->GetBlackboardComponent()->SetValueAsObject("TargetActor", pBestTargetActor); //TargetActor
					}
				}
			}
		}
	}
}


void ABaseAICharacter::FindNextEvent()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	fNearestEventDistance = 100000000.f;

	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), outActors);

	bEventProceed = false;

	for (auto i : pGetAllEvents)
	{
		if (Cast<ABaseEvent>(i)->GetIsActive() == false) //if any green
		{
			bEventProceed = true; //if any green
		}
	}

	if (pGetAllEvents.Num() > 0 && bEventProceed)
	{

		for (int i = 0; i < pGetAllEvents.Num() - 1; i++)
		{
			float Distance = (GetActorLocation() - pGetAllEvents[i]->GetActorLocation()).Size();
			if (Distance < fNearestEventDistance)
			{
				fNearestEventDistance = Distance;
				pBestEventActor = pGetAllEvents[i]; //BACKTRACK

				CurrentMechanism = Cast<ABaseEvent>(pBestEventActor);
				//controller->GetBlackboardComponent()->SetValueAsObject("TargetMechanism", pBestEventActor); //TargetActor

				if (CurrentMechanism && CurrentMechanism->GetIsActive() && controller)
				{
					controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);
					GetWorldTimerManager().SetTimer(TimerHandle_ResetAIFind, this, &ABaseAICharacter::FindNextEvent, 0.1f, true);
				}

				if (CurrentMechanism && CurrentMechanism->GetIsActive() == false && controller)
				{
					GetWorldTimerManager().ClearTimer(TimerHandle_ResetAIFind);

					UObject* mechanismObject = Cast<UObject>(CurrentMechanism);
					controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);
					controller->GetBlackboardComponent()->SetValueAsObject("TargetMechanism", mechanismObject);
				}
			}

			if (!bEventProceed && !bShipPatroller)
			{
				AfterEventsAreGone();
			}
		}
	}
}


void ABaseAICharacter::IsAbleToFire_Code()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller && !bDied) //important
	{
		bool CanSeePlayer = controller->GetBlackboardComponent()->GetValueAsBool("CanSeePlayer");
		UObject* TargetActor = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");

		{
			AActor* target = Cast<AActor>(TargetActor);

			if (target)
			{
				DistancePlayer = (target->GetActorLocation() - GetActorLocation()).Size();

				if (DistancePlayer <= fFireDistance && !bDontFire) //done inside gamemode when all players are dead
				{

					RaycastEnd = target->GetActorLocation() + FVector(0.f, 0.f, 45.f);
					RaycastBegin = GetActorLocation() + FVector(0.f, 0.f, 70.f) + (GetActorForwardVector() * 45);

					if (bIsCrouching)
					{
						RaycastBegin = GetActorLocation() + FVector(0.f, 0.f, 15.f) + (GetActorForwardVector() * 45);
					}

					//DrawDebugSphere(GetWorld(), RaycastBegin, 6.f, 12, FColor::Green, true, 0.1f);
					//DrawDebugSphere(GetWorld(), RaycastEnd, 6.f, 12, FColor::Red, true, 0.1f);

					//More matt temp code
					TArray<FHitResult> Hits;

					if (pRaycastComponent->RaycastMulti(Hits, RaycastBegin, RaycastEnd, ECollisionChannel::ECC_MAX))
					{
						FHitResult* hit = nullptr;

						for (auto h : Hits)
						{
							if (h.GetActor() && h.GetActor()->IsA<ABaseAICharacter>()) continue;
							if (h.GetActor() && h.GetActor()->IsA<AEnemyShield>()) continue;
							if (h.GetActor() && h.GetActor()->IsA<ABasePlayer>()) { hit = &h; break; }
						}
						if (hit != nullptr)
						{
							ABasePlayer* basePlayer3 = Cast<ABasePlayer>(hit->GetActor()); //COMEBACK

							//if (basePlayer && basePlayer->GetHealthComponent()->bIsDead == false)
							if (basePlayer3 && basePlayer3->GetIsPlayerDowned() == false)
							{
								bPlayerDown = false;

								FindNextTarget(); //double

								if (bMeleeAttacker && DistancePlayer < 180 && !bBossAttacker && !bShieldEnemy) //melee attacks with close range (180)
								{
									Fire();
									//controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
								}
								if (bShieldEnemy && DistancePlayer < 203 && !bBossAttacker)
								{
									Fire();
								}
								if (!bMeleeAttacker && !bBossAttacker) //range attacks
								{
									Fire();
								}
								if (bBossAttacker && DistancePlayer > 180)
								{
									Fire();
								}
							}

							//if (basePlayer && basePlayer->GetHealthComponent()->bIsDead) //Player dead
							if (basePlayer3 && basePlayer3->GetIsPlayerDowned())
							{
								//AI On our ship! 
								if (!bShipBoarded && !bPlayerDown)
								{
									bPlayerDown = true;
									GetRandomEvent();
									bChaseEvent = true;

								}



								GetWorldTimerManager().ClearTimer(TimerHandle_DipTimer); //turn off when player dies

								if (bShipBoarded)
								{
									// Clear focus
									GetWorldTimerManager().ClearTimer(TimerHandle_DipTimer); //turn off when player dies //TODO!!!
									bStandStill = true; //if ship boarded without being patroller
								}


								if (bShipPatroller && bShipBoarded) //if target dead, keep investigating
								{
									controller->GetBlackboardComponent()->SetValueAsBool("Investigating", true);
								}
								if (bShipPatroller && !bShipBoarded) //if target dead, keep investigating
								{
									controller->GetBlackboardComponent()->SetValueAsBool("Securing", true);
								}
							}
						}
					}
				}
			}

		}
	}

}


void ABaseAICharacter::HealSelfInitialize()
{
	//BOSS CODE INHERITS
}

void ABaseAICharacter::HealSelfTick()
{
	//BOSS CODE INHERITS
}


void ABaseAICharacter::DetectPlayer() //if player is within line of sight / Inrange of next target
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		UObject* object = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		if (object)
		{
			ABasePlayer* playerActor = Cast<ABasePlayer>(object);


			float Distance = (playerActor->GetActorLocation() - GetActorLocation()).Size();

			if (Distance <= fMovementDistance)
			{
				if (playerActor)
				{
					FHitResult hit;
					if (pRaycastComponent->RaycastSingle(hit, GetActorLocation(), playerActor->GetActorLocation(), ECollisionChannel::ECC_MAX))
					{
						if (hit.GetActor() != nullptr && hit.GetActor()->IsA<ABasePlayer>())
						{
							bPlayerDetected = true; //COMEBACK MAY NOT WORK!!!!!!!! hide if you have too
						}
						else
						{
							bPlayerDetected = false;
						}
					}

				}
				else
				{
					bPlayerDetected = false; //Comeback

				}
			}

		}
	}
}

void ABaseAICharacter::ChangeDirectionIncrement()
{
}

void ABaseAICharacter::EnemyDipTimer()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller && !bMeleeAttacker && !bBossAttacker) //change? Boss attacker TODO
	{
		controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
	}
}
void ABaseAICharacter::ChangeDip()
{
}

void ABaseAICharacter::WithinRangeDip() //within dipping range - cancels when outside
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller && !bDied) //important
	{
		UObject* TargetActor = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");

		AActor* target = Cast<AActor>(TargetActor);

		if (target)
		{
			float Distance = (target->GetActorLocation() - GetActorLocation()).Size();

			if (Distance <= 2000 && !bDontFire)
			{

				bWithinRange = true; //use for FindNextTarget() too!

			}
			else
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_DipTimer);
				bWithinRange = false;
			}
		}

	}
}

void ABaseAICharacter::Activate()
{
	Super::Activate();

	if (GetLocalRole() == ROLE_Authority)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}


void ABaseAICharacter::Deactivate()
{
	Super::Deactivate();
}

void ABaseAICharacter::Reset()
{
	Super::Reset();
	//pHealthComp->Health = pHealthComp->DefaultHealth;
}

void ABaseAICharacter::EnableTest() //Not important - meant for testing Enable or Disable after set timer
{
	Activate();
	Reset();
}

class URaycastComponent* ABaseAICharacter::GetRaycastComponent()
{
	return pRaycastComponent;
}

void ABaseAICharacter::StandupPeriod()
{
	//Set bool?
	bStandupPeriod = false;

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);
	controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);


	//DUNNO BELOW
	bIsCrouching = false;
	UnCrouch();
}

void ABaseAICharacter::ReturnToRegularMode()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if ((bBehindAI || bWithinBossRange || bReturnToRegular == false) && controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("ProtectScavenger", false);
		controller->GetBlackboardComponent()->SetValueAsBool("BossProtect", false);
		controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);

		bBehindAI = false;
		bWithinBossRange = false;
		bReturnToRegular = true; //ShieldAI from protecting Scavenger

		bLookTowardsObjective = false;
		bStandStill = false;

		if (!bShipBoarded)
		{
			bChaseEvent = true;
		}

		// Sets patrolling ai to investigating at startup (when boarded their ship)
		if (bShipPatroller && bShipBoarded && controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("Investigating", true);
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
			GetCharacterMovement()->bOrientRotationToMovement = true; //TODOS

		}
		if (bChasePlayers && !bShipBoarded)
		{
			//Chase Players
			//bChasePlayers = true;
			//bShipBoarded = false;
			//bShipPatroller = false;

			if (controller)
			{
				FindNextTarget();
				controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
				controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
				//controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true); //Meant for MeleeAI
			}
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
			GetCharacterMovement()->bOrientRotationToMovement = true; //TODOS
		}

		if (!bShipPatroller && !bChasePlayers && !bShipBoarded)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", false);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", true);
		}

		if (bShipPatroller && !bShipBoarded && controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("Securing", true);
			controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
			GetCharacterMovement()->bOrientRotationToMovement = true; //TODOS

		}

		GetRandomEvent();

		GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;

		UnCrouch();
		bIsCrouching = false;

		bProtectingBoss = false;
	}
}


void ABaseAICharacter::FindingScavengerClosest()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller == nullptr) //DONT let boss help scavengers
	{
		return;
	}

	bool RunAway2 = controller->GetBlackboardComponent()->GetValueAsBool("RunAway2");
	bool BossProtect = controller->GetBlackboardComponent()->GetValueAsBool("BossProtect");

	UObject* TargetActor = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
	ABasePlayer* Player = Cast<ABasePlayer>(TargetActor);
	{
		fNearestScavengerDistance = 10000000.f;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AScavengerAI::StaticClass(), pOutScavengers);

		for (int i = 0; i < pOutScavengers.Num(); i++)
		{
			UHealthComponent* healthComp = Cast<UHealthComponent>(pOutScavengers[i]->GetComponentByClass(UHealthComponent::StaticClass())); //if actor has a health component
			scavenger = Cast<AScavengerAI>(pOutScavengers[i]);

			//if (scavenger && (scavenger->bDisabled == false))
			{
				float Distance = (GetActorLocation() - pOutScavengers[i]->GetActorLocation()).Size();
				if (Distance < fNearestScavengerDistance)
				{
					fNearestScavengerDistance = Distance;
					pBestScavengerActor = pOutScavengers[i];
					scavengerAI = Cast<AScavengerAI>(pBestScavengerActor);

					if (Distance <= 2000.f && scavengerAI && !scavengerAI->bTaken && !bChasingPlayerScavenger) //distance scavenger
					{
						scavengerAI->bTaken = true;
						FollowScavenger = scavengerAI;
					}

					if (FollowScavenger && FollowScavenger->bScavengerHidden)
					{
						controller->GetBlackboardComponent()->SetValueAsBool("ProtectScavenger", false);							
						GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
						//GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed; IN ReturnToRegularMode();
						ReturnToRegularMode();
						bChasingPlayerScavenger = false;
						FollowScavenger = nullptr;
					}


					if (FollowScavenger && scavengerAI && scavengerAI->bTaken && controller != nullptr && !BossProtect) //if protecting boss, dont protect scavenger
					{
						FollowScavenger->GetCharacterMovement()->MaxWalkSpeed = 500.f;

						FVector scavengerVector = FollowScavenger->GetActorLocation();

						controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
						controller->GetBlackboardComponent()->SetValueAsBool("Securing", false);
		
						controller->ClearFocus(EAIFocusPriority::Gameplay);
						controller->ClearFocus(EAIFocusPriority::LastFocusPriority);

						bReturnToRegular = false;
						//UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, scavengerVector);
						controller->MoveToLocation(scavengerVector);

						//FollowScavenger->GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
						//GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;

						if (FollowScavenger->bVaccineRetrieved)
						{
							float scavengerSlowSpeed = FollowScavenger->fOriginalWalkSpeed * 0.32f;
							FollowScavenger->GetCharacterMovement()->MaxWalkSpeed = scavengerSlowSpeed;
							GetCharacterMovement()->MaxWalkSpeed = scavengerSlowSpeed;
						}
					}

				
					

					if (DistancePlayer < 2000.f && controller && !RunAway2) //distance player ONLY works on MeleeAI
					{
						if (Player && Player->GetIsPlayerDowned() == false)
						{
							//controller->ClearFocus(EAIFocusPriority::LastFocusPriority);
							bChasingPlayerScavenger = true;
							//UAIBlueprintHelperLibrary::SimpleMoveToActor(controller, Player);

							GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;

							controller->GetBlackboardComponent()->SetValueAsBool("ProtectScavenger", false);
							//ReturnToRegularMode();

							controller->GetBlackboardComponent()->SetValueAsBool("ChasePlayer", true);


							if (FollowScavenger)
							{
								FollowScavenger->GetCharacterMovement()->MaxWalkSpeed = FollowScavenger->fOriginalWalkSpeed;
								FollowScavenger = nullptr;

							}
						}
						if (Player && Player->GetIsPlayerDowned())
						{
							
						}
					}

					if (DistancePlayer > 2000.f && scavengerAI && scavengerAI->bVaccineRetrieved == false)
					{
						//scavengerAI->GetCharacterMovement()->MaxWalkSpeed = fAIMovementSpeed;
					}
				}
			}
		}

	}
	if (Player && Player->GetIsPlayerDowned())
	{
		bChasingPlayerScavenger = false;
		if (scavengerAI)
		{
			scavengerAI->bTaken = false;
		}
		ReturnToRegularMode();
		Player = nullptr;
	}

}

void ABaseAICharacter::FindRandomDoor()
{
	//bChaseEvent = false;

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Door", pGetAllDoors);

	bDoorProceed = false;

	for (auto i : pGetAllDoors)
	{
		if (Cast<AShipDoor>(i)->GetLockedStatus() == false) //if any green
		{
			bDoorProceed = true; //if any green
		}
	}

	if (pGetAllDoors.Num() > 0 && bDoorProceed)
	{

		//int eventRand = FMath::RandRange(0, FMath::Max(pGetAllEvents.Num() - 1, 0)); //Num need -1?
		int doorRand = FMath::RandRange(0, pGetAllDoors.Num() - 1); //Num need -1?

			//ABaseEvent* mechanism = Cast<ABaseEvent>(pGetAllEvents[eventRand]);
		CurrentDoor = Cast<AShipDoor>(pGetAllDoors[doorRand]);

		if (CurrentDoor && CurrentDoor->GetLockedStatus() && controller) //active means broken. red
		{
			GetWorldTimerManager().SetTimer(TimerHandle_DoorReset, this, &ABaseAICharacter::FindRandomDoor, 0.1f, true); //checking without tick
		}

		if (CurrentDoor && CurrentDoor->GetLockedStatus() == false && controller) //green
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_DoorReset);
			bEndDoor = true;
		}
	}

	if (!bEventProceed && !bShipPatroller) //if ALL doors become red
	{
	}
}

void ABaseAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAICharacter, bIsCrouching);
}