// Copyright Quarantine Games 2020


#include "LobbyTeleporter.h"
#include "LobbyGamemode.h"
#include "../Player/BasePlayer.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ALobbyTeleporter::ALobbyTeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	pTeleportArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Teleport Area"));
	SetRootComponent(pTeleportArea);

	pTeleportMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Mesh"));
	pTeleportMesh->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void ALobbyTeleporter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		pLobbyGamemode = GetWorld()->GetAuthGameMode<ALobbyGamemode>();

		if (pLobbyGamemode == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("The Lobby teleporter wasn't added to a level with a LobbyGamemode"));
		}

		pTeleportArea->OnComponentBeginOverlap.AddDynamic(this, &ALobbyTeleporter::OnBeginOverlap);
		pTeleportArea->OnComponentEndOverlap.AddDynamic(this, &ALobbyTeleporter::OnEndOverlap);
	}

}


void ALobbyTeleporter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (pLobbyGamemode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The Lobby lobbyGamemode was null"));
		return;
	}

	if (ABasePlayer* baseplayer = Cast<ABasePlayer>(OtherActor))
	{
		PlayersInsideMe++;

		if (pLobbyGamemode->uiConnectedPlayers == PlayersInsideMe)
			pLobbyGamemode->StartGame();
	}
}


void ALobbyTeleporter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABasePlayer* baseplayer = Cast<ABasePlayer>(OtherActor))
	{
		PlayersInsideMe--;
	}
}
