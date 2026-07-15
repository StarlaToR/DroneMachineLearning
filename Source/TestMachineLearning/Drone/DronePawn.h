// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "DronePawn.generated.h"

class UInputAction;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

USTRUCT(BlueprintType)
struct FPreceptionData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Distance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsGate;

	FPreceptionData(float InDistance = 5000.f, bool InIsGate = false)
		: Distance(InDistance), IsGate(InIsGate)
	{}
};

UCLASS()
class TESTMACHINELEARNING_API ADronePawn : public APawn
{
	GENERATED_BODY()

	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

public:
	/** Throttle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrottleAction;

	/** Brake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* YawAction;

	/** Handbrake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PitchAction;

	/** Look Around Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* RollAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Parameters")
	FQuat AccelerationDir = FQuat::Identity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Parameters")
	float AccelerationPower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parameters", meta = (AllowPrivateAccess = "true"))
	float DroneMaxPower = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	bool UsePerception = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	bool ShowDebugLines = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	float ConeAngle = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	int CircleCount = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	int FirstCircleRayNb = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	int RayPerCircleAdded = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	float RaySize = 5000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	float RayTickInterval = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	TEnumAsByte<ECollisionChannel> RayTraceChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters|Perception")
	TEnumAsByte<ECollisionChannel> GateTriggerChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Perception")
	TArray<TObjectPtr<AActor>> ActorsToIgnore;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parameters")
	bool HitGround = false;

	ADronePawn();
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;
	
	virtual void Tick(float Delta) override;
	
protected:
	UPROPERTY()
	int hitNumber = 0;
	
	UFUNCTION()
	void Throttle(const FInputActionValue& Value);

	UFUNCTION()
	void Yaw(const FInputActionValue& Value);

	UFUNCTION()
	void Pitch(const FInputActionValue& Value);

	UFUNCTION()
	void Roll(const FInputActionValue& Value);

public:
	/** Handle throttle input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoThrottle(float ThrottleValue);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoYaw(float ThrottleValue);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoPitch(float ThrottleValue);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoRoll(float ThrottleValue);

	UFUNCTION(BlueprintCallable, Category="Collision")
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); 

	UFUNCTION(BlueprintCallable, Category="Learning")
	void ResetForNewIteration();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Learning")
	TArray<float> GetPerceptionData_CPP();

public:
	/** Returns the front spring arm subobject */
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns the front camera subobject */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

private:
	TArray<FVector> PerceptionRayDirections;
	TArray<float> PerceptionData;
	int8 currentTick = 0;

private:
	void InitializePerceptionRayDirections();
	float ShootPerceptionRaycast(FVector RayDirection);
	void UpdatePerceptionData();
};
