// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MonitorWidget.h"
#include "MonitorWidget_Medbay.generated.h"

/**
 * 
 */

UCLASS()
class STELLARFEVER_API UMonitorWidget_Medbay : public UMonitorWidget
{
	GENERATED_BODY()
protected:
		virtual void OnMonitorStateChanged_Internal(class AMonitorState* monitor, int32 prevID, int32 newID) override;
		
};