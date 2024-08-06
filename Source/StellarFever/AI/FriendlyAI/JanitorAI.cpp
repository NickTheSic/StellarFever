// Copyright Quarantine Games 2020


#include "JanitorAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "..\PatrolPath.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"
#include "JanitorAI_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AJanitorAI::AJanitorAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pBallAudioComp = CreateDefaultSubobject<UAudioComponent>("BallRollingSound");
	pBallAudioComp->bAutoActivate = true;
	pBallAudioComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	pBallAudioComp->SetupAttachment(RootComponent);
	pBallAudioComp->SetIsReplicated(true);

	bPickedUp = false;
	bBallRoll = false;

	fMovementSpeed = 320.0f;
	GetCharacterMovement()->MaxWalkSpeed = fMovementSpeed;

	bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);

		NetUpdateFrequency = 10; //FRICKIN IMPORTANT!

}

// Called when the game starts or when spawned
void AJanitorAI::BeginPlay()
{
	Super::BeginPlay();



	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SecurePathJanitor", SecurePaths);
	if (SecurePaths.IsValidIndex(0))
	{
		patrol_path = Cast<APatrolPath>(SecurePaths[0]);
	}

	if (pBallRollSound)
	{
		pBallAudioComp->SetSound(pBallRollSound);
		pBallAudioComp->Play();
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;


	AIControllerClass = AJanitorAI_AIController::StaticClass();

	if (AIControllerClass != NULL && GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags |= RF_Transient;    // We never want to save AI controllers into a map
		AJanitorAI_AIController* NewController = GetWorld()->SpawnActor<AJanitorAI_AIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (NewController != NULL)
		{
			// if successful will result in setting this->Controller 
			// as part of possession mechanics
			NewController->Possess(this);
		}
	}

	pController = Cast<AJanitorAI_AIController>(GetController());

	if (pController)
	{
		pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", true);
	}

	//FTimerHandle TimerHandle_Test1;
	//GetWorldTimerManager().SetTimer(TimerHandle_Test1, this, &AJanitorAI::Disable, 5.f, false);
	//FTimerHandle TimerHandle_Test2;
	//GetWorldTimerManager().SetTimer(TimerHandle_Test2, this, &AJanitorAI::Enable, 10.f, false);

}

// Called every frame
void AJanitorAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (SecurePaths.Num() > 0)
	//if (GetLocalRole() == ROLE_Authority)
	{
		///ServerSound();
	}

	if (GetActorLocation().Z < -5000)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "JANITOR BELOW -5000.z");
	}

	//GetMesh()->SetCustomDepthStencilValue(1);

}

void AJanitorAI::ServerSound()
{
	if (GetCharacterMovement()->Velocity.Size() == 0)
	{
		pBallAudioComp->SetPaused(true);
		bBallRoll = false;
	}
	if (GetCharacterMovement()->Velocity.Size() > 0)
	{
		pBallAudioComp->SetPaused(false);
		bBallRoll = true;
	}

	if (bPickedUp)
	{
		pBallAudioComp->SetPaused(true);
		SetActorLocation(FVector(32.0f, 32.0f, -9000.f));
	}

	//pBallAudioComp->Stop();

}

APatrolPath* AJanitorAI::get_patrol_path()
{
	//if (patrol_path != nullptr)
	return patrol_path;
}



void AJanitorAI::Activate()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	//controller = Cast<AJanitorAI_AIController>(GetController());
	if (pController)
	{
		pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", false);
		pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", true);
	}
}

void AJanitorAI::Deactivate()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

//void AJanitorAI::Enable()
//{
//	bDisabled = false;
//
//	//Remove? Keep? Dont know yet
//	if (pController)
//	{
//		pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", false);
//		pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", true);
//	}
//}
//
//void AJanitorAI::Disable()
//{
//	bDisabled = true;
//}

// Called to bind functionality to input
void AJanitorAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJanitorAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME(AJanitorAI, pMesh);
	DOREPLIFETIME(AJanitorAI, BehaviorTree);
	DOREPLIFETIME(AJanitorAI, pBallAudioComp);
	DOREPLIFETIME(AJanitorAI, bPickedUp);
	DOREPLIFETIME(AJanitorAI, bBallRoll);
	DOREPLIFETIME(AJanitorAI, pBallRollSound);
	DOREPLIFETIME(AJanitorAI, patrol_path);
	DOREPLIFETIME(AJanitorAI, BlackboardComponent);
	DOREPLIFETIME(AJanitorAI, pController);*/
}