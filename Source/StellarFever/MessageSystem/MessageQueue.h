// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Message.h"
#include "MessageQueue.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UMessageQueue : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMessageQueue();

protected:
	UPROPERTY(replicated)
		TArray<const AMessage*> messages;
public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE bool IsEmpty() const { return messages.Num() == 0; }

	//Appends a message to the queue
	void Push(const AMessage* message);
	//Appends a message to the queue
	void Push(const FString message, const FString sender, const uint32 timestamp = 0);

	//Grab the oldest message from the queue and removes it from the queue
	const AMessage* Pop();

	//Returns a pointer to the oldest message, without removing it
	FORCEINLINE const AMessage* const PeekMessage() const { return IsEmpty() ? nullptr : *messages.begin(); }

	//Returns the timestamp of the oldest message in seconds sinch epoch, or 0 (1970-01-01T00:00:00Z) if none is found
	//Please note, however, that if a valid message is found, but is uninitialized, the timestamp
	//Will be 1 (1970-00-00T00:00:01Z), and not 0.
	FORCEINLINE uint32 PeekMessageTimestamp() const { return IsEmpty() ? 0 : (*messages.begin())->GetUTCTimestamp(); }
};
