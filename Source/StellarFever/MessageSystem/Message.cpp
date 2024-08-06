// Copyright Quarantine Games 2020


#include "Message.h"
#include "Misc/DateTime.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetTextLibrary.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Internationalization/Regex.h"

AMessage::AMessage()
{
	bReplicates = true;
	//Nulls are easy to see, and the difference in capitalization is to make differentiation easy.
	//As well, a time stamp of 1970-01-01T00:00:01Z would be a dead give away of uninitialized data.
	Init("null", "NULL", 1);
}
void AMessage::Init(FName message, FName sender, uint32 timestamp)
{
	//If the timestamp is 0, which is Jan. 1st 1970, get the current UTC timestamp.
	//FDateTime returns a signed 64 bit integer. If we only take the last 32 bits, it is the same as a unsigned 32 bit integer.
	uTimestamp = timestamp == 0 ? (uint32)FDateTime::UtcNow().ToUnixTimestamp() : timestamp;

	sMessage = message;
	sSenderName = sender;
}

void AMessage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMessage, sMessage);
	DOREPLIFETIME(AMessage, sSenderName);
	DOREPLIFETIME(AMessage, uTimestamp);
}

FName AMessage::GetMessage() const
{
	return sMessage;
}

FName AMessage::GetSenderName() const
{
	return sSenderName;
}

uint32 AMessage::GetUTCTimestamp() const
{
	return uTimestamp;
}

//Format:
//[HH:MM:SS] Sender: This is message.
FName AMessage::GetFormattedMessage() const
{
	auto FixedTime = [](int32 time) -> FString
	{
		if (time < 10)
			return "0" + FString::FromInt(time);
		return FString::FromInt(time);
	};
	//Convert timestamp to corrected timezone time
	//Forgive the mess
	FString str = UKismetTextLibrary::AsTimeZoneDateTime_DateTime(FDateTime::FromUnixTimestamp((int64)uTimestamp), FGenericPlatformMisc::GetTimeZoneId()).ToString();
	//Matches substring for the hours/minutes/seconds
	FRegexPattern pattern(TEXT("(\\d{1,2}:\\d{1,2}:\\d{1,2})"));
	FRegexMatcher matcher(pattern, str);
	//We should always have a match, but just in case it got corrupted somehow
	if (matcher.FindNext())
	{
		FDateTime time = FDateTime::FromUnixTimestamp((int64)uTimestamp);
		return FName("[" + matcher.GetCaptureGroup(0) + "] " + sSenderName.ToString() + ": " + sMessage.ToString());
	}

	//This should never happen
	return "[??:??:??] Error: Error";
}