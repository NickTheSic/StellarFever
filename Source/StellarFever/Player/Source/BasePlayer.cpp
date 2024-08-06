// Coded by Nick Lemmon

#include "../BasePlayer.h"
#include "../../Bay/MedicBay.h"
#include "../../Bay/AmmoBay.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Components/RaycastComponent.h"
#include "../../Components/StatusComponent.h"
#include "../../Events/BaseEvent.h"
#include "../../Events/ShipSystems/DoorObject.h"
#include "../../Events/PickUpItem.h"
#include "../../Events/LevelProgression/RoundPlanet.h"
#include "../../Events/ShipSystems/Shop/ShopItem.h"
#include "../../Ship parts/ShipDoor.h"
#include "../../StellarFeverGameInstance.h"
#include "../../StellarFeverGameModeBase.h"
#include "../../Weapons/WeaponBase.h"
#include "../../Weapons/GrenadeBase.h"
#include "../../Weapons/GunBase.h"
#include "../Animation/PlayerActionMontageComponent.h"
#include "../SF_PlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "DrawDebugHelpers.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Net/UnrealNetwork.h"

#include "../../Environment/Monitor.h"
#include "../../Environment/MonitorState.h"

#include "../../BeenoFight/BeenoDimeBoss.h"


#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg) 
#endif


// Sets default values
ABasePlayer::ABasePlayer()
{
    PrimaryActorTick.bCanEverTick = true;

	//Setup Networking and tag
	bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	Tags.Add("Player");

    //Setup Capsule component
	SetRootComponent(GetCapsuleComponent());
    GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
    GetCapsuleComponent()->SetCollisionProfileName("BlockAll");
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

	//Setup the Camera
    pCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    pCameraComponent->SetupAttachment(GetCapsuleComponent());
    pCameraComponent->SetRelativeLocation(StandingCamera); //TODO: Setup for our Arms
	pCameraComponent->bUsePawnControlRotation = true;


	//Setup the player meshes
	//Create
	pFirstPersonPlayerMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh Component"));
	pFirstPersonPlayerMeshComponent->SetupAttachment(pCameraComponent);
	//Shadows
	pFirstPersonPlayerMeshComponent->bCastDynamicShadow = false;
	pFirstPersonPlayerMeshComponent->CastShadow = false;
	//Collision
	pFirstPersonPlayerMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pFirstPersonPlayerMeshComponent->SetCollisionProfileName("NoCollision");
	pFirstPersonPlayerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pFirstPersonPlayerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	//Visibility
	pFirstPersonPlayerMeshComponent->SetOnlyOwnerSee(true);
	pFirstPersonPlayerMeshComponent->SetVisibility(true);


	//Create
	pThirdPersonPlayerMeshComponent = GetMesh();
	pThirdPersonPlayerMeshComponent->SetupAttachment(GetCapsuleComponent());
	//Shadows
	pThirdPersonPlayerMeshComponent->bCastDynamicShadow = false;
	pThirdPersonPlayerMeshComponent->CastShadow = false;
	//Collision
	pThirdPersonPlayerMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pThirdPersonPlayerMeshComponent->SetCollisionProfileName("NoCollision");
	pThirdPersonPlayerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pThirdPersonPlayerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	//Visibility
	pThirdPersonPlayerMeshComponent->SetOwnerNoSee(true);
	pThirdPersonPlayerMeshComponent->SetVisibility(true);
	//We want this mesh to be networked
	pThirdPersonPlayerMeshComponent->SetIsReplicated(true);


	//Setup the AI Perception //Might still require a setup in blueprints
	pAIPerception = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Perception"));
	pAIPerception->RegisterForSense(UAISense_Hearing::StaticClass());
	pAIPerception->RegisterForSense(UAISense_Sight::StaticClass());


	//Setup the custom Components
	pHealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	pInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	pOxygenComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("Oxygen Component"));
	pOxygenComponent->SetRefillWait(6.0f); //Roberts Oxygen event takes 5 seconds so we need this to take longer
	pOxygenComponent->SetIsReplicated(true);

	pPlayerAnimComponent = CreateDefaultSubobject<UPlayerActionMontageComponent>(TEXT("Animation Component"));

	pPlayerSoundComponent = CreateDefaultSubobject<UPlayerSoundComponent>(TEXT("Sound Component"));
	pPlayerSoundComponent->SetupAttachment(GetCapsuleComponent());

	pRaycastComponent = CreateDefaultSubobject<URaycastComponent>(TEXT("Raycast Component"));

	pStaminaComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("Stamina Component"));

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	}

    //Setup widget to interact with world UI
    pWidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>("WidgetInteractionComponent");
    pWidgetInteractionComponent->InteractionDistance = playerInformation.fPlayerInteractDistance;
    pWidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::CenterScreen;
}


