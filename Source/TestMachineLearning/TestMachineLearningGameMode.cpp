// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMachineLearningGameMode.h"
#include "TestMachineLearningPlayerController.h"

ATestMachineLearningGameMode::ATestMachineLearningGameMode()
{
	PlayerControllerClass = ATestMachineLearningPlayerController::StaticClass();
}
