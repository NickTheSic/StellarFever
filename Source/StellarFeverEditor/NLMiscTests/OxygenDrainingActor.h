// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "NLSelfTestActorFinder.h"
#include "OxygenDrainingActor.generated.h"

UCLASS()
class STELLARFEVEREDITOR_API AOxygenDrainingActor : public ANLSelfTestActorFinder
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOxygenDrainingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