//Called after all components have been initialized
void ABasePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ROLE_Authority)
	{
		pHealthComponent->OnHealthChanged.AddDynamic(this, &ABasePlayer::OnHealthChanged);

		if (pOxygenComponent != nullptr)
		{
			pOxygenComponent->SetMaxValue(playerInformation.fMaxLungSize);
			pOxygenComponent->SetValue(playerInformation.fMaxLungSize);
		}


		if (pStaminaComponent != nullptr)
		{
			pStaminaComponent->SetMaxValue(playerInformation.fMaxStamina);
			pStaminaComponent->SetValue(playerInformation.fMaxStamina);
		}


		//Setup our inventory
		if (pInventoryComponent != nullptr)
		{
			pInventoryComponent->SetupInventory();
		}
	}

	if (pPlayerAnimComponent != nullptr)
	{
		pPlayerAnimComponent->SetupOwningBasePlayer(this);
	}

}


// Called when the game starts or when spawned
void ABasePlayer::BeginPlay()
{
    Super::BeginPlay();

	//Possible to do during PostLogin?
	//We only want to do this once per new player
	if (GetLocalRole() == ROLE_Authority)
	{
		//Make sure that the Player is able to move
		IgnorePlayerInput(false);

		//This will setup some walk defaults
		//Setting the bool to true so we actually setup the defaults
		AddToPlayerStatusFlag(EPlayerStatus::Running);
		StopRunning();

		//Make sure that the player is Normal when we start the game
		ResetPlayerStatusFlag();

		AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
		if (pGameMode)
		{
			pGameMode->PlayerAmount++;
			pGameMode->WhenPlayersAllDie();
		}


		if (pInventoryComponent != nullptr)
		{
			pActiveWeapon = pInventoryComponent->SwitchToGun();
		}

		//Disable shield in boss fight
		if (UGameplayStatics::GetActorOfClass(GetWorld(), ABeenoDimeBoss::StaticClass()) != nullptr)
		{
			pHealthComponent->bDisableShieldRegeneration = true;
		}
	}
}

void ABasePlayer::SetupWidgetInteractionUserID_Implementation()
{
	int iPlayerWidgetID = 0;

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		iPlayerWidgetID = 1;
	}

	pWidgetInteractionComponent->VirtualUserIndex = iPlayerWidgetID;
	pWidgetInteractionComponent->PointerIndex = iPlayerWidgetID;

	#if WITH_EDITOR
	pWidgetInteractionComponent->bShowDebug = true;
	#endif
}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


	if (!IsLocallyControlled())
	{
		FRotator NewRot = pCameraComponent->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.0f / 255.0f;

		pCameraComponent->SetRelativeRotation(NewRot);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		HandlePlayerIsRunning(DeltaTime);
		HandleOxygenComponentTick(DeltaTime);
		HandleStaminaComponentTick(DeltaTime);
	}

	if (HasPickupBeenPickedUp() == false && GetLocalRole() == ROLE_Authority)
	{
		if (GetIsPlayerHoldingItem())
			HandlePickupSetDown();
	}
}


void ABasePlayer::HandlePlayerIsRunning(float DeltaTime)
{
	if (!GetIsPlayerRunning())
	{
		return;
	}

	else if (GetIsPlayerRunning())
	{
		if (pPlayerSoundComponent != nullptr &&
			pPlayerSoundComponent->IsPlaying() == false &&
			pStaminaComponent->GetPercentage() < 0.75f)
		{
			if (FMath::RandRange(0, 10) < 3)
			{
				pPlayerSoundComponent->PlayRunningGrunt();
			}
		}

		if (pStaminaComponent->IsEmpty())
		{
			print("Out Of Stamina");
			StopRunning();
		}
	}
}


void ABasePlayer::HandleOxygenComponentTick(float DeltaTime)
{
	//Start taking damage in we are out of oxygen and only if we aren't already dead
	if (pOxygenComponent->IsEmpty())
	{
		DrainHealth(DeltaTime);
	}

	//Check the percentage to play a sound if we aren't dead
	if ( (pOxygenComponent->GetPercentage() < 0.2f || 
		  pOxygenComponent->GetMaxValue() <= 20)
	  && (!GetIsPlayerDead() || !GetIsPlayerDowned()))
	{
		if (pPlayerSoundComponent != nullptr && !pPlayerSoundComponent->IsPlaying())
		{
			if (FMath::RandRange(0, 10) < 3)
			{
				pPlayerSoundComponent->PlayGasping();
			}
		}
	}
}


