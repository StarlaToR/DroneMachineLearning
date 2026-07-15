// Fill out your copyright notice in the Description page of Project Settings.


#include "DronePawn.h"

#include "EnhancedInputComponent.h"
#include "TestMachineLearning.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsProxy/FieldSystemProxyHelper.h"


ADronePawn::ADronePawn()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetSimulatePhysics(true);
	BoxCollider->SetCollisionProfileName(FName("Pawn"));
	BoxCollider->OnComponentHit.AddUniqueDynamic(this, &ADronePawn::OnHit);
	
	// construct the back camera boom
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(BoxCollider);
	SpringArm->TargetArmLength = 650.0f;
	SpringArm->SocketOffset.Z = 150.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 2.0f;
	SpringArm->CameraLagMaxDistance = 50.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ADronePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ADronePawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ADronePawn::Throttle);
		
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &ADronePawn::Yaw);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Completed, this, &ADronePawn::Yaw);
		
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &ADronePawn::Pitch);
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Completed, this, &ADronePawn::Pitch);
		
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ADronePawn::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &ADronePawn::Roll);

		GEngine->AddOnScreenDebugMessage(6, 5, FColor::Red, "Input component binded");
	}
	else
	{
		UE_LOG(LogTestMachineLearning, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADronePawn::BeginPlay()
{
	Super::BeginPlay();

	HitGround = false;

	if (UsePerception)
	{
		InitializePerceptionRayDirections();

		FTimerHandle handle = FTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(
			handle,
			this,
			&ADronePawn::UpdatePerceptionData,
			RayTickInterval,  
			true    
		);

	}
}

void ADronePawn::Tick(float Delta)
{
	Super::Tick(Delta);

	BoxCollider->AddForce(AccelerationDir.RotateVector(FVector::UpVector * AccelerationPower), NAME_None, true);
	AccelerationPower = 0;
	//DrawDebugLine(GetWorld(), GetActorLocation(),
	//	GetActorLocation() + AccelerationDir.RotateVector(FVector::UpVector * 100.f), FColor::Red);
	SetActorRotation(AccelerationDir);
	
	// realign the camera yaw to face front
	float CameraYaw = SpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	SpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ADronePawn::Throttle(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, "Throttle");
	// route the input
	DoThrottle(Value.Get<float>());
}

void ADronePawn::Yaw(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(2, 5, FColor::Red, "Yaw");

	// route the input
	DoYaw(Value.Get<float>());
}

void ADronePawn::Pitch(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(3, 5, FColor::Red, "Pitch");

	// route the input
	DoPitch(Value.Get<float>());
}

void ADronePawn::Roll(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(4, 5, FColor::Red, "Roll");

	// route the input
	DoRoll(Value.Get<float>());
}

void ADronePawn::DoThrottle(float ThrottleValue)
{
	AccelerationPower = FMath::GetMappedRangeValueClamped(
		FVector2f(-1.f, 1.f), FVector2f(0.f, DroneMaxPower), ThrottleValue);
}

void ADronePawn::DoYaw(float YawValue)
{
	AccelerationDir = AccelerationDir * FQuat(GetActorUpVector(), YawValue * PI / 180.f);
}

void ADronePawn::DoPitch(float PitchValue)
{
	AccelerationDir = AccelerationDir * FQuat(GetActorRightVector(), PitchValue * PI / 180.f);
}

void ADronePawn::DoRoll(float RollValue)
{
	AccelerationDir = AccelerationDir * FQuat(GetActorForwardVector(), RollValue * PI / 180.f);
}

void ADronePawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	HitGround = true;
}

void ADronePawn::ResetForNewIteration()
{
	AccelerationDir = FQuat::Identity;
	AccelerationPower = 0;
}

TArray<float> ADronePawn::GetPerceptionData_CPP()
{
	return PerceptionData;
}

void ADronePawn::InitializePerceptionRayDirections()
{
	FVector coneDirection = (FVector::ForwardVector + FVector::UpVector).GetSafeNormal();
	FVector rayDirection = coneDirection;

	PerceptionRayDirections.Add(rayDirection);
	float circleAngle = ConeAngle * 0.5f / CircleCount;

	for (int i = 0; i < CircleCount; i++)
	{
		rayDirection = rayDirection.RotateAngleAxis(circleAngle, FVector::LeftVector);

		int dotCount = i * RayPerCircleAdded + FirstCircleRayNb;
		float dotAngle = 360 / dotCount;

		rayDirection = rayDirection.RotateAngleAxis(dotAngle * 0.5f * i, coneDirection);

		for(int j = 0; j < dotCount; j++)
		{
			PerceptionRayDirections.Add(rayDirection);
			rayDirection = rayDirection.RotateAngleAxis(dotAngle, coneDirection);
		}
	}

	PerceptionData.SetNum(PerceptionRayDirections.Num() + 4);
}

float ADronePawn::ShootPerceptionRaycast(FVector RayDirection)
{
	FHitResult hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActors(ActorsToIgnore); 

	if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), GetActorLocation() + RayDirection * RaySize, RayTraceChannel, Params))
	{
		//if (hit.Component.IsValid())
		//{
		//	bool isGate = hit.Component->GetCollisionObjectType() == GateTriggerChannel;
		//	if (ShowDebugLines)
		//		DrawDebugLine(
		//			GetWorld(),
		//			GetActorLocation(),
		//			GetActorLocation() + RayDirection * hit.Distance,
		//			isGate ? FColor::Green : FColor::Red,
		//			false,
		//			RayTickInterval,
		//			0,
		//			2.0f
		//		);

		//	return(FPreceptionData(hit.Distance, isGate));
		//}
		
		if (ShowDebugLines)
			DrawDebugLine(
				GetWorld(),
				GetActorLocation(),
				GetActorLocation() + RayDirection * hit.Distance,
				FColor::Red,
				false,
				RayTickInterval,
				0,
				2.0f
			);

		return(hit.Distance);
	}

	//if (ShowDebugLines)
	//	DrawDebugLine(
	//		GetWorld(),
	//		GetActorLocation(),
	//		GetActorLocation() + RayDirection * RaySize,
	//		FColor::Red,
	//		false,
	//		RayTickInterval,
	//		0,
	//		2.0f
	//	);

	//return FPreceptionData(RaySize, false);

	if (ShowDebugLines)
		DrawDebugLine(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() + RayDirection * RaySize,
			FColor::Red,
			false,
			RayTickInterval,
			0,
			2.0f
		);

	return(RaySize);
}

void ADronePawn::UpdatePerceptionData()
{
	if (currentTick == 0)
	{
		int num = PerceptionData.Num();
		PerceptionData[0] = ShootPerceptionRaycast(GetActorTransform().TransformVector(PerceptionRayDirections[0]));
		PerceptionData[num - 4] = ShootPerceptionRaycast(GetActorRightVector() * -1);
		PerceptionData[num - 3] = ShootPerceptionRaycast(GetActorRightVector());
		PerceptionData[num - 2] = ShootPerceptionRaycast(FVector::DownVector);
		PerceptionData[num - 1] = ShootPerceptionRaycast(FVector::UpVector);
	}
	else if (currentTick == 1)
	{
		FTransform transform = GetActorTransform();
		for (int i = 1; i < 7; i++)
		{
			PerceptionData[i] = ShootPerceptionRaycast(transform.TransformVector(PerceptionRayDirections[i]));
		}
	}
	else
	{ 
		FTransform transform = GetActorTransform();
		for (int i = 7; i < 13; i++)
		{
			PerceptionData[i] = ShootPerceptionRaycast(transform.TransformVector(PerceptionRayDirections[i]));
		}
	}

	currentTick++;
	if (currentTick > 2)
		currentTick = 0;
}
