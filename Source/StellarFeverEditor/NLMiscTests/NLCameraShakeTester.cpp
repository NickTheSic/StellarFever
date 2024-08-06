// Copyright Quarantine Games 2020


#include "NLCameraShakeTester.h"
#include "Kismet/GameplayStatics.h"
#include "StellarFever/Player/BasePlayer.h"

void ANLCameraShakeTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;

	if (Timer > 2.0f)
	{
		Timer = 0.0f;

		for (ABasePlayer* player : pPlayers)
		{
			UGameplayStatics::PlayWorldCameraShake(player, Shake, player->GetActorLocation(), 0.0f,500.0f,1.0f);
		}

	}
}