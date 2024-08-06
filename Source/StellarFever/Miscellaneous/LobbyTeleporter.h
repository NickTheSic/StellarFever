// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyTeleporter.generated.h"

UCLASS()
class STELLARFEVER_API ALobbyTeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyTeleporter();

	UPROPERTY(EditAnywhere)
	class UBoxComponent* pTeleportArea = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* pTeleportMesh = nullptr;

	class ALobbyGamemode* pLobbyGamemode = nullptr;

	uint8 PlayersInsideMe = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
