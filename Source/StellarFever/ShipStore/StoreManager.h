// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoreManager.generated.h"

UCLASS()
class STELLARFEVER_API AStoreManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStoreManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//This is how we are going to set what the player has selected to purchase
	void PurchaseAvailableItems();


	void OpenStoreMenu();
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_OpenStoreMenu();

	void CloseStoreMenu();
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_CloseStoreMenu();

	void SetPurchaseInformation(int idx);
	UFUNCTION(Server, Reliable)
		void Server_SetPurchaseInformation(int idx);

	UPROPERTY(EditAnywhere, Category = "purchasable items")
		TArray<TSubclassOf<class AStoreItem>> Items;

	UPROPERTY(EditAnywhere, Category = "medic mesh")
		UStaticMeshComponent* pMonitorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
		class UStoreMenu* pMenuWidget;

	UPROPERTY()
		class AShipState* pshipstate;

	UPROPERTY(EditAnywhere, Category = "purchasable items")
		int iItemAmount = 3;


private:
	void PurchaseTheItem();
	UFUNCTION(Server, Reliable)
		void Server_PurchaseTheItem();

	/*
* -1  : Hasn't been selected
* -2  : Player Cancelled
* 0++ : The purchased wanted
*/
	UPROPERTY(Replicated)
		int8 iPuchaseInformation = -1;
};