void ABasePlayer::HandleStaminaComponentTick(float DeltaTime)
{
	bool oxygenEmpty = pOxygenComponent->IsEmpty();

	if (!GetIsPlayerRunning() && !oxygenEmpty)
		return;

	//If our velocity is 0 then we aren't moving
	if (GetIsPlayerRunning() && GetVelocity().X != 0.0f && GetVelocity().Y != 0.0f)
	{
		DrainStamina(playerInformation.fStaminaDrainRate * DeltaTime);
	}
	
	if (oxygenEmpty)
	{
		DrainStamina(0.5f * DeltaTime);
	}
}

void ABasePlayer::StartInteraction()
{
	//UI Interaction Code
	if (pWidgetInteractionComponent->IsOverInteractableWidget())
	{
		pWidgetInteractionComponent->PressPointerKey(FKey("LeftMouseButton"));
		return;
	}

	//When I call interact while in a menu I only want to interact with that menu 
	if (GetIsPlayerInMenu() || GetIsPlayerDowned() || GetIsPlayerDead())
	{
		return;
	}

	Server_StartInteraction();
}

void ABasePlayer::Server_StartInteraction_Implementation()
{

	if (HasPickupBeenPickedUp())
	{
		//We don't want to do anything if we are holding an item
		HandlePickupSetDown();
		return;
	}

	else
	{

		if (pRaycastComponent == nullptr)
		{
			print("Raycast Component was null")
			return;
		}

		
		FHitResult hit;

		if (pRaycastComponent->RaycastSingleFromPlayer(hit, playerInformation.fPlayerInteractDistance, ECollisionChannel::ECC_MAX, true))
		{

			AActor* actor = hit.GetActor();
			
			//Might be a redundant check since we are already in the function
			if (actor != nullptr)
			{

				#if WITH_EDITOR
				print(actor->GetActorLabel());
				DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Cyan, false, 5.0f);
				#endif


				//We check if we are interacting with a button
				if (actor->ActorHasTag("Event"))
				{
					ABaseEvent* pEventObject = Cast<ABaseEvent>( hit.GetActor());
					if (pEventObject)
					{
						if (pEventObject->GetIsActive())
						{
							pEventObject->Deactivate();

							pPlayerAnimComponent->PlayEventInteractionAnimation();
							pPlayerSoundComponent->PlayEventInteractGeneral();
							
							return; 
						}
					}
				}

				//Interaction with a planet for the next round
				if (actor->ActorHasTag("Planet"))
				{
					ARoundPlanet* pPlanet = Cast<ARoundPlanet>(hit.GetActor());
					if (pPlanet)
					{
						pPlanet->Deactivate();
						pPlayerAnimComponent->PlayPlanetSelectAnimation();
						if (pPlayerSoundComponent != nullptr)
						{
							pPlayerSoundComponent->PlayJumpSelection();
						}
						return;
					}
				}


				if (actor->ActorHasTag("PickUpItem"))
				{
					pPickupItem = Cast<APickUpItem>(actor);
					HandlePickupPickUp();
					return;
				}


				if (actor->ActorHasTag("Player"))
				{
					ABasePlayer* otherPlayer = Cast<ABasePlayer>(actor);

					//if it is a BasePlayer and they are downed but not dead we can start reviving them
					if (otherPlayer && otherPlayer->GetIsPlayerDowned() && !otherPlayer->GetIsPlayerDead())
					{
						otherPlayer->StartRevive();
						pPlayerSoundComponent->PlayReviving();
					}
				}

				else if (actor->ActorHasTag("Door"))
				{
					if (AShipDoor* door = Cast<AShipDoor>(actor))
					{
						door->StartFixingDoor();
						IgnorePlayerInput(true);
					}
				}

				else if (actor->ActorHasTag("Ammo Bay"))
				{
					AAmmoBay* ammo = Cast<AAmmoBay>(actor);
					if (ammo != nullptr)
					{
						//Get Gun from inventory
						AGunBase* gun = pInventoryComponent->GetGun();
						//Pass in Gun to the ammo box
						ammo->refillAmmo(gun);
					}
				}


				else if (actor->ActorHasTag("Medic Bay"))
				{
					AMedicBay* medic = Cast<AMedicBay>(actor);
					if (medic != nullptr)
					{
						medic->StartHeal(this);
						//We are interacting so stop looking
						IgnorePlayerInput(true);
					}
				}

				else if (actor->ActorHasTag("Shop"))
				{
					AShopItem* shop = Cast<AShopItem>(actor);
					if (shop != nullptr)
					{
						pPlayerAnimComponent->PlayEventInteractionAnimation();
						shop->Deactivate();
					}
				}
			}
		}
	}
}

