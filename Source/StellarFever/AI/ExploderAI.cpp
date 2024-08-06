// Copyright Quarantine Games 2020


#include "ExploderAI.h"
#include "Components/StaticMeshComponent.h"
#include "..\Components/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Materials/Material.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AIController.h"
#include "..\Player/BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "BaseAICharacter.h"
#include "BaseAICharacter_AIController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "ExploderAI_AIController.h"
#include "PatrolPath.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Components/DecalComponent.h"
#include "../StellarFeverGameModeBase.h"

// Sets default values
AExploderAI::AExploderAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pHealthComp->DefaultHealth = 1.f;
	//pHealthComp->DefaultHealth *= fHealthIncrease;

	fExplosionRadius = 300.f;

	pSphereExplosion = CreateDefaultSubobject<USphereComponent>(TEXT("SphereExplode"));
	pSphereExplosion->SetSphereRadius(fExplosionRadius);
	pSphereExplosion->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pSphereExplosion->SetCollisionProfileName("OverlapAll");
	pSphereExplosion->SetVisibility(true);
	pSphereExplosion->bHiddenInGame = true;
	pSphereExplosion->SetupAttachment(RootComponent);

	pSphereInstigate = CreateDefaultSubobject<USphereComponent>(TEXT("SphereInstigate"));
	pSphereInstigate->SetSphereRadius(fExplosionRadius / 3);
	pSphereInstigate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pSphereInstigate->SetCollisionProfileName("OverlapAll");
	pSphereInstigate->SetVisibility(true);
	pSphereInstigate->bHiddenInGame = true;
	pSphereInstigate->SetupAttachment(RootComponent);

	pBallAudioComp = CreateDefaultSubobject<UAudioComponent>("BallRollingSound");
	pBallAudioComp->bAutoActivate = false;
	pBallAudioComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	pBallAudioComp->SetupAttachment(RootComponent);
	//pBallAudioComp->SetIsReplicated(true);

	//pPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovementComp"));

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AExploderAI::OnPawnSeen);

	fAIMovementSpeed = 1000.0;
	fOriginalWalkSpeed = fAIMovementSpeed; //meant for Stunned reset

	InitialSpeed = fAIMovementSpeed;

	fRequiredDistanceToTarget = 100.f;

	fExplosionDamage = 50;
	InitialDamage = fExplosionDamage;

	bStartedSelfDestruction = false;
	fSelfDamageInterval = 1.f;
	pBestTargetActor = nullptr;
	LockedPlayer = nullptr;

	bExploder = true;

	//SetReplicates(true);
	//SetReplicateMovement(true);
}

// Called when the game startss or when spawned
void AExploderAI::BeginPlay()
{
	Super::BeginPlay();
	
	//pHealthComp->OnHealthChanged.AddDynamic(this, &AExploderAI::OnHealthChanged);

	DeactivateSpot = GetActorLocation();

	GetCharacterMovement()->MaxWalkSpeed = fAIMovementSpeed;


	AExploderAI_AIController* controller = Cast<AExploderAI_AIController>(GetController());
	if (controller)
	{
		eStatus = Status::Patrolling;
		controller->GetBlackboardComponent()->SetValueAsBool("SecuringExploder", true); //PATROL
	}

	//ALWAY PATROLS so set always!
	//patrol_path = Cast<APatrolPath>(SecurePaths[0]);

	pBallAudioComp->SetSound(pBallRollSound);

	//InitialDamage = fExplosionDamage;
}

void AExploderAI::OnPawnSeen(APawn* SeenPawn)
{

	ABasePlayer* TargetActor2 = Cast<ABasePlayer>(SeenPawn);
	if (TargetActor2)
	{
		AExploderAI_AIController* controller = Cast<AExploderAI_AIController>(GetController());

		//FindNextTarget();s
		controller->GetBlackboardComponent()->SetValueAsBool("AttackMode", true);
	}
}

void AExploderAI::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType2, AController* InstigatedBy, AActor* DamageCauser)
{

	MulticastExplode();

	if (Health <= 0.0f)
	{
		SelfDestruct();
		//ForceNetUpdate(); //USE OFTEN! Lower frequency on ALL AIS
	}
}

void AExploderAI::MulticastExplode_Implementation()
{
	if (pMatInst == nullptr)
	{
		pMatInst = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GetMesh()->GetMaterial(0));
	}

	if (pMatInst)
	{
		pMatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
}
bool AExploderAI::MulticastExplode_Validate()
{
	return true;
}

void AExploderAI::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	MulticastSelfDestruct();

	if (pSphereExplosion != nullptr)
	{
		TArray<AActor*> CollectedActors;
		pSphereExplosion->GetOverlappingActors(CollectedActors);

		for (auto& actor : CollectedActors)
		{
			//if (actor->ActorHasTag(FName("Pickup")))
			ABasePlayer* player = Cast<ABasePlayer>(actor);
			if (Cast<ABasePlayer>(player))
			{
				UGameplayStatics::ApplyDamage(actor, fExplosionDamage, GetController(), this, nullptr);
			}

			//if (actor->ActorHasTag(FName("Pickup")))
			AAIEnemy* enemy = Cast<AAIEnemy>(actor);
			if (Cast<AAIEnemy>(enemy))
			{
				UGameplayStatics::ApplyDamage(actor, fExplosionDamage, GetController(), this, nullptr);
			}
		}
	}

	bDied = true;

	GetWorldTimerManager().SetTimer(TimerHandle_DisableTimer, this, &AExploderAI::Deactivate, 0.06f, false);
}

