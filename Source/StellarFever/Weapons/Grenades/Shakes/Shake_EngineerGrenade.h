// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "Shake_EngineerGrenade.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UShake_EngineerGrenade : public UCameraShake
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		float fOscillationDuration;
	UPROPERTY(EditAnywhere)
		float fOscillationBlendIn;
	UPROPERTY(EditAnywhere)
		float fOscillationBlendOut;

	UPROPERTY(EditAnywhere)
		float fPitchAmplitude;
	UPROPERTY(EditAnywhere)
		float fPitchFrequency;
	UPROPERTY(EditAnywhere)
		bool bPitchSine;

	UPROPERTY(EditAnywhere)
		float fYawAmplitude;
	UPROPERTY(EditAnywhere)
		float fYawFrequency;
	UPROPERTY(EditAnywhere)
		bool bYawRandom;
	UPROPERTY(EditAnywhere)
		bool bYawSine;

	UPROPERTY(EditAnywhere)
		float fRollAmplitude;
	UPROPERTY(EditAnywhere)
		float fRollFrequency;
	UPROPERTY(EditAnywhere)
		bool bRollRandom;
	UPROPERTY(EditAnywhere)
		bool bRollSine;

	UPROPERTY(EditAnywhere)
		bool bIsSingleInstance;

	UShake_EngineerGrenade();
	void Init();
};
