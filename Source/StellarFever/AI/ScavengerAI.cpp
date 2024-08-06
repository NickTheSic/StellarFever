// Copyright Quarantine Games 2020


#include "ScavengerAI.h"
#include "..\Components/HealthComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/Material.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "../Events/PickupItems/VaccineObject.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Engine.h"
#include "..\Miscellaneous\ShipState.h"
#include "..\StellarFeverGameState.h"

// Sets default values
AScavengerAI::AScavengerAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//pHealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	pVaccineModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VaccineModel"));
	pVaccineModel->SetVisibility(false);
	pVaccineModel->SetupAttachment(RootComponent);

	pInteractCollider = CreateDefaultSubobject<USphereComponent>(TEXT("InteractColliderComp"));
	pInteractCollider->SetCollisionProfileName("OverlapAll");
	pInteractCollider->SetupAttachment(RootComponent);

	pVaccine2 = nullptr;

	pHealthComp->DefaultHealth = 40.f;
	InitialHealth = pHealthComp->DefaultHealth;

	//pHealthComp->DefaultHealth *= fHealthIncrease;

	//bAlwaysRelevant = true;
	//SetReplicates(true);
	//SetReplicateMovement(true);

	fOriginalWalkSpeed = 500;
	GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AScavengerAI::BeginPlay()
{
	Super::BeginPlay();

	//pHealthComp->OnHealthChanged.AddDynamic(this, &AScavengerAI::OnHealthChanged);

	pInteractCollider->OnComponentBeginOverlap.AddDynamic(this, &AScavengerAI::OnBeginOverlap);

	SpawnedLocation = GetActorLocation(); //spawns and returns

	//fOriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	bVaccineRetrieved = false;
	GetWorldTimerManager().SetTimer(TimerHandle_VaccineSearch, this, &AScavengerAI::FindClosestVaccine, .5f, true);

	//Set beginning initial so Scavengers dont delete right away (works if they have no vaccine in hand)
	GetWorldTimerManager().SetTimer(TimerHandle_PreventDisappearing, this, &AScavengerAI::BeginPreventDisappearing, 3.2f, false);

}

// Called every frame
void AScavengerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FindClosestVaccine();

	// Get vaccine 
	AAIController* controller = Cast<AAIController>(GetController());

	if (bStunned)
	{
		//SO he cannot disappear when stunned
		GetWorldTimerManager().ClearTimer(TimerHandle_Dissappear);
	}

	//if (controller && !bVaccineRetrieved) //get vaccine
	if (controller && !bVaccineRetrieved && pBestTargetActor)
	{
		//controller->MoveToActor(pBestTargetActor);
		UAIBlueprintHelperLibrary::SimpleMoveToActor(controller, pBestTargetActor);
	}

	if (controller && (bVaccineRetrieved)) //retrieve vaccine
	{
		//controller->MoveToLocation(SpawnedLocation);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, SpawnedLocation);
		bIsCarrying = true;

		//pVaccineModel->SetVisibility(true);
		//GetCharacterMovement()->MaxWalkSpeed = fAverageMovementSpeed * 0.32f;
	}



	float Distance = (GetActorLocation() - SpawnedLocation).Size();

	//if (Distance < 100.f && bVaccineRetrieved && bPreventDisappearing)
	if (Distance < 100.f && bPreventDisappearing)
	{
		//do not increase vaccine currency
		if (bVaccineRetrieved)
		{
			AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(GetWorld()->GetGameState());
			if (gameState)
			{
				AShipState* shipState = Cast<AShipState>(gameState->GetShipState());
				if (shipState && GetLocalRole() == ROLE_Authority)
				{
					shipState->SubVaccines(1);

					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Vaccines: " + FString::SanitizeFloat(shipState->GetVaccines()));
				}
			}
			
		}

		Disappear();
		//GetWorldTimerManager().ClearTimer(TimerHandle_Dissappear);
	}


	//Return with nothing
	if (Distance < 11500.f && GetCharacterMovement()->Velocity.Size() == 0 && bDisappear == false && bPreventDisappearing)
	{
		bDisappear = true;
		GetWorldTimerManager().SetTimer(TimerHandle_Dissappear, this, &AScavengerAI::Disappear, 2.3f, false);
		//Returns with nothing
	}

	if (GetCharacterMovement()->Velocity.Size() > 0 && bDisappear)
	{
		bDisappear = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_Dissappear);
	}


	if (!bDisappear && !bVaccineRetrieved && GetCharacterMovement()->Velocity.Size() == 0)
	{
		bDisappear = true;
	}
	if (GetCharacterMovement()->Velocity.Size() > 0 && bDisappear)
	{
		bDisappear = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_Dissappear);
	}

}

