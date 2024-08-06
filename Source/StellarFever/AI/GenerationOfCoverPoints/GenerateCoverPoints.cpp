// Copyright Quarantine Games 2020


#include "GenerateCoverPoints.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GeneratedCodeHelpers.h"
#include "..\..\Player\BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavFilters/RecastFilter_UseDefaultArea.h"

UGenerateCoverPoints::UGenerateCoverPoints(FObjectInitializer const& object_initializer)
{
	Radius = 1000.0f;
	LocationSet = TArray<FVector>();
	GeneratedItemType = UEnvQueryItemType_Point::StaticClass();
	VerNum = 1;
}

void UGenerateCoverPoints::DoItemGeneration1(TArray<FVector> const& ContextLocations)
{
	const_cast<UGenerateCoverPoints*>(this)->DoItemGeneration2(ContextLocations);
}

void UGenerateCoverPoints::DoItemGeneration2(TArray<FVector> const& ContextLocations)
{
	/*bool bLessEqual;
	int CurrentState = 1;

	FCustomThunkTemplates::Array_Clear(LocationSet);


	int temp = 0;

	bLessEqual = UKismetMathLibrary::LessEqual_IntInt(temp, 35);
	if (!bLessEqual)
	{
		__CurrentState = 12;
		break;
	}*/

	typedef TArray<FVector>  T__Local__0;
	T__Local__0& bpp__ContextLocations__pf = *const_cast<T__Local__0*>(&ContextLocations);
	int32 bpfv__Temp_int_Variable__pf{};
	float bpfv__CallFunc_Multiply_IntFloat_ReturnValue__pf{};
	FRotator bpfv__CallFunc_MakeRotator_ReturnValue__pf(EForceInit::ForceInit);
	bool bpfv__CallFunc_LessEqual_IntInt_ReturnValue__pf{};
	FVector bpfv__CallFunc_GreaterGreater_VectorRotator_ReturnValue__pf(EForceInit::ForceInit);
	int32 bpfv__CallFunc_Add_IntInt_ReturnValue__pf{};
	TArray<ABasePlayer*> bpfv__CallFunc_GetAllActorsOfClass_OutActors__pf{};
	int32 bpfv__CallFunc_Array_Length_ReturnValue__pf{};
	TArray<AActor*> bpfv__Temp_object_Variable__pf{};
	int32 bpfv__Temp_int_Array_Index_Variable__pf{};
	int32 bpfv__Temp_int_Loop_Counter_Variable__pf{};
	int32 bpfv__CallFunc_Add_IntInt_ReturnValue_1__pf{};
	int32 bpfv__Temp_int_Variable_1__pf{};
	ACharacter* bpfv__CallFunc_GetPlayerCharacter_ReturnValue__pf{};
	FVector bpfv__CallFunc_Array_Get_Item__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_K2_GetActorLocation_ReturnValue__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_K2_ProjectPointToNavigation_ProjectedLocation__pf(EForceInit::ForceInit);
	bool bpfv__CallFunc_K2_ProjectPointToNavigation_ReturnValue__pf{};
	int32 bpfv__CallFunc_Array_Length_ReturnValue_1__pf{};
	bool bpfv__CallFunc_Less_IntInt_ReturnValue__pf{};
	bool bpfv__CallFunc_LessEqual_IntInt_ReturnValue_1__pf{};
	int32 bpfv__CallFunc_Add_IntInt_ReturnValue_2__pf{};
	FVector bpfv__CallFunc_Multiply_VectorFloat_ReturnValue__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_Add_VectorVector_ReturnValue__pf(EForceInit::ForceInit);
	FHitResult bpfv__CallFunc_LineTraceSingle_OutHit__pf{};
	bool bpfv__CallFunc_LineTraceSingle_ReturnValue__pf{};
	bool bpfv__CallFunc_BreakHitResult_bBlockingHit__pf{};
	bool bpfv__CallFunc_BreakHitResult_bInitialOverlap__pf{};
	float bpfv__CallFunc_BreakHitResult_Time__pf{};
	float bpfv__CallFunc_BreakHitResult_Distance__pf{};
	FVector bpfv__CallFunc_BreakHitResult_Location__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_BreakHitResult_ImpactPoint__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_BreakHitResult_Normal__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_BreakHitResult_ImpactNormal__pf(EForceInit::ForceInit);
	UPhysicalMaterial* bpfv__CallFunc_BreakHitResult_PhysMat__pf{};
	AActor* bpfv__CallFunc_BreakHitResult_HitActor__pf{};
	UPrimitiveComponent* bpfv__CallFunc_BreakHitResult_HitComponent__pf{};
	FName bpfv__CallFunc_BreakHitResult_HitBoneName__pf{};
	int32 bpfv__CallFunc_BreakHitResult_HitItem__pf{};
	int32 bpfv__CallFunc_BreakHitResult_FaceIndex__pf{};
	FVector bpfv__CallFunc_BreakHitResult_TraceStart__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_BreakHitResult_TraceEnd__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_Multiply_VectorFloat_ReturnValue_1__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_Add_VectorVector_ReturnValue_1__pf(EForceInit::ForceInit);
	FVector bpfv__CallFunc_Add_VectorVector_ReturnValue_2__pf(EForceInit::ForceInit);
	int32 bpfv__CallFunc_Array_Add_ReturnValue__pf{};
	TArray< int32, TInlineAllocator<8> > __StateStack;

	int32 __CurrentState = 1;
	do
	{
		switch (__CurrentState)
		{
		case 1:
		{
			FCustomThunkTemplates::Array_Clear(LocationSet);
		}
		case 2:
		{
			bpfv__Temp_int_Variable__pf = 0;
		}
		case 3:
		{
			bpfv__CallFunc_LessEqual_IntInt_ReturnValue__pf = UKismetMathLibrary::LessEqual_IntInt(bpfv__Temp_int_Variable__pf, 35);
			if (!bpfv__CallFunc_LessEqual_IntInt_ReturnValue__pf)
			{
				__CurrentState = 12;
				break;
			}
		}
		case 4:
		{
			__StateStack.Push(19);
		}
		case 5:
		{
			(bpfv__CallFunc_GetAllActorsOfClass_OutActors__pf).Reset();
			UGameplayStatics::GetAllActorsOfClass(this, ABasePlayer::StaticClass(), /*out*/ TArrayCaster<ABasePlayer*>(bpfv__CallFunc_GetAllActorsOfClass_OutActors__pf).Get<AActor*>());
		}
		case 6:
		{
			bpfv__Temp_int_Variable_1__pf = 0;
		}
		case 7:
		{
			bpfv__CallFunc_Array_Length_ReturnValue__pf = FCustomThunkTemplates::Array_Length(bpfv__CallFunc_GetAllActorsOfClass_OutActors__pf);
			bpfv__CallFunc_LessEqual_IntInt_ReturnValue_1__pf = UKismetMathLibrary::LessEqual_IntInt(bpfv__Temp_int_Variable_1__pf, bpfv__CallFunc_Array_Length_ReturnValue__pf);
			if (!bpfv__CallFunc_LessEqual_IntInt_ReturnValue_1__pf)
			{
				__CurrentState = (__StateStack.Num() > 0) ? __StateStack.Pop(/*bAllowShrinking=*/ false) : -1;
				break;
			}
		}
		case 8:
		{
			__StateStack.Push(20);
		}
		case 9:
		{
			bpfv__CallFunc_Multiply_IntFloat_ReturnValue__pf = UKismetMathLibrary::Multiply_IntFloat(bpfv__Temp_int_Variable__pf, 10.000000);
			bpfv__CallFunc_MakeRotator_ReturnValue__pf = UKismetMathLibrary::MakeRotator(0.000000, 0.000000, bpfv__CallFunc_Multiply_IntFloat_ReturnValue__pf);
			bpfv__CallFunc_GreaterGreater_VectorRotator_ReturnValue__pf = UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(1.000000, 0.000000, 0.000000), bpfv__CallFunc_MakeRotator_ReturnValue__pf);
			bpfv__CallFunc_GetPlayerCharacter_ReturnValue__pf = UGameplayStatics::GetPlayerCharacter(this, bpfv__Temp_int_Variable_1__pf);
			if (::IsValid(bpfv__CallFunc_GetPlayerCharacter_ReturnValue__pf))
			{
				bpfv__CallFunc_K2_GetActorLocation_ReturnValue__pf = bpfv__CallFunc_GetPlayerCharacter_ReturnValue__pf->AActor::K2_GetActorLocation();
			}
			bpfv__CallFunc_K2_ProjectPointToNavigation_ReturnValue__pf = UNavigationSystemV1::K2_ProjectPointToNavigation(this, bpfv__CallFunc_K2_GetActorLocation_ReturnValue__pf, /*out*/ bpfv__CallFunc_K2_ProjectPointToNavigation_ProjectedLocation__pf, ((ANavigationData*)nullptr), URecastFilter_UseDefaultArea::StaticClass(), FVector(0.000000, 0.000000, 0.000000));
			bpfv__CallFunc_Multiply_VectorFloat_ReturnValue__pf = UKismetMathLibrary::Multiply_VectorFloat(bpfv__CallFunc_GreaterGreater_VectorRotator_ReturnValue__pf, Radius);
			bpfv__CallFunc_Add_VectorVector_ReturnValue__pf = UKismetMathLibrary::Add_VectorVector(bpfv__CallFunc_Multiply_VectorFloat_ReturnValue__pf, bpfv__CallFunc_K2_ProjectPointToNavigation_ProjectedLocation__pf);
			bpfv__CallFunc_LineTraceSingle_ReturnValue__pf = UKismetSystemLibrary::LineTraceSingle(this, bpfv__CallFunc_Add_VectorVector_ReturnValue__pf, bpfv__CallFunc_K2_ProjectPointToNavigation_ProjectedLocation__pf, ETraceTypeQuery::TraceTypeQuery1, false, bpfv__Temp_object_Variable__pf, EDrawDebugTrace::ForOneFrame, /*out*/ bpfv__CallFunc_LineTraceSingle_OutHit__pf, true, FLinearColor(1.000000, 0.000000, 0.000000, 1.000000), FLinearColor(0.000000, 1.000000, 0.000000, 1.000000), 5.000000);
		}
		case 10:
		{
			if (!bpfv__CallFunc_LineTraceSingle_ReturnValue__pf)
			{
				__CurrentState = (__StateStack.Num() > 0) ? __StateStack.Pop(/*bAllowShrinking=*/ false) : -1;
				break;
			}
		}
		case 11:
		{
			UGameplayStatics::BreakHitResult(bpfv__CallFunc_LineTraceSingle_OutHit__pf, /*out*/ bpfv__CallFunc_BreakHitResult_bBlockingHit__pf, /*out*/ bpfv__CallFunc_BreakHitResult_bInitialOverlap__pf, /*out*/ bpfv__CallFunc_BreakHitResult_Time__pf, /*out*/ bpfv__CallFunc_BreakHitResult_Distance__pf, /*out*/ bpfv__CallFunc_BreakHitResult_Location__pf, /*out*/ bpfv__CallFunc_BreakHitResult_ImpactPoint__pf, /*out*/ bpfv__CallFunc_BreakHitResult_Normal__pf, /*out*/ bpfv__CallFunc_BreakHitResult_ImpactNormal__pf, /*out*/ bpfv__CallFunc_BreakHitResult_PhysMat__pf, /*out*/ bpfv__CallFunc_BreakHitResult_HitActor__pf, /*out*/ bpfv__CallFunc_BreakHitResult_HitComponent__pf, /*out*/ bpfv__CallFunc_BreakHitResult_HitBoneName__pf, /*out*/ bpfv__CallFunc_BreakHitResult_HitItem__pf, /*out*/ bpfv__CallFunc_BreakHitResult_FaceIndex__pf, /*out*/ bpfv__CallFunc_BreakHitResult_TraceStart__pf, /*out*/ bpfv__CallFunc_BreakHitResult_TraceEnd__pf);
			
			//bpfv__CallFunc_Multiply_VectorFloat_ReturnValue_1__pf = UKismetMathLibrary::Multiply_VectorFloat(bpfv__CallFunc_BreakHitResult_ImpactNormal__pf, 50.000000);
			bpfv__CallFunc_Multiply_VectorFloat_ReturnValue_1__pf = UKismetMathLibrary::Multiply_VectorFloat(bpfv__CallFunc_BreakHitResult_ImpactNormal__pf, 50.000000);
			bpfv__CallFunc_Add_VectorVector_ReturnValue_1__pf = UKismetMathLibrary::Add_VectorVector(bpfv__CallFunc_BreakHitResult_ImpactPoint__pf, bpfv__CallFunc_Multiply_VectorFloat_ReturnValue_1__pf);
			//bpfv__CallFunc_Add_VectorVector_ReturnValue_2__pf = UKismetMathLibrary::Add_VectorVector(bpfv__CallFunc_Add_VectorVector_ReturnValue_1__pf, FVector(0.000000, 0.000000, 150.000000));
			bpfv__CallFunc_Add_VectorVector_ReturnValue_2__pf = UKismetMathLibrary::Add_VectorVector(bpfv__CallFunc_Add_VectorVector_ReturnValue_1__pf, FVector(0.000000, 0.000000, 150.000000));
			bpfv__CallFunc_Array_Add_ReturnValue__pf = FCustomThunkTemplates::Array_Add(LocationSet, bpfv__CallFunc_Add_VectorVector_ReturnValue_2__pf);
			__CurrentState = (__StateStack.Num() > 0) ? __StateStack.Pop(/*bAllowShrinking=*/ false) : -1;
			break;
		}
		case 12:
		{
			bpfv__Temp_int_Loop_Counter_Variable__pf = 0;
		}
		case 13:
		{
			bpfv__Temp_int_Array_Index_Variable__pf = 0;
		}
		case 14:
		{
			bpfv__CallFunc_Array_Length_ReturnValue_1__pf = FCustomThunkTemplates::Array_Length(LocationSet);
			bpfv__CallFunc_Less_IntInt_ReturnValue__pf = UKismetMathLibrary::Less_IntInt(bpfv__Temp_int_Loop_Counter_Variable__pf, bpfv__CallFunc_Array_Length_ReturnValue_1__pf);
			if (!bpfv__CallFunc_Less_IntInt_ReturnValue__pf)
			{
				__CurrentState = 18;
				break;
			}
		}
		case 15:
		{
			bpfv__Temp_int_Array_Index_Variable__pf = bpfv__Temp_int_Loop_Counter_Variable__pf;
		}
		case 16:
		{
			__StateStack.Push(21);
		}
		case 17:
		{
			FCustomThunkTemplates::Array_Get(LocationSet, bpfv__Temp_int_Array_Index_Variable__pf, /*out*/ bpfv__CallFunc_Array_Get_Item__pf);
			UEnvQueryGenerator_BlueprintBase::AddGeneratedVector(bpfv__CallFunc_Array_Get_Item__pf);
			__CurrentState = (__StateStack.Num() > 0) ? __StateStack.Pop(/*bAllowShrinking=*/ false) : -1;
			break;
		}
		case 18:
		{
			__CurrentState = -1;
			break;
		}
		case 19:
		{
			bpfv__CallFunc_Add_IntInt_ReturnValue__pf = UKismetMathLibrary::Add_IntInt(bpfv__Temp_int_Variable__pf, 1);
			bpfv__Temp_int_Variable__pf = bpfv__CallFunc_Add_IntInt_ReturnValue__pf;
			__CurrentState = 3;
			break;
		}
		case 20:
		{
			bpfv__CallFunc_Add_IntInt_ReturnValue_2__pf = UKismetMathLibrary::Add_IntInt(bpfv__Temp_int_Variable_1__pf, 1);
			bpfv__Temp_int_Variable_1__pf = bpfv__CallFunc_Add_IntInt_ReturnValue_2__pf;
			__CurrentState = 7;
			break;
		}
		case 21:
		{
			bpfv__CallFunc_Add_IntInt_ReturnValue_1__pf = UKismetMathLibrary::Add_IntInt(bpfv__Temp_int_Loop_Counter_Variable__pf, 1);
			bpfv__Temp_int_Loop_Counter_Variable__pf = bpfv__CallFunc_Add_IntInt_ReturnValue_1__pf;
			__CurrentState = 14;
			break;
		}
		default:
			check(false); // Invalid state
			break;
		}
	} while (__CurrentState != -1);
}
