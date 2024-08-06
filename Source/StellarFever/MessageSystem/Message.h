// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Message.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMessage : public AInfo
{
	GENERATED_BODY()
protected:
	UPROPERTY(replicated)
		FName sMessage;
	UPROPERTY(replicated)
		FName sSenderName;
	UPROPERTY(replicated)
		uint32 uTimestamp;
public:
	AMessage();
	//The default value for the timestamp is 0, which is Jan. 1st 1970 @ 0:00 GMT.
	//When it is set to 0, we should grab our timestamp as it is impossible to be this time.
	//FName message: The message;
	//FName sender: The sender's name;
	//uint32 timestamp: Seconds since unix epoch in UTC. Leave empty or 0 to auto set the timestamp to the current time
	void Init(FName message, FName sender, uint32 timestamp = 0);

	//Replicate properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Get message text
	FName GetMessage() const;

	//Get sender's name
	FName GetSenderName() const;

	//Gets the timestamp in seconds since unix epoch (Jan. 1st, 1970 @ 0:00 GMT)
	uint32 GetUTCTimestamp() const;

	//Formats the message for display
	FName GetFormattedMessage() const;
};
