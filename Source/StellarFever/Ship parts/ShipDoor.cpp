// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipDoor.h"
#include "Components/BoxComponent.h"
#include "EmergencyLightComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimSequence.h"
#include "EmergencyLightComponent.h"
#include "TimerManager.h"
#include "../AI/AIEnemy.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"


#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
// Sets default values
AShipDoor::AShipDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pDoorTrigger = CreateDefaultSubobject<UBoxComponent>("Door Trigger");
	RootComponent = pDoorTrigger;
	pDoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &AShipDoor::OnTriggerOverlap);
	pDoorTrigger->OnComponentEndOverlap.AddDynamic(this, &AShipDoor::EndTriggerOverlap);

	pDoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>("door mesh");
	pDoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pDoorMesh->SetupAttachment(RootComponent);

	pAudioComp = CreateDefaultSubobject<UAudioComponent>("sound effect");

	pLockLight = CreateDefaultSubobject<UEmergencyLightComponent>("Door Light Component");
	pLockLight->SetupAttachment(RootComponent);
	pLockLight->FixedDoor();

	pLockLight2 = CreateDefaultSubobject<UEmergencyLightComponent>("Door Light Component2");
	pLockLight2->SetupAttachment(RootComponent);
	pLockLight2->FixedDoor();

	SetReplicates(true);
	SetReplicateMovement(true);

	Tags.Add("Door");
}


// Called when the game starts or when spawned
void AShipDoor::BeginPlay()
{
	Super::BeginPlay();
}

void AShipDoor::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		if (OtherActor->ActorHasTag("Player") && pLockLight->GetDoorStatus() == false)
			//if (OtherActor->ActorHasTag("Player") )
		{
			OpenDoor();
			numInteracted++;
			numPlayer++;
		}

		//if enemy at door n no player
		if (OtherActor->ActorHasTag("Enemy"))
		{
			OpenDoor();
			numInteracted++;
			numAI++;

			if (numPlayer == 0 && GetWorldTimerManager().IsTimerActive(LockTimer) == false)
			{
				//start locking door timer
				//GetWorldTimerManager().SetTimer(LockTimer, this, &AShipDoor::HackedDoor, HackingTimer, true);
			}
		}
	}



}

void AShipDoor::EndTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			numInteracted--;
			numPlayer--;

			//when no one at door
			if (numInteracted == 0)
			{
				ClosedDoor();
			}

			//if only AI at door n no player
			if (numPlayer == 0 && numAI > 0 && GetWorldTimerManager().IsTimerActive(LockTimer) == false)
			{
				// AI start hacking
				ClosedDoor();
				//GetWorldTimerManager().SetTimer(LockTimer, this, &AShipDoor::HackedDoor, HackingTimer, true);
			}
		}

		if (OtherActor->ActorHasTag("Enemy"))
		{

			numInteracted--;
			numAI--;

			//if there is no enemy in area
			if (numAI == 0)
			{
				//stop hacking door timer
				GetWorldTimerManager().ClearTimer(LockTimer);
			}

			if (numInteracted == 0)
			{
				ClosedDoor();
			}

		}

	}

}

// Called every frame
void AShipDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if AI dies near door
	if (pDoorTrigger != nullptr)
	{
		TArray<AActor*> CollectedActors;
		pDoorTrigger->GetOverlappingActors(CollectedActors);

		for (auto& actor : CollectedActors)
		{
			AAIEnemy* enemy = Cast<AAIEnemy>(actor);
			if (enemy && enemy->bDied == true && enemy->bResetDoor == false)
			{
				//say AI is dead
				enemy->bDied = true;
				enemy->bResetDoor = true;
				numInteracted--;
			}
		}
	}
}



void AShipDoor::OpenDoor()
{

	if (bIsLocked == false)
	{
		if (bIsDoorOpen == false && pDoorMesh->IsPlaying() == true)
		{
			////reverse the animation 

			float pr = pDoorMesh->GetPlayRate();
			pDoorMesh->SetPlayRate(pr * -1);
			bIsDoorOpen = true;
		}
		else if (bIsDoorOpen == false && pDoorMesh->IsPlaying() == false)
		{
			////add animation
			pDoorMesh->SetAnimation(OpenAnim);
			pDoorMesh->PlayAnimation(OpenAnim, false);
			pAudioComp->SetSound(pOpenSound1);
			pAudioComp->Play();
			bIsDoorOpen = true;
		}
	}
}

void AShipDoor::ClosedDoor()
{
	//	pDoorCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//pDoorCollider->SetCollisionProfileName("BlockAll");

	if (bIsDoorOpen == true && pDoorMesh->IsPlaying() == true)
	{
		float pr = pDoorMesh->GetPlayRate();
		pDoorMesh->SetPlayRate(pr * -1);
		bIsDoorOpen = false;
	}

	if (bIsDoorOpen == true && pDoorMesh->IsPlaying() == false)
	{
		pDoorMesh->SetAnimation(CloseAnim);
		pDoorMesh->PlayAnimation(CloseAnim, false);
		pAudioComp->SetSound(pCloseSound1);
		pAudioComp->Play();
		bIsDoorOpen = false;
	}
	//comment add animation close
}

void AShipDoor::StartFixingDoor()
{
	//start timer to fix door
	GetWorldTimerManager().SetTimer(UnlockTimer, this, &AShipDoor::FixDoor, FixingTimer, true);
}

void AShipDoor::CancelFixingDoor()
{
	//when player let go of interact too soon it is canceled
	GetWorldTimerManager().ClearTimer(UnlockTimer);
}

void AShipDoor::FixDoor()
{
	bIsLocked = false;
	pLockLight->FixedDoor();
	pLockLight2->FixedDoor();
	GetWorldTimerManager().ClearTimer(UnlockTimer);
}

void AShipDoor::HackedDoor()
{
	bIsLocked = true;
	pLockLight->HackedDoor();
	pLockLight2->HackedDoor();
	GetWorldTimerManager().ClearTimer(LockTimer);
}

bool AShipDoor::GetLockedStatus()
{
	return bIsLocked;
}

float AShipDoor::GetHackTimer()
{
	return HackingTimer;
}

void AShipDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipDoor, pDoorMesh);
	//DOREPLIFETIME(AShipDoor, pParticleSystem);

}


//#undef print
