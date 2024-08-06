// Copyright Quarantine Games 2020


#include "Teleporter.h"
#include "../SpawnPoint.h"
#include "../../Player/BasePlayer.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../MainEvents/TeleporterEvent.h"
#include "../EventObject.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creating all objects for the class
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	pMesh->SetCollisionProfileName("BlockAll");
	RootComponent = pMesh;

	pTeleRings = CreateDefaultSubobject<USkeletalMeshComponent>("TeleRings");
	pTeleRings->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pTeleRings->SetIsReplicated(true);
	pTeleRings->SetupAttachment(RootComponent);

	pCollider = CreateDefaultSubobject<UBoxComponent>("Collider");
	pCollider->SetCollisionProfileName("OverlapAllDynamic");
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Ignore);
	pCollider->SetupAttachment(RootComponent);

	bisActive = false;
	bButtonPressed = false;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATeleporter::BeginPlay()
{
	Super::BeginPlay();
	
	//Setting up the dynamic collision calls
	if (GetLocalRole() == ROLE_Authority)
	{
		pCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ATeleporter::OnColliderOverlap);
		pCollider->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnColliderOverlap);
	}

	if (pButton != nullptr)
	{
		pButton->bForTeleporter = true;
	}
}

// Called every frame
void ATeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//A check for when the button is pressed
	if (pButton->GetIsActive() == false && bButtonPressed == false)
	{
		//Disables the button and starts the timers
		GetWorld()->GetTimerManager().SetTimer(TeleportTimer, this, &ATeleporter::ActivateTeleport, fTeleportSpeed);
		GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &ATeleporter::Reset, fResetSpeed);
		Multicast_Animation();
		bButtonPressed = true;
	}
}

void ATeleporter::Activate()
{
	Server_Activate();
}

void ATeleporter::Server_Activate_Implementation()
{
	bisActive = true;

	//Ensures the teleporter isnt instantly active on both ships
	pCollider->SetCollisionProfileName("NoCollision");

	//Activates the event object for teleporting
	pButton->Activate();
}

void ATeleporter::Deactivate()
{
	Server_Deactivate();
}

void ATeleporter::Server_Deactivate_Implementation()
{
	bisActive = false;

	//Ensures the teleporter isnt active
	pCollider->SetCollisionProfileName("NoCollision");

	//Deactivates the event object for teleporting
	pButton->Deactivate();
}

void ATeleporter::Multicast_Animation_Implementation()
{
	pTeleRings->PlayAnimation(pRingAnim, false);
}

void ATeleporter::Teleport(ABasePlayer* player)
{
	player->SetActorLocationAndRotation(pTeleportPoint->GetActorLocation(), pTeleportPoint->GetActorRotation());
}

void ATeleporter::ActivateTeleport()
{
	//sets the bool so teleporting can work
	bCanTeleport = true;

	//Activates the collision again
	pCollider->SetCollisionProfileName("OverlapAllDynamic");
}

void ATeleporter::Reset()
{
	//sets the bool so teleporting can't work
	bCanTeleport = false;

	//Activates the collision to no collision
	pCollider->SetCollisionProfileName("NoCollision");

	//The button can be pressed again
	pButton->Activate();
	bButtonPressed = false;
}

void ATeleporter::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bisActive == true && bCanTeleport == true)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			ABasePlayer* tempplayer = Cast<ABasePlayer>(OtherActor);
			Teleport(tempplayer);

			if (pTeleporterEvent != nullptr)
			{
				//Adding the player to the teleporter array to see if they are on the ship or not
				if (bIsEnemyTeleporter == true)
				{
					pTeleporterEvent->RemovePlayer(tempplayer);
					tempplayer->RemoveFromPlayerStatusFlag(EPlayerStatus::OnEnemyShip);
				}
				else
				{
					
					pTeleporterEvent->AddPlayer(tempplayer);
					tempplayer->AddToPlayerStatusFlag(EPlayerStatus::OnEnemyShip);
				}
			}
		}
	}
}

void ATeleporter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeleporter, pTeleRings);
}