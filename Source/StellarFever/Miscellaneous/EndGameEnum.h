// Copyright Quarantine Games 2020
#pragma once

#include "CoreMinimal.h"
#include "EndGameEnum.generated.h"

/**
 * An enum for keeping track of how the game ended
 */
UENUM()
enum class EEndGameReason : uint8
{
	GameWon = 0,

	AllPlayersDied,
	EnemiesStoleVaccines,


	DefaultReason = 255,
};
