// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMachineLearningWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UTestMachineLearningWheelFront::UTestMachineLearningWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}