// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEvent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseEvent::ABaseEvent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set up for the main collision on the event, players must interact with this collider at end to finish the event
	pFinalCollider = CreateDefaultSubobject<UBoxComponent>("MainCollider");
	pFinalCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pFinalCollider->SetCollisionProfileName("BlockAll");
	RootComponent = pFinalCollider;

	//Sets up the AI collider, and adds the dynamic calls to call when interacted with and when interaction stops
	pAICollider = CreateDefaultSubobject<UBoxComponent>("AICollider");
	pAICollider->SetCollisionProfileName("OverlapAllDynamic");
	pAICollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	pAICollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pAICollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pAICollider->SetupAttachment(RootComponent);

	//Sets up the main mesh so it doesnt interfere with the collider
	pInteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>("MainMesh");
	pInteractableMesh->SetCollisionProfileName("BlockAll");
	pInteractableMesh->bRenderCustomDepth = true;
	pInteractableMesh->SetupAttachment(RootComponent);

	//The Mesh that will be interacted with and changing which material it will use
	pStatusMesh = CreateDefaultSubobject<UStaticMeshComponent>("StatusMesh");
	pStatusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pStatusMesh->SetIsReplicated(true);
	pStatusMesh->SetupAttachment(RootComponent);
	
	//Setting up the title text per event
	pTitleText = CreateDefaultSubobject<UTextRenderComponent>("TitleText");
	pTitleText->SetupAttachment(RootComponent);

	//Setting up the Additional text that each event may have
	pAdditionalText = CreateDefaultSubobject<UTextRenderComponent>("AdditionalText");
	pAdditionalText->SetNetAddressable();
	pAdditionalText->SetIsReplicated(true);
	pAdditionalText->SetupAttachment(RootComponent);

	//Sets up the Audio for the AI to say
	pAudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio");
	pAudioComponent->SetIsReplicated(true);
	pAudioComponent->SetupAttachment(RootComponent);

	//Ensures the starting state is false
	bIsActive = false;
	bHasActivated = false;
	bCanDeactivate = true;
	bForTeleporter = false;

	Tags.Add("Event");

	SetReplicates(true);
	bAlwaysRelevant = true;
	SetNetUpdateTime(10);
}

// Called when the game starts or when spawned
void ABaseEvent::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		pAICollider->OnComponentBeginOverlap.RemoveDynamic(this, &ABaseEvent::OnAIOverlap);
		pAICollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEvent::OnAIOverlap);
		pAICollider->OnComponentEndOverlap.RemoveDynamic(this, &ABaseEvent::OnAIExit);
		pAICollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEvent::OnAIExit);
	}

	//Assigning and finding the ship state
	AStellarFeverGameState* ptempstate = Cast<AStellarFeverGameState>(GetWorld()->GetGameState());
	pShipState = ptempstate->GetShipState();

	pAudioComponent->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pVoiceSoundClass;
}

// Called every frame
void ABaseEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* ctrl = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (ctrl != nullptr && ctrl->GetPawn() != nullptr && bForTeleporter == false)
	{
		if (pInteractableMesh != nullptr)
		{
			if (bIsActive == true && FVector::Distance(GetActorLocation(), ctrl->GetPawn()->GetActorLocation()) <= fDistanceForRenderOutline)
			{
				pInteractableMesh->SetCustomDepthStencilValue(1);
			}
			else
			{
				pInteractableMesh->SetCustomDepthStencilValue(0);
			}
		}
	}
}

void ABaseEvent::Activate()
{
	Server_Activate();
}

void ABaseEvent::Server_Activate_Implementation()
{
	//Sets that the event is activated
	bIsActive = true;
	bCanDeactivate = false;
	bHasActivated = true;

	//Sets the audio
	if (pActivationSound != nullptr)
	{
		Multicast_PlaySound(pActivationSound);
	}

	//Stops the Timer from working right away
	GetWorld()->GetTimerManager().ClearTimer(AICollisionTimer);

	//A quick check to make sure AI leave the event
	if (pInteractingAI.Num() > 0)
	{
		for (int i = 0; i < pInteractingAI.Num(); i++)
		{
			if (pInteractingAI.IsValidIndex(i) && pInteractingAI[i] != nullptr)
			{
				pInteractingAI[i]->GetRandomEvent();
			}
		}
	}

	//Clears all player pointers before readding them to the array
	pPlayers.Empty();

	Multicast_Activate();
}

