// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TeleportActorComponent.generated.h"

/**This is more of a wrapper around a UTimelineComponent since I am unable to inherit from a timeline component*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UTeleportActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeleportActorComponent();

	UPROPERTY(EditAnywhere)
		class UTimelineComponent* pTimeline = nullptr;

	UPROPERTY(EditAnywhere)
		class UMaterialParameterCollection* pShaderParameters = nullptr;


	UPROPERTY(EditAnywhere)
		class UCurveFloat* pTimeCurve = nullptr;


	/** Our delegate function which binds to TimlineUpdate*/
	FOnTimelineFloat InterpFunction;
	
	/*a Delegate Bound to our TeleportOnTimelinefinished */
	FOnTimelineEvent TimelineFinished;

	void StartTeleporting(const FVector& Loc, const FRotator& Rot);

	UFUNCTION(Client, Reliable)
		void ClientMoveCameraForTeleport();

	UFUNCTION(Client, Reliable)
		void ClientReturnCameraToNormal();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticastPlay();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticastReverse();

	UFUNCTION()
		void TimelineUpdate(float value);

	UFUNCTION()
		void TeleportOnTimelineFinished();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	FVector TeleportLocation;
	UPROPERTY(Replicated)
	FRotator TeleportRotation;
	UPROPERTY(Replicated)
	bool bWantsToTeleport;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		FVector CameraJumpLoc;


public:	

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float ZAmountMin; 
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float ZAmountMax;

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float DisplacementMin; 
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float DisplacementMax;

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float NoiseMin; 
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float NoiseMax;

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float NumberTilingMin; 
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float NumberTilingMax;

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float TransitionMin; 
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		float TransitionMax;

	UPROPERTY(EditAnywhere, Category = "Shader Config")
		FVector GlowColorStart;
	UPROPERTY(EditAnywhere, Category = "Shader Config")
		FVector GlowColorEnd;
};
