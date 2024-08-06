// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SFBasicSaveGame.generated.h"

UENUM()
enum class EInvertControllerAxis
{
	Invert_None		= 0x00,
	Invert_Axis_X	= 0x01,
	Invert_Axis_Y	= 0x02,
	Invert_Axis_XY	= 0x03
};

/**
 * 
 */
UCLASS()
class STELLARFEVER_API USFBasicSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere)
		FString SaveSlotName;
	UPROPERTY(VisibleAnywhere)
		uint32 UserIndex;
	UPROPERTY(VisibleAnywhere)
		bool bTutorialComplete;
	UPROPERTY(VisibleAnywhere)
		float fBrightness;
	UPROPERTY(VisibleAnywhere)
		float fLookSensitivity;
	UPROPERTY(VisibleAnywhere)
		float fGeneralVolume;
	UPROPERTY(VisibleAnywhere)
		float fMusicVolume;
	UPROPERTY(VisibleAnywhere)
		float fVoiceVolume;
	UPROPERTY(VisibleAnywhere)
		EInvertControllerAxis eInvertAxis;

	USFBasicSaveGame();

};