void AExploderAI::MulticastSelfDestruct_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, pExplodeSound, GetActorLocation());

	pNiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pExplosionNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(6.f, 6.f, .6f));

}
bool AExploderAI::MulticastSelfDestruct_Validate()
{
	return true;
}

void AExploderAI::DamageSelf()
{
	float DamageAmount = pHealthComp->DefaultHealth / 3;
	if (!bStunned)
		UGameplayStatics::ApplyDamage(this, DamageAmount, GetInstigatorController(), this, nullptr);

	if ((pHealthComp->GetHealth() / pHealthComp->DefaultHealth) < 0.5f)
	{
		eStatus = Status::Exploding;
	}
}

// Called every frame
void AExploderAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FindNextTarget();

	if (LockedPlayer && LockedPlayer->GetIsPlayerDowned())
	{
		LockedPlayer = nullptr;
	}

	AExploderAI_AIController* controller = Cast<AExploderAI_AIController>(GetController());

	if (controller && !bDisabled && controller->GetBlackboardComponent()->GetValueAsBool("AttackMode"))
	{
		if (pBestTargetActor) //new? need?
		{
			FVector TargetActorVector = pBestTargetActor->GetActorLocation();
			controller->GetBlackboardComponent()->SetValueAsVector("TargetActorVector", TargetActorVector);


			if (LockedPlayer != nullptr)
			{
				controller->MoveToActor(LockedPlayer);
			}
			else
			{
				controller->GetBlackboardComponent()->SetValueAsBool("SecuringExploder", true);
			}


			if (eStatus != Status::Exploding)
			{
				eStatus = Status::Chasing; //Set to chasing if not exploding
			}

		}
	}

	// IF runs into player, activate!
	if (pSphereInstigate != nullptr && !bStunned)
	{
		TArray<AActor*> CollectedActors2;
		pSphereInstigate->GetOverlappingActors(CollectedActors2);

		for (auto& actor : CollectedActors2)
		{
			ABasePlayer* player = Cast<ABasePlayer>(actor);
			if (!bStartedSelfDestruction && !bExploded && player && bDisabled == false) //bDisable meant for AI enable/disable
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AExploderAI::DamageSelf, fSelfDamageInterval, true, 0.0f);

				bStartedSelfDestruction = true;
				bDisabled = true;

				FVector position = GetActorLocation();
				position.Z -= 25.f;
				UGameplayStatics::SpawnDecalAttached(pGreenRadius, FVector(50, 530, 530), GetMesh(), "", position, FRotator(-90, 0.f, -90), EAttachLocation::KeepWorldPosition, 1.f);
				UGameplayStatics::SpawnSoundAttached(pSelfDestructSound, RootComponent);

				if (controller)
				{
					controller->GetBlackboardComponent()->SetValueAsBool("AttackMode", true);
				}

			}
		}
	}

	if (GetCharacterMovement()->Velocity.Size() > 0 && bBallRoll == false)
	{
		pBallAudioComp->Play();
		bBallRoll = true;
	}
	if (GetCharacterMovement()->Velocity.Size() == 0 && bBallRoll == true)
	{
		pBallAudioComp->Stop();
		bBallRoll = false;
	}

	if (bShipBoarded && controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("SecuringExploder", false);
	}
	if (bChasePlayers && controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("AttackMode", true);
	}

	if (bDisabled)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}



void AExploderAI::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void AExploderAI::Reset()
{
	Super::Reset();

	SetActorTickEnabled(true);
	bStartedSelfDestruction = false;
	bExploded = false;
	bDisabled = false;
}

void AExploderAI::FindNextTarget()
{
	fNearestTargetDistance = 10000000.f;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), pOutActors);

	for (int i = 0; i < pOutActors.Num(); i++)
	{
		//UHealthComponent* healthComp = Cast<UHealthComponent>(pOutActors[i]->GetComponentByClass(UHealthComponent::StaticClass())); //if actor has a health component
		ABasePlayer* basePlayer = Cast<ABasePlayer>(pOutActors[i]);

		//if (basePlayer && basePlayer->GetHealthComponent()->bIsDead == false)
		if (basePlayer && (basePlayer->GetIsPlayerDowned() == false))
		{
			float Distance = (GetActorLocation() - pOutActors[i]->GetActorLocation()).Size();
			if (Distance < fNearestTargetDistance)
			{
				fNearestTargetDistance = Distance;
				pBestTargetActor = pOutActors[i];

				LockedPlayer = Cast<ABasePlayer>(pBestTargetActor);


				//if (Distance < 2000.f) { //if within distance, not working JUSTIN IMPORTANT
				//	controller->GetBlackboardComponent()->SetValueAsObject("TargetActor", pBestTargetActor); //TargetActor
				//}
			}
		}
	}
	
}

// Called to bind functionality to input
void AExploderAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AExploderAI::Activate()
{
	Super::Activate();

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("SecuringExploder", true);
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_SelfDamage);

	//bExploded = false;

	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		fExplosionDamage = InitialDamage + (1.6f * pGameMode->GetDifficulyValue());
	}
}

void AExploderAI::SurviveBeginning()
{

}

void AExploderAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExploderAI, pMatInst);
	DOREPLIFETIME(AExploderAI, pBallAudioComp);
}




