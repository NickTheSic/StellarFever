// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MonitorState.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMonitorStateChanged, AMonitorState*, int32, int32);
UCLASS(BlueprintType, Blueprintable)
class STELLARFEVER_API AMonitorState : public AInfo
{
	GENERATED_BODY()
protected:
	int32 iPageID = 0;
public:
	UFUNCTION(BlueprintCallable)
		void SetPage(int32 page);
	UFUNCTION(BlueprintCallable)
		int32 GetPage();

	//void OnMonitorStateChanged(AMonitorState* pMonitorState, int32 prevID, int32 newID);
	FMonitorStateChanged onMonitorStateChanged;
};