void AScavengerAI::Disappear()
{
	if (pVaccine2)
	{
		pVaccine2->Deactivate();
	}
	bScavengerHidden = true; //?
	bTaken = false; //If taken, turn to false when scavenger disappears
	Deactivate(); //? BRING TO OTHER SHIP? CODE HERE!!!!!!!!!!
}

void AScavengerAI::BeginPreventDisappearing()
{
	//Prevents Scavengers from deleting right away
	bPreventDisappearing = true;
}

void AScavengerAI::TestDrop()
{
	if (pVaccine2)
	{
		pVaccine2->bIsPickedUpVaccine = false;
		FVector direction = GetActorForwardVector();
		pVaccine2->DropObject(direction);
		pVaccine2->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		pVaccine2->Tags.Add("Vaccines");
	}
}

// Called to bind functionality to input
void AScavengerAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AScavengerAI::Reset()
{
	Super::Reset();

	//GetWorldTimerManager().SetTimer(TimerHandle_VaccineSearch, this, &AScavengerAI::FindClosestVaccine, .5f, true);
	bScavengerHidden = false;
}

void AScavengerAI::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (pVaccine2 == nullptr)
	{
		if (OtherActor->ActorHasTag("Vaccines"))
		{
			pVaccine2 = Cast<AVaccineObject>(OtherActor);
			//if (OtherActor->ActorHasTag("Vaccines"))
			
			if (pVaccine2 && pVaccine2->bIsPickedUpVaccine == false)
			{
				pVaccine2->Tags.Remove("Vaccines");
				pVaccine2->bIsPickedUpVaccine = true;
				pVaccine2->PickUpObject();
				bVaccineRetrieved = true;
				pVaccine2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "MeleeHitPoint");
				//GetWorldTimerManager().SetTimer(TimerHandle_DropTest, this, &AScavengerAI::TestDrop, 2.f, false);
			}
		}

	}
}

void AScavengerAI::FindClosestVaccine()
{
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), "Vaccines", pOutVaccines);

	fNearestTargetDistance = 10000000.f;

	for (int i = 0; i < pOutVaccines.Num(); i++)
	{
		//bVaccineExist = true; //meant for chasing spawn point when none exist
		float Distance = (GetActorLocation() - pOutVaccines[i]->GetActorLocation()).Size();
		if (Distance < fNearestTargetDistance)
		{
			fNearestTargetDistance = Distance;
			pBestTargetActor = pOutVaccines[i];
		}
	}

	if (pOutVaccines.Num() <= 0) //causing issues?
	{
		//bVaccineRetrieved = true;
		//bEmptyHanded = true;
		//bVaccineExist = false;
		AAIController* controller = Cast<AAIController>(GetController());
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, SpawnedLocation);
	}
}

void AScavengerAI::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType2, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health > 0 && !bDied && bStunned)
	{
		StunnedRelease();
	}

	if (Health <= 0 && !bDied)
	{
		//DropItem();f

		bDied = true;
		UGameplayStatics::PlaySoundAtLocation(this, pDeathSounds, GetActorLocation());

		bTaken = false;

		if (pVaccine2)
		{
			pVaccine2->bIsPickedUpVaccine = false;
			FVector direction = GetActorForwardVector();
			pVaccine2->DropObject(direction);
			pVaccine2->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			pVaccine2->Tags.Add("Vaccines");

		}

		GetWorldTimerManager().SetTimer(TimerHandle_DropVaccine, this, &AScavengerAI::DropVaccineDelay, 0.2f, false);

	}
}

void AScavengerAI::DropVaccineDelay()
{
	bIsCarrying = false;
	Deactivate();
}
