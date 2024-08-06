// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "NLSelfTestActorFinder.h"
#include "NLCameraShakeTester.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API ANLCameraShakeTester : public ANLSelfTestActorFinder
{
	GENERATED_BODY()
	
public:

	float Timer = 0.0f;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> Shake = UCameraShake::StaticClass();

	virtual void Tick(float dt) override;

};
