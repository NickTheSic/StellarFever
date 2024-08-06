/* Document initial creation by Nick Lemmon*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerInformation.h"
#include "SF_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ASF_PlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	ASF_PlayerState();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EPlayerClass PlayerClass = EPlayerClass::Base; //A Default initial value

	UPROPERTY(replicated)
		class UMessageQueue* pMessageQueue = nullptr;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
