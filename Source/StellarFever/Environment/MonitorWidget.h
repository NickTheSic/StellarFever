// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonitorWidget.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMonitorWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
		class AMonitorState* pMonitorState;
		FDelegateHandle onMonitorStateChangedHandle;
public:
	UFUNCTION(BlueprintCallable)
		void SetMonitorState(AMonitorState* state);
	UFUNCTION(BlueprintCallable)
		AMonitorState* GetMonitorState();

protected:

	UFUNCTION()
		virtual void OnMonitorStateChanged_Internal(class AMonitorState* monitor, int32 prevID, int32 newID);

	UFUNCTION(NetMulticast, Reliable)
		void OnMonitorStateChanged(class AMonitorState* monitor, int32 prevID, int32 newID);
};
