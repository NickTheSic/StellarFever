// Copyright Quarantine Games 2020


#include "BossAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "..\Components/HealthComponent.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyMeleeWeapon.h"
#include "../StellarFeverGameModeBase.h"
#include "FragGrenadeAI.h"
#include "..\Player\\BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeneratedCodeHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Templates/Casts.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "FragGrenadeAI.h"

ABossAI::ABossAI(FObjectInitializer const& object_initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	pHealthComp->DefaultHealth = 390.f;
	InitialHealth = pHealthComp->DefaultHealth;
	fBelowHealthRun = 100.f;

	ProtectInfrontPosition = CreateDefaultSubobject<USceneComponent>("FollowPoint");
	ProtectInfrontPosition->SetupAttachment(GetRootComponent());
	ProtectInfrontPosition->ComponentTags.Add(FName("ProtectInfrontPosition"));

	bBossAttacker = true;

	DistancePlayer = 1000000.f;

	fAttackRangeDistance = 1100.f;

	fAIMovementSpeed = 700;
	InitialSpeed = fAIMovementSpeed;

	//Gravity = 420.000000f;
	//Velocity = 1620.0;

	Tags.Add("BossAI");
	Tags.Add("BossRange2");
}

void ABossAI::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	pBossMeleeWeapon = GetWorld()->SpawnActor<AEnemyMeleeWeapon>(StarterMeleeClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (pBossMeleeWeapon)
	{
		pBossMeleeWeapon->SetOwner(this);
		pBossMeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "MeleeHitPoint");
	}

	//bChasePlayers = true;

	//Get GameMode
	AStellarFeverGameModeBase* GameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		//AttackRangeDistance *= LevelDifficulty * fRangeIncrease; //Gamemode level difficulty
	}

	GetCharacterMovement()->MaxWalkSpeed = fAIMovementSpeed;
}

void ABossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	//if (HealthComp->GetHealth() <= BelowHealthRun && IsHealing == false)
	if (pHealthComp->GetHealth() <= fBelowHealthRun && bRunToHeal == false && bResetHealActive == false && controller)
	{
		bResetHealActive = true;
		bRunToHeal = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ResetHealActive, this, &ABossAI::ResetHealActive, 30.f, false);

		controller->GetBlackboardComponent()->SetValueAsBool("HealRun", true);
	}

	if (DistancePlayer < 180) //if within close range, attack with melee
	{
		AttackMelee();
	}



	//STRAFE TICK CODE
	if (DistancePlayer < 1500 && bStrafeAway == false)
	{
		bStrafeAway = true;
		GetWorldTimerManager().SetTimer(TimerHandle_Strafe, this, &ABossAI::StrafeFunction, .82f, true);

	}
	if (DistancePlayer > 1500 && bStrafeAway == true)
	{
		bStrafeAway = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_Strafe);
	}


	//GRENADE LAUNCHING
	if (DistancePlayer < 1820 && bGrenadePlay == false) //if within close range, attack with melee
	{
		bGrenadePlay = true;
		
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnGrenade, this, &ABossAI::ShotGrenade, 6.f, true);
	}
	if (DistancePlayer > 2000)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_SpawnGrenade);
		bGrenadePlay = false;
	}

	if (bRunToHeal)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_SpawnGrenade);
	}

	if (controller)
	{
		UObject* TargetActor = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		ABasePlayer* target = Cast<ABasePlayer>(TargetActor);
		if (target && target->GetHealthComponent()->bIsDead)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_SpawnGrenade);
		}
	}
}

void ABossAI::HealSelfInitialize()
{
	iHealIndex = 0;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	UHealthComponent* healthComp = Cast<UHealthComponent>(GetComponentByClass(UHealthComponent::StaticClass()));
	if (healthComp)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HealSelf, this, &ABossAI::HealSelfTick, fHealTickInterval, true);
	}
}

void ABossAI::HealSelfTick()
{
	if (bResetHealActive == false)
	{
		return;
	}

	UHealthComponent* healthComp = Cast<UHealthComponent>(GetComponentByClass(UHealthComponent::StaticClass()));
	if (healthComp)
	{
		iHealIndex++;
		healthComp->Heal(fHealPerTick);
	}
	if (iHealIndex >= 5)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_HealSelf);
		GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
		//bHealing2 = false;

		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
		//controller->GetBlackboardComponent()->SetValueAsBool("HealRun", false);
		iHealIndex = 0;

		bRunToHeal = false;
		bGrenadePlay = false;

		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
			FindNextTarget();
		}
	}
}

void ABossAI::ResetHealActive()
{
	bResetHealActive = false;
}

void ABossAI::AttackMelee()
{
	if (!bStunned)
	{
		if (pBossMeleeWeapon)
		{
			pBossMeleeWeapon->FireSingleAI();
		}
	}
}

void ABossAI::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
{
	Super::OnHealthChangedInternal(InHealthComp, Health, HealthDelta, DamageType2, InstigatedBy, DamageCauser);

}

void ABossAI::StrafeFunction()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");

	if (HealRun || bStunned)
	{
		return;
	}

	if (controller && DistancePlayer < 1500)
	{
		//controller->GetBlackboardComponent()->S etValueAsBool("RunAway", true);
		FVector velocity = (GetActorRightVector());
		velocity.Z += .17;
		velocity.Normalize();

		int random = FMath::RandRange(0, 1);
		if (random == 0) //left
		{
			LaunchCharacter(velocity * 1000, false, true);
		}
		if (random == 1) //right
		{
			LaunchCharacter(-velocity * 1000, false, true);
		}
	}
}

void ABossAI::Activate()
{
	Super::Activate();

	//Chase Players ALWAYS
	bChasePlayers = true;
	bShipBoarded = false;
	bShipPatroller = false;

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
		controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
		FindNextTarget();
	}
}

void ABossAI::ShotGrenade()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}
	if (bStunned)
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
	AFragGrenadeAI* FragGrenade = GetWorld()->SpawnActor<AFragGrenadeAI>(FragGrenadeClass, position, forwardRotation, SpawnParams);

}