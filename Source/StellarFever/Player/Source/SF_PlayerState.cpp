// Fill out your copyright notice in the Description page of Project Settings.


#include "../SF_PlayerState.h"
#include "../../MessageSystem/MessageQueue.h"
#include "Net/UnrealNetwork.h"

ASF_PlayerState::ASF_PlayerState()
{
	pMessageQueue = CreateDefaultSubobject<UMessageQueue>("Messages");
	pMessageQueue->SetIsReplicated(true);
}
void ASF_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASF_PlayerState, pMessageQueue);
}