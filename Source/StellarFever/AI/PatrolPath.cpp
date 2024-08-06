// Copyright Quarantine Games 2020


#include "PatrolPath.h"

// Sets default values
APatrolPath::APatrolPath()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APatrolPath::BeginPlay()
{
	Super::BeginPlay();

	if (ActorHasTag("SecurePath"))
	{
		//NEW! 03/19/2021
		patrol_points.Add(FVector(-4180.0, 110.0, -510.0));
		patrol_points.Add(FVector(-560.0, 80.0, -280.0));
		patrol_points.Add(FVector(20.0, -1970.0, -250.0));
		patrol_points.Add(FVector(2270.0, -1000.0, -250.0));
		patrol_points.Add(FVector(1890.0, 1060.0, -260.0));
		patrol_points.Add(FVector(6530.0, 50.0, -260.0));
		patrol_points.Add(FVector(4200.0, -910.0, -250.0));
		patrol_points.Add(FVector(4350.0, -2980.0, -260.0));
		patrol_points.Add(FVector(5570.0, -3820.0, -240.0));
		patrol_points.Add(FVector(7430.0, -3270.0, -230.0));
		patrol_points.Add(FVector(4390.0, 1020.0, -240.0));
		patrol_points.Add(FVector(5800.0, 3290.0, -240.0));
		patrol_points.Add(FVector(8860.0, 2380.0, -1060.0));
		patrol_points.Add(FVector(6730.0, 3770.0, -1660.0));
		patrol_points.Add(FVector(8880.0, -2370.0, -1050.0));
		patrol_points.Add(FVector(6550.0, -3770.0, -1670.0));
		patrol_points.Add(FVector(5890.0, -1110.0, -1030.0));
		patrol_points.Add(FVector(5190.0, 1440.0, -1050.0));
		patrol_points.Add(FVector(4910.0, -2650.0, -1030.0));
		patrol_points.Add(FVector(-630.0, -20.0, -1070.0));
		patrol_points.Add(FVector(3700.0, 230.0, -1880.0));
		patrol_points.Add(FVector(2080.0, 30.0, -1880.0));
		patrol_points.Add(FVector(7350.0, 80.0, -3070.0));
		patrol_points.Add(FVector(4080.0, 850.0, -3080.0));
		patrol_points.Add(FVector(4020.0, -1320.0, -3080.0));
		patrol_points.Add(FVector(510.0, 30.0, -1060.0));

	}

	if (ActorHasTag("SecurePathJanitor"))
	{
		//NEW! 03/19/2021
		patrol_points.Add(FVector(-4180.0, 110.0, -510.0));
		patrol_points.Add(FVector(-560.0, 80.0, -280.0));
		patrol_points.Add(FVector(20.0, -1970.0, -250.0));
		patrol_points.Add(FVector(2270.0, -1000.0, -250.0));
		//patrol_points.Add(FVector(1890.0, 1060.0, -260.0)); //Teleportation room (disabled)
		patrol_points.Add(FVector(6530.0, 50.0, -260.0));
		patrol_points.Add(FVector(4200.0, -910.0, -250.0));
		patrol_points.Add(FVector(4350.0, -2980.0, -260.0));
		patrol_points.Add(FVector(5570.0, -3820.0, -240.0));
		patrol_points.Add(FVector(7430.0, -3270.0, -230.0));
		patrol_points.Add(FVector(4390.0, 1020.0, -240.0));
		patrol_points.Add(FVector(5800.0, 3290.0, -240.0));
		patrol_points.Add(FVector(8860.0, 2380.0, -1060.0));
		patrol_points.Add(FVector(6730.0, 3770.0, -1660.0));
		patrol_points.Add(FVector(8880.0, -2370.0, -1050.0));
		patrol_points.Add(FVector(6550.0, -3770.0, -1670.0));
		patrol_points.Add(FVector(5890.0, -1110.0, -1030.0));
		patrol_points.Add(FVector(5190.0, 1440.0, -1050.0));
		patrol_points.Add(FVector(4910.0, -2650.0, -1030.0));
		patrol_points.Add(FVector(-630.0, -20.0, -1070.0));
		patrol_points.Add(FVector(3700.0, 230.0, -1880.0));
		patrol_points.Add(FVector(2080.0, 30.0, -1880.0));
		patrol_points.Add(FVector(7350.0, 80.0, -3070.0));
		patrol_points.Add(FVector(4080.0, 850.0, -3080.0));
		patrol_points.Add(FVector(4020.0, -1320.0, -3080.0));
		patrol_points.Add(FVector(510.0, 30.0, -1060.0));

	}
}

FVector APatrolPath::get_patrol_point(int const index) const
{
	return patrol_points[index];
}

int APatrolPath::num() const
{
	return patrol_points.Num();
}
