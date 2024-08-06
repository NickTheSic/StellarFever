// Copyright Quarantine Games 2020


#include "NLSelfTestActorFinder.h"
#include "Kismet/GameplayStatics.h"
#include "StellarFever/Player/BasePlayer.h"

// Sets default values
ANLSelfTestActorFinder::ANLSelfTestActorFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANLSelfTestActorFinder::BeginPlay()
{
	Super::BeginPlay();
	
	FindPlayers();

}

// Called every frame
void ANLSelfTestActorFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ANLSelfTestActorFinder::FindPlayers()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABasePlayer::StaticClass(), actors);

	pPlayers.Empty();

	for (AActor* actor : actors)
	{
		ABasePlayer* player = Cast<ABasePlayer>(actor);

		if (player != nullptr)
		{
			pPlayers.Add(player);
		}
	}

	actors.Empty();
}

