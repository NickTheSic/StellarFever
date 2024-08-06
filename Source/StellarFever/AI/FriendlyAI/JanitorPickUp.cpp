// Copyright Quarantine Games 2020


#include "JanitorPickUp.h"
#include "JanitorAI.h"
#include "JanitorAI_AIController.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\Player/BasePlayer.h"
#include "Net/UnrealNetwork.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

AJanitorPickUp::AJanitorPickUp()
{
	PrimaryActorTick.bCanEverTick = true;

	pMesh2 = CreateDefaultSubobject<UStaticMeshComponent>("Mesh2");
	//pMesh2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pMesh2->SetCollisionProfileName("NoCollision");
	pMesh2->SetupAttachment(RootComponent);

	pPickupComponent = CreateDefaultSubobject<UCapsuleComponent>("PickupComponent");
	pPickupComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pPickupComponent->SetCollisionProfileName("JanitorPickup");
	pPickupComponent->SetupAttachment(RootComponent);

	pCollider = CreateDefaultSubobject<UBoxComponent>("Collider");
	pCollider->SetupAttachment(RootComponent);
	pCollider->bHiddenInGame = false;
	pCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	pCollider->SetCollisionProfileName("OverlapAllDynamic");
	pCollider->SetIsReplicated(true);


	bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	NetUpdateFrequency = 10; //FRICKIN IMPORTANT!


	Tags.Add("Janitor");
}

void AJanitorPickUp::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;

		pJanitor = GetWorld()->SpawnActor<AJanitorAI>(pJanitorClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
		if (pJanitor)
		{
			pJanitor->Activate();
		}
	}

	pMesh2->SetVisibility(false);
	pPickupComponent->SetSimulatePhysics(false);


	//FTimerHandle TimerHandle_Test1;
	//GetWorldTimerManager().SetTimer(TimerHandle_Test1, this, &AJanitorPickUp::Disable, 5.f, false);
	//FTimerHandle TimerHandle_Test2;
	//GetWorldTimerManager().SetTimer(TimerHandle_Test2, this, &AJanitorPickUp::Enable, 10.f, false);
	//SetActorTickEnabled(true);
}

void AJanitorPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pJanitor)
	{
		pJanitor->ServerSound();
	}

	if (pJanitor && pJanitor->bPickedUp == false)
	{
		SetActorLocation(pJanitor->GetActorLocation());
	}
	if (pJanitor && pJanitor->bPickedUp)
	{
		//int randomX = FMath::FRandRange(0, 1000); //50 percent to drop item
		//int randomY = FMath::FRandRange(0, 1000);
		pJanitor->SetActorLocation(FVector(32.0f, 32.0f, -9000.f));
	}


	if (pJanitor && pJanitor->bPickedUp)
	{
		pJanitor->pBallAudioComp->SetPaused(true);
	}


	if (bDisabled) //Within toilet range
	{
		pJanitor->GetCharacterMovement()->StopMovementImmediately();
	}

	if (GetActorLocation().Z < -5000)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "JANITOR PICKUP BELOW -5000.z");
	}

	//pMesh2->SetCustomDepthStencilValue(1);
}

void AJanitorPickUp::Activate()
{
	Super::Activate();

	MulticastActivate();
}

void AJanitorPickUp::MulticastActivate_Implementation()
{
	//if (GetLocalRole() == ROLE_Authority)
	{
		pJanitor->SetActorHiddenInGame(false);
		pJanitor->SetActorEnableCollision(true);
		pJanitor->SetActorTickEnabled(true);
	}
}

void AJanitorPickUp::Deactivate()
{
	Super::Deactivate();

	MulticastDeactivate();

}

void AJanitorPickUp::MulticastDeactivate_Implementation()
{
	//if (GetLocalRole() == ROLE_Authority)
	{
		pJanitor->SetActorHiddenInGame(true);
		pJanitor->SetActorEnableCollision(false);
		pJanitor->SetActorTickEnabled(false);
	}
}

void AJanitorPickUp::PickUpObject()
{
	Super::PickUpObject();

	MulticastPickupObject();
}

void AJanitorPickUp::MulticastPickupObject_Implementation()
{
	//if (GetLocalRole() == ROLE_Authority)
	{
		if (pJanitor)
		{
			//SetActorEnableCollision(true);

			pJanitor->Deactivate();

			if (pJanitor->pMesh != nullptr)
				pJanitor->pMesh->bHiddenInGame = true;

			pMesh2->SetVisibility(true);
			//pMesh2->SetSimulatePhysics(false);
			pPickupComponent->SetCollisionProfileName("NoCollision");

			pJanitor->bPickedUp = true;
			pJanitor->pBallAudioComp->SetPaused(true);
		}
	}

}

void AJanitorPickUp::DropObject(FVector direction)
{

	//Super::DropObject(direction); //caused me big issues

	MulticastDropObject();
}

void AJanitorPickUp::MulticastDropObject_Implementation()
{

	//if (GetLocalRole() == ROLE_Authority)
	{
		if (pJanitor)
		{
			pJanitor->Activate();

			pJanitor->bBallRoll = false;
	
			pMesh2->SetVisibility(false);
			//pMesh2->SetSimulatePhysics(true);
			pPickupComponent->SetCollisionProfileName("JanitorPickup");
			//pMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	
			pJanitor->bPickedUp = false;
			pJanitor->SetActorLocation(this->GetActorLocation());

			if (pJanitor->pController)
			{
				pJanitor->pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", false);
				pJanitor->pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", true);
			}

			pJanitor->pBallAudioComp->SetPaused(false);
		}
	}
}

void AJanitorPickUp::Enable()
{
	bDisabled = false;

	//Remove? Keep? Dont know yet
	if (pJanitor->pController)
	{
		pJanitor->pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", false);
		pJanitor->pController->GetBlackboardComponent()->SetValueAsBool("SecuringJanitor", true);
	}
}

void AJanitorPickUp::Disable()
{
	bDisabled = true;
}

void AJanitorPickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJanitorPickUp, pJanitor);
	DOREPLIFETIME(AJanitorPickUp, pMesh2);
	DOREPLIFETIME(AJanitorPickUp, pCollider); 


}