void ABasePlayer::StopInteraction()
{
	if (pWidgetInteractionComponent->IsOverInteractableWidget())
	{
		pWidgetInteractionComponent->ReleasePointerKey(FKey("LeftMouseButton"));
		return;
	}

	Server_StopInteraction();
}

void ABasePlayer::Server_StopInteraction_Implementation()
{
	//Return input to the player - Due to Delay the player might not be looking directly at the object
	//Placing this here garauntees that the player regains control of their player
	IgnorePlayerInput(false);

	FHitResult hit;
	if (pRaycastComponent->RaycastSingleFromPlayer(hit, playerInformation.fPlayerInteractDistance, ECollisionChannel::ECC_MAX, false))
	{
		if (hit.GetActor())
		{
			if (hit.GetActor()->ActorHasTag("Medic Bay"))
			{
				AMedicBay* medic = Cast<AMedicBay>(hit.GetActor());
				if (medic)
				{
					medic->StopHealing();
				}
			}
			else if (hit.GetActor()->ActorHasTag("Door"))
			{
				if (AShipDoor* door = Cast<AShipDoor>(hit.GetActor()))
				{
					door->CancelFixingDoor();
				}
			}
		}
	}
}


void ABasePlayer::HandlePickupPickUp()
{
	print("Pick Up")

	StopFire();
	StopAltFire();

	DisableInventory();

	pPickupItem->PickUpObject();

	pPickupItem->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("Marine002RPalm"));

	if (pPickupItem->ActorHasTag("Janitor")) //Justin B added
	{
		pPickupItem->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			FName("JanitorSocket"));
	}

	pPickupItem->SetOwner(this);

	AddToPlayerStatusFlag(EPlayerStatus::HoldingPickup);
}

void ABasePlayer::HandlePickupSetDown()
{
	print("Set down");

	if (pPickupItem != nullptr)
	{
		pPickupItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		pPickupItem->SetOwner(nullptr);

		pPickupItem->DropObject(GetActorForwardVector());

		pPickupItem = nullptr;
	}

	RemoveFromPlayerStatusFlag(EPlayerStatus::HoldingPickup);

	EnableInventory();
}

void ABasePlayer::ClassSpecial()
{
	if (!GetCanUseSpecial() || GetIsPlayerDowned() || GetIsPlayerDead())
		return; 


	if (UseSpecial())
	{
		RemoveFromPlayerStatusFlag(EPlayerStatus::CanUseSpecial);
		AddToPlayerStatusFlag(EPlayerStatus::SpecialIsActive);
		GetWorldTimerManager().SetTimer(TimerHandle_ActiveAbilityEnd, this, &ABasePlayer::TimerEndSpecial, playerInformation.fActiveAbilityLength, false);
		
		if (pPlayerSoundComponent != nullptr)
		{
			pPlayerSoundComponent->PlayClassAbility();
		}
	}
}


void ABasePlayer::SetCanUseSpecialAgain()
{
	print("Can use special again")
	AddToPlayerStatusFlag(EPlayerStatus::CanUseSpecial);
}


void ABasePlayer::TimerEndSpecial()
{
	print("Special has ended")
	RemoveFromPlayerStatusFlag(EPlayerStatus::SpecialIsActive);

	GetWorldTimerManager().SetTimer(TimerHandle_ActiveAbilityCooldown, this, 
                                    &ABasePlayer::SetCanUseSpecialAgain, 
		                            playerInformation.fActiveAbilityCooldown, false);
}


bool ABasePlayer::UseSpecial()
{
	return true;
}


void ABasePlayer::IgnorePlayerInput_Implementation(bool bNewIgnore)
{
	if (GetController() != nullptr)
	{
		GetController()->SetIgnoreLookInput(bNewIgnore);
		GetController()->SetIgnoreMoveInput(bNewIgnore);
	}

	if (bNewIgnore == true)
	{
		AddToPlayerStatusFlag(EPlayerStatus::IsInteracting);
	}
	else
	{
		RemoveFromPlayerStatusFlag(EPlayerStatus::IsInteracting);
	}
}


#undef print


void ABasePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//Relpicate the player flag
	DOREPLIFETIME(ABasePlayer, _PlayerStatusFlag);

	//Replicated pointers* 
	DOREPLIFETIME(ABasePlayer, pActiveWeapon);
	DOREPLIFETIME(ABasePlayer, pPickupItem); 

	//Replicated Components
	DOREPLIFETIME(ABasePlayer, pInventoryComponent);
}
