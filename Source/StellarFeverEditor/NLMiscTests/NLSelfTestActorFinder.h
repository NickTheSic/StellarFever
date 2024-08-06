// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NLSelfTestActorFinder.generated.h"

UCLASS()
class STELLARFEVEREDITOR_API ANLSelfTestActorFinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANLSelfTestActorFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<class ABasePlayer*> pPlayers;

	void FindPlayers();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
