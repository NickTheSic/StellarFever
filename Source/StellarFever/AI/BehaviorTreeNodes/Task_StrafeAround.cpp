// Copyright Quarantine Games 2020


#include "Task_StrafeAround.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Vector.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


UTask_StrafeAround::UTask_StrafeAround(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Strafe Around C++");
}

EBTNodeResult::Type UTask_StrafeAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(controller->GetPawn());

	//ACharacter* character = Cast<ACharacter>(AIPawn); AIPawn ignore
	//character->GetCapsuleComponent()->GetOwner();

	//TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	//AIPawn->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
	//AIPawn
	//SkeletalMeshComps = AIPawn->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	//if (SkeletalMeshComps.Num() > 0)
	//{
	//	pMeshComp = SkeletalMeshComps[0];
	//}
	pMeshComp = AIPawn->FindComponentByClass<USkeletalMeshComponent>();

	int random = FMath::RandRange(0, 1);
	if (random == 0) //left
	{
		LeftOrRightLane = AIPawn->GetActorRightVector() * Distance;
	}
	if (random == 1) //right
	{
		//LeftOrRightLane = AIPawn->GetActorRightVector() * Distance;
		LeftOrRightLane = AIPawn->GetActorRightVector() * Distance * -1;
	}

	FHitResult Hit;
	FVector Start = pMeshComp->GetComponentLocation();
	FVector End = pMeshComp->GetComponentLocation() + LeftOrRightLane;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(AIPawn);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 1.f);
	bool Success = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, TraceParams);

	if (Success)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, Hit.Location); //move to line

	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, End); //move to completely
	}

	//UKismetSystemLibrary::Delay(GetWorld(), 3.2f, )
	//FTimerHandle TimerHandle_Delay;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Delay, this, UTask_StrafeAround::EndTaskDelay, 3.2f, false);

	//FTimerDelegate TimerDelegate_Delay;
	//TimerDelegate_Delay.BindUFunction(this, FName("EndTaskDelay"), &OwnerComp);

	//FTimerHandle TimerHandle_Delay;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Delay, TimerDelegate_Delay, 3.2f, false); //0.5 = twice a second

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

//EBTNodeResult::Type UTask_StrafeAround::EndTaskDelay(UBehaviorTreeComponent& OwnerComp)
//{
//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
//	return EBTNodeResult::Succeeded;
//}
