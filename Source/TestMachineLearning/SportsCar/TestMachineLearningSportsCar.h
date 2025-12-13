// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TestMachineLearningPawn.h"
#include "TestMachineLearningSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class ATestMachineLearningSportsCar : public ATestMachineLearningPawn
{
	GENERATED_BODY()
	
public:

	ATestMachineLearningSportsCar();
};
