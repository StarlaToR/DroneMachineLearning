// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DroneController.generated.h"

class ADronePawn;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class TESTMACHINELEARNING_API ADroneController : public APlayerController
{
	GENERATED_BODY()

	protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Type of vehicle to automatically respawn when it's destroyed */
	UPROPERTY(EditAnywhere, Category="Vehicle|Respawn")
	TSubclassOf<ADronePawn> DronePawnClass;

	/** Pointer to the controlled vehicle pawn */
	TObjectPtr<ADronePawn> DronePawn;
		
protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input setup */
	virtual void SetupInputComponent() override;

public:

	/** Update vehicle UI on tick */
	virtual void Tick(float Delta) override;

protected:

	/** Pawn setup */
	virtual void OnPossess(APawn* InPawn) override;

	/** Handles pawn destruction and respawning */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedPawn);
};
