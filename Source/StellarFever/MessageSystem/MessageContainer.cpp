// Copyright Quarantine Games 2020


#include "MessageContainer.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameState.h"
#include "MessageQueue.h"
#include "../Player/SF_PlayerState.h"

// Sets default values for this component's properties
UMessageContainer::UMessageContainer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//SetIsReplicated(true);
	// ...
}

void UMessageContainer::Clear()
{
	messages.Empty();
}

void UMessageContainer::CleanupContainer()
{
	//Fill later
}

uint8 UMessageContainer::FindOldestMessageOwner(const TArray<ASF_PlayerState*>& players)
{
	uint8 idx = 0xFF; //Impossible value
	uint32 oldestTimestamp = ~0; //Highest value

	for (uint8 i = 0; i < players.Num(); ++i)
	{
		if (players[i] != nullptr && players[i]->pMessageQueue->IsEmpty())
			continue;

		uint32 time = players[i]->pMessageQueue->PeekMessageTimestamp();
		if (time < oldestTimestamp)
		{
			oldestTimestamp = time;
			idx = i;
		}
	}
	return idx;
}

void UMessageContainer::FetchNewMessages()
{
	TArray<ASF_PlayerState*> players;

	//Get converted player states
	{
		TArray<APlayerState*> _players = GetWorld()->GetGameState()->PlayerArray;
		for (auto& p : _players)
			players.Add(Cast<ASF_PlayerState>(p));
	}

	//Loop through all messages until all queues are emptied
	//0xFF is the value being used for "no message found", as there can not be 255 players
	for (uint8 idx = FindOldestMessageOwner(players); idx != 0xFF; idx = FindOldestMessageOwner(players))
	{
		const AMessage* message = players[idx]->pMessageQueue->Pop();
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "New message:\n" + message->GetFormattedMessage().ToString());
		messages.Add(message);
	}
}


// Called every frame
void UMessageContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FetchNewMessages();
}

void UMessageContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMessageContainer, messages);
}