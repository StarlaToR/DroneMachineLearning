// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMachineLearningWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UTestMachineLearningWheelRear::UTestMachineLearningWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}