// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "PlayerInformation.generated.h"

/**
 * The following is to contain player information for the player
 * The file consists of enums and structs and allows for a single
 * include of player information without having to include the entire player
 */

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	Base	 = 255,
	Captain  = 0,
	Gunner   = 1,
	Engineer = 2,
	Medic    = 3,
};


USTRUCT(BlueprintType)
struct STELLARFEVER_API FPlayerInformation
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Player Information")
		EPlayerClass ePlayerClass = EPlayerClass::Base;

	UPROPERTY(EditAnywhere, Category = "Player Information")
		float fPlayerInteractDistance = 300.0f;
	
	UPROPERTY(EditAnywhere, Category = "Player Information", meta = (ToolTip="How much oxygen this player has") )
		float fMaxLungSize = 100.0f;


	/**** Knockedout / Revive Values ****/
	UPROPERTY(EditAnywhere, Category = "Player Information")
		float fKnockedoutTimer = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information")
		float fReviveSpeed = 6.0f;


	/*------Speed Values--------*/
	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fWalkSpeed = 800.0f;	//700

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fRunSpeed = 1400.0f; 

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fCarrySpeed = 600.0f; 

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fCrouchSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fDownedMovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fMovementNoise = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fHeavyMovementNoise = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Movement Speed")
		float fCrouchedMovementNoise = 0.1;



	/*----- Stamina Valuess -----*/

	UPROPERTY(EditAnywhere, Category = "Player Information | Stamina")
		float fMaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Stamina")
		float fStaminaDrainRate = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Stamina")
		float fStaminaReplenishRate = 30.0f;


	/*----- Ability Values ------*/
	UPROPERTY(EditAnywhere, Category = "Player Information | Special", meta = (ToolTip = "How long before the player can use their special again"))
		float fActiveAbilityCooldown = 10.0f; 

	UPROPERTY(EditAnywhere, Category = "Player Information | Special", meta = (ToolTip = "How long before the players special is active"))
		float fActiveAbilityLength = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Player Information | Class Colour")
		FColor ClassColour = FColor::White;

};


//A Flag to use instead of constantly adding bools
UENUM()
enum class EPlayerStatus : uint16
{
	Normal        = 0,  // 0

	//8 bits
	Downed        = 1,  // 1
	Dead          = 2,  // 2
					    
	Running       = 4, // 3
	Crouched      = 8, // 4
	HoldingPickup = 16, // 5
	CanUseSpecial = 32, //6

	SpecialIsActive = 64,// 7
	PlayerInMenu = 128, //8

	//16 Bits
	IsInteracting = 256, //9
	IsFiring = 512, //10  

	Enraged = 1024, //11  
	OnEnemyShip = 2048,  //12
	//4096   //13
	//8192   //14
	//16384  //15


	MAXVALUE = 1+2+4+8+16+32+64+128+256+512+1024+2048,	//I use this to clamp the Add and Remove flag to this value
};

