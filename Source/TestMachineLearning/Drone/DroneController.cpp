// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneController.h"

#include "DronePawn.h"
#include "EnhancedInputSubsystems.h"
#include "TestMachineLearningPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ADroneController::BeginPlay()
{
	Super::BeginPlay();
}

void ADroneController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void ADroneController::Tick(float Delta)
{
	Super::Tick(Delta);
}

void ADroneController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	DronePawn = CastChecked<ADronePawn>(InPawn);

	// subscribe to the pawn's OnDestroyed delegate
	DronePawn->OnDestroyed.AddDynamic(this, &ADroneController::OnPawnDestroyed);
}

void ADroneController::OnPawnDestroyed(AActor* DestroyedPawn)
{
	// find the player start
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// spawn a vehicle at the player start
		const FTransform SpawnTransform = ActorList[0]->GetActorTransform();

		if (ATestMachineLearningPawn* RespawnedVehicle = GetWorld()->SpawnActor<ATestMachineLearningPawn>(DronePawnClass, SpawnTransform))
		{
			// possess the vehicle
			Possess(RespawnedVehicle);
		}
	}
}
