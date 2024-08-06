// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/RaycastNotifyable.h"
#include "Monitor.generated.h"

UCLASS()
class STELLARFEVER_API AMonitor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonitor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UChildActorComponent* pModel = nullptr;
	UPROPERTY(EditAnywhere, Replicated)
		class UWidgetComponent* pWidget = nullptr;

	UPROPERTY()
		class AMonitorState* pState = nullptr;
	UPROPERTY(EditDefaultsOnly)
		class UBoxComponent* pRangeBox = nullptr; //If players leave this box, return to page 0
	UPROPERTY(EditInstanceOnly)
		bool bSyncShipEvents = false;

	UPROPERTY(Replicated)
		uint16 usEventsState = 0;

	UFUNCTION(NetMulticast, UnReliable)
		void UpdateShipState();

	FTimerHandle ShipStateHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		UWidgetComponent* GetWidget();
	UFUNCTION(BlueprintCallable)
		AMonitorState* GetMonitorState();
	
	UFUNCTION()
	void EndComponentOverlap(UPrimitiveComponent* overlappedComp, AActor* other, UPrimitiveComponent* otherPrimitive, int32 OtherBodyIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const struct FEventsState& GetEventsState() const;

};
