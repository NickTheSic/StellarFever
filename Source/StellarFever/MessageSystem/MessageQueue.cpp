// Copyright Quarantine Games 2020


#include "MessageQueue.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMessageQueue::UMessageQueue()
{
	//Just a container, doesn't need to tick
	PrimaryComponentTick.bCanEverTick = false;
}

void UMessageQueue::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMessageQueue, messages);
}

void UMessageQueue::Push(const AMessage* message)
{
	messages.Add(message);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Pushed message: \"" + message->GetMessage().ToString() + "\"");
}

void UMessageQueue::Push(const FString message, const FString sender, const uint32 timestamp)
{
	AMessage* msg = GetWorld()->SpawnActor<AMessage>();
	msg->Init(FName(message), FName(sender), timestamp);
	Push(msg);
}

const AMessage* UMessageQueue::Pop()
{
	if (!IsEmpty())
	{
		
		const AMessage* ptr = *messages.begin();
		messages.Remove(ptr);
		return ptr;
	}
	return nullptr;
}