void ABaseEvent::Deactivate()
{
	Server_Deactivate();
}

void ABaseEvent::Server_Deactivate_Implementation()
{
	//Adds one to the currency
	if (bIsActive == true)
	{
		if (ActorHasTag("MainEvent"))
		{
			pShipState->AddMoney(1);
		}

		if (pDeactivateSound != nullptr)
			Multicast_PlaySound(pDeactivateSound);
	}

	//Sets that the event is deactivated
	bIsActive = false;
	bCanDeactivate = false;
	Multicast_Deactivate();
}

void ABaseEvent::Failure()
{

}

void ABaseEvent::EventObjectComplete()
{
	Server_EventObjectComplete();
}

void ABaseEvent::Server_EventObjectComplete_Implementation()
{

}

void ABaseEvent::SetupPlayerPointer()
{
	//Finds all players and adds a reference to each event
	for (TActorIterator<ABasePlayer> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ABasePlayer* pTempplayer = Cast<ABasePlayer>(*ActorItr);
		pPlayers.Add(pTempplayer);
	}
}

void ABaseEvent::Multicast_Activate_Implementation()
{
	//Sets the mesh color to Red
	pStatusMesh->SetMaterial(0, pRedMaterial);

	//Sets all potential meshs to visible.
	for (int i = 0; i < pStatusIndicators.Num(); i++)
	{
		if (pStatusIndicators[i] == nullptr)
			continue;
		pStatusIndicators[i]->SetVisibility(true);
	}
}

void ABaseEvent::Multicast_Deactivate_Implementation()
{
	//Sets the mesh color to Green
	pStatusMesh->SetMaterial(0, pGreenMaterial);

	//Sets all potential meshs to red again and makes them disappear from sight
	for (int i = 0; i < pStatusIndicators.Num(); i++)
	{
		if (pStatusIndicators[i] == nullptr)
			continue;
		pStatusIndicators[i]->SetMaterial(0, pRedMaterial);
		pStatusIndicators[i]->SetVisibility(false);
	}
}

void ABaseEvent::Multicast_PlaySound_Implementation(USoundBase* pSound)
{
	//Sets the sound and plays it, can take in any sound from child events, and baseevents sounds as well
	pAudioComponent->SetSound(pSound);
	pAudioComponent->Play();
}

void ABaseEvent::Multicast_ChangeStatus_Implementation(uint8 val)
{
	//Changes the correct status indicator when in range, can be overridden for specific interactions
	if (pStatusIndicators.IsValidIndex(val) == true && pStatusIndicators[val] != nullptr)
	{
		pStatusIndicators[val]->SetMaterial(0, pGreenMaterial);
	}
}

void ABaseEvent::Multicast_PlayMusic_Implementation()
{

}

void ABaseEvent::Multicast_StopMusic_Implementation()
{

}

void ABaseEvent::ResetActivation()
{
	bHasActivated = false;
}

void ABaseEvent::OnAIOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Adds the AI to the array, ensures that if one AI is in the collider they will break it
	if (OtherActor->ActorHasTag("Enemy"))
	{
		ABaseAICharacter* ptempEnemy = Cast<ABaseAICharacter>(OtherActor);
		pInteractingAI.Add(ptempEnemy);
		GetWorld()->GetTimerManager().UnPauseTimer(AICollisionTimer);
	}
}

void ABaseEvent::OnAIExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Enemy"))
	{
		ABaseAICharacter* ptempEnemy = Cast<ABaseAICharacter>(OtherActor);

		for (int i = 0; i < pInteractingAI.Num(); i++)
		{
			if (ptempEnemy == pInteractingAI[i])
			{
				pInteractingAI.RemoveAt(i);
			}
		}
		if (pInteractingAI.Num() == 0)
		{
			GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
		}
	}

}

void ABaseEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEvent, pStatusMesh);
	DOREPLIFETIME(ABaseEvent, pStatusIndicators);
	DOREPLIFETIME(ABaseEvent, bIsActive);
	DOREPLIFETIME(ABaseEvent, bHasActivated);
	DOREPLIFETIME(ABaseEvent, bCanDeactivate);
	DOREPLIFETIME(ABaseEvent, pAdditionalText);
	DOREPLIFETIME(ABaseEvent, bForTeleporter);
	DOREPLIFETIME(ABaseEvent, bForStencil);
}

