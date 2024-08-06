// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/


#include "TeleportActorComponent.h"
#include "../Player/BasePlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UTeleportActorComponent::UTeleportActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	pTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	pTimeline->SetLooping(false);
	pTimeline->SetIgnoreTimeDilation(false);


	InterpFunction.BindUFunction(this, FName("TimelineUpdate"));
	TimelineFinished.BindUFunction(this, FName("TeleportOnTimelineFinished"));


	//Setup Initial values for the values that we want to lerp to and from
	ZAmountMin = 210.0f;
	ZAmountMax = -70.0f;

	DisplacementMin = 0.0f;
	DisplacementMax = 400.0f;

	NoiseMin = 30.0f;
	NoiseMax = 110.0f;

	NumberTilingMin = 0.0f;
	NumberTilingMax = 1.0f;

	TransitionMin = 0.0f;
	TransitionMax = 360.0f;

	GlowColorStart = FVector(0, 0, 0);
	GlowColorEnd = FVector(0.3,0.4,0.8);

	bWantsToTeleport = false;

	CameraJumpLoc = FVector(-200, 0, +24);
}


// Called when the game starts
void UTeleportActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (pTimeCurve != nullptr)
	{
		pTimeline->AddInterpFloat(pTimeCurve, InterpFunction);
	}


	pTimeline->SetTimelineFinishedFunc(TimelineFinished);
}


void UTeleportActorComponent::StartTeleporting(const FVector& Loc, const FRotator& Rot)
{
	TeleportLocation = Loc;
	TeleportRotation = Rot;
	bWantsToTeleport = true;

	ClientMoveCameraForTeleport();
	NetMulticastPlay();
}

void UTeleportActorComponent::ClientMoveCameraForTeleport_Implementation()
{
	if (ABasePlayer* player = Cast<ABasePlayer>(GetOwner()))
	{
		player->IgnorePlayerInput(true);
		player->GetMesh()->SetOwnerNoSee(false);
		player->DisableInventory();

		//Move the camera so we can see the player
		player->GetCamera()->SetRelativeLocation(player->StandingCamera + CameraJumpLoc);
		player->GetCamera()->SetWorldRotation(FVector(0, 0, 0).Rotation());
	}
}

void UTeleportActorComponent::ClientReturnCameraToNormal_Implementation()
{
	if (ABasePlayer* player = Cast<ABasePlayer>(GetOwner()))
	{
		player->IgnorePlayerInput(false);
		player->GetMesh()->SetOwnerNoSee(true);
		player->EnableInventory();

		player->GetCamera()->SetRelativeLocation(player->StandingCamera);
	}
}

void UTeleportActorComponent::NetMulticastPlay_Implementation()
{
	if(pTimeline != nullptr)
		pTimeline->PlayFromStart();
}

void UTeleportActorComponent::NetMulticastReverse_Implementation()
{
	if(pTimeline != nullptr)
		pTimeline->ReverseFromEnd();
}

void UTeleportActorComponent::TimelineUpdate(float value)
{
	if (pShaderParameters)
	{
		//Lerp the values to pass in
		float zAmount = FMath::Lerp<float>(ZAmountMin, ZAmountMax, value);
		UKismetMaterialLibrary::SetScalarParameterValue(this, pShaderParameters, FName("Z amount"), zAmount);


		float displacement = FMath::Lerp<float>(DisplacementMin, DisplacementMax, value);
		UKismetMaterialLibrary::SetScalarParameterValue(this, pShaderParameters, FName("Displacement amount"), displacement);


		float noise = FMath::Lerp<float>(NoiseMin, NoiseMax, value);
		UKismetMaterialLibrary::SetScalarParameterValue(this, pShaderParameters, FName("Noise tiling"), noise);


		float numberTiling = 0.0f;
		if(value < 0.5f)
			numberTiling = FMath::Lerp<float>(NumberTilingMin, NumberTilingMax, value);
		else 
			numberTiling = FMath::Lerp<float>(NumberTilingMax, NumberTilingMin, value);
		UKismetMaterialLibrary::SetScalarParameterValue(this, pShaderParameters, FName("Number Tiling"), numberTiling);


		float transition = FMath::Lerp<float>(TransitionMin, TransitionMax, value);
		UKismetMaterialLibrary::SetScalarParameterValue(this, pShaderParameters, FName("Transition"), transition);


		FVector glowColor = FMath::Lerp<FVector>(GlowColorStart, GlowColorEnd, value);
		UKismetMaterialLibrary::SetVectorParameterValue(this, pShaderParameters, FName("GlowColour"), glowColor);
	}
}

void UTeleportActorComponent::TeleportOnTimelineFinished()
{
	if (bWantsToTeleport)	//We are teleporting and this is the first time we have hit here
	{
		GLog->Log("Finished timeline Play - Teleporting player");

		if (!GetOwner()->TeleportTo(TeleportLocation, TeleportRotation))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to teleport - location obstructed"));
		}

		NetMulticastReverse();

		bWantsToTeleport = false;
	}

	else	//We have already teleported and the effect has reversed
	{
		GLog->Log("Finished timeline Reverse - Other stuff");

		ClientReturnCameraToNormal();


		//If we were on the enemy ship before we teleported, there is a good chance that we are no longer on the enemy ship
		if(ABasePlayer* player = Cast<ABasePlayer>(GetOwner()))
		{
			if (player->GetIsOnEnemyShip())
			{
				player->RemoveFromPlayerStatusFlag(EPlayerStatus::OnEnemyShip);
			}
		}

	}
}


void UTeleportActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTeleportActorComponent, bWantsToTeleport);
	DOREPLIFETIME(UTeleportActorComponent, TeleportLocation);
	DOREPLIFETIME(UTeleportActorComponent, TeleportRotation);
}

