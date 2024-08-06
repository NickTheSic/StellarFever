// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Message.h"
#include "MessageContainer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UMessageContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMessageContainer();

protected:
	//List of messages
	UPROPERTY(replicated)
	TArray<const AMessage*> messages;

	//Deletes the older messages
	void CleanupContainer();

	//Gets the new messages and adds it to the container in order
	void FetchNewMessages();

	//Get oldest message owner
	uint8 FindOldestMessageOwner(const TArray<class ASF_PlayerState*>& players);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	void Clear(); //Empties the container
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
