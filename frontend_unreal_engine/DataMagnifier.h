// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CovertToARKitExpression.h"
#include "DataMagnifier.generated.h"

UCLASS()
class MYPROJECT_API ADataMagnifier : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float brown_up_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float brown_down_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float eyeball_movement_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float eye_blink_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float eye_squint_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float eye_wide_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mouth_two_sides_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mouth_middle_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float jaw_movement_mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float head_rotation_mag;

	// Sets default values for this actor's properties
	ADataMagnifier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void magnifyOneValue(float& value, float mag);

public:	

	void applyMagifier(float* M);
	void applyHeadRotationMagifier(float* M);
	

};
