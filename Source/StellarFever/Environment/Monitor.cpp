// Fill out your copyright notice in the Description page of Project Settings.


#include "Monitor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "MonitorState.h"
#include "MonitorWidget.h"
#include "UObject/UObjectGlobals.h"
#include "Components/BoxComponent.h"
#include "../Player/BasePlayer.h"
#include "Net/UnrealNetwork.h"
#include "../StellarFeverGameModeBase.h"
#include "../Events/EventsManager.h"

// Sets default values
AMonitor::AMonitor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pModel = CreateDefaultSubobject<UChildActorComponent>("Model");
	SetRootComponent(pModel);

	pRangeBox = CreateDefaultSubobject<UBoxComponent>("InteractRange");
	pRangeBox->SetupAttachment(RootComponent);
	pRangeBox->SetGenerateOverlapEvents(true);

	pWidget = CreateDefaultSubobject<UWidgetComponent>("Widget");
	pWidget->SetupAttachment(RootComponent);

	NetUpdateFrequency = 10.f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMonitor::BeginPlay()
{
	Super::BeginPlay();


	//Must be added in BeginPlay, as it does not yet exist in the constructor
	pState = NewObject<AMonitorState>(this);
	pState->SetOwner(this);
	//pWidget->AttachToComponent(pModel, FAttachmentTransformRules::KeepWorldTransform, "Monitor");
	UMonitorWidget* widget = Cast<UMonitorWidget>(pWidget->GetUserWidgetObject());
	if (widget != nullptr)
	{
		widget->SetMonitorState(pState);
		pRangeBox->OnComponentEndOverlap.AddDynamic(this, &AMonitor::EndComponentOverlap);

		if (bSyncShipEvents && GetLocalRole() == ROLE_Authority)
			GetWorld()->GetTimerManager().SetTimer(ShipStateHandle, this, &AMonitor::UpdateShipState, 2.f, true, -1.f);
	}
	
}

// Called every frame
void AMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonitor::UpdateShipState_Implementation()
{
	if (GetWorld() != nullptr && GetWorld()->GetAuthGameMode<AStellarFeverGameModeBase>() != nullptr && GetWorld()->GetAuthGameMode<AStellarFeverGameModeBase>()->pEventManager != nullptr)
		usEventsState = *(uint16*)&GetWorld()->GetAuthGameMode<AStellarFeverGameModeBase>()->pEventManager->GetEventsState();
}

UWidgetComponent* AMonitor::GetWidget()
{
	return pWidget;
}



AMonitorState* AMonitor::GetMonitorState()
{
	return pState;
}

void AMonitor::EndComponentOverlap(UPrimitiveComponent* overlappedComp, AActor* other, UPrimitiveComponent* otherPrimitive, int32 OtherBodyIndex)
{
	if (pState != nullptr)
	{
		//Return to main screen after last player leaves
		TArray<AActor*> players;
		overlappedComp->GetOverlappingActors(players, ABasePlayer::StaticClass());
		if (players.Num() == 0)
			if (pState->GetPage() != 0)
				pState->SetPage(0);
	}
}

void AMonitor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMonitor, usEventsState);
	//DOREPLIFETIME(AMonitor, pWidget);
	//DOREPLIFETIME(AMonitor, pState);
}

const FEventsState& AMonitor::GetEventsState() const
{
	return *(FEventsState*)&usEventsState;
}