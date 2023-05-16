// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <windows.h>
#include "CovertToARKitExpression.h"
#include "ExpressionClientA.generated.h"


UCLASS()
class MYPROJECT_API AExpressionClientA : public AActor
{
	GENERATED_BODY()

	HANDLE pipe = NULL;
	bool connected = false;
	static const int BUFFER_SIZE = 10 * 57 * sizeof(float);
	
	CovertToARKitExpression ca;
	
	int count_thread = 0;
	int count_receive = 0;
	
	float data_buffer[10 * 57];

public:
	float expression[53];
	float headrot[4];

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A01_Brow_Inner_Up = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A02_Brow_Down_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A03_Brow_Down_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A04_Brow_Outer_Up_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A05_Brow_Outer_Up_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A06_Eye_Look_Up_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A07_Eye_Look_Up_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A08_Eye_Look_Down_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A09_Eye_Look_Down_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A10_Eye_Look_Out_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A11_Eye_Look_In_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A12_Eye_Look_In_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A13_Eye_Look_Out_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A14_Eye_Blink_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A15_Eye_Blink_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A16_Eye_Squint_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A17_Eye_Squint_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A18_Eye_Wide_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A19_Eye_Wide_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A20_Cheek_Puff = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A21_Cheek_Squint_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A22_Cheek_Squint_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A23_Nose_Sneer_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A24_Nose_Sneer_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A25_Jaw_Open = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A26_Jaw_Forward = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A27_Jaw_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A28_Jaw_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A29_Mouth_Funnel = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A30_Mouth_Pucker = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A31_Mouth_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A32_Mouth_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A33_Mouth_Roll_Upper = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A34_Mouth_Roll_Lower = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A35_Mouth_Shrug_Upper = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A36_Mouth_Shrug_Lower = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A37_Mouth_Close = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A38_Mouth_Smile_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A39_Mouth_Smile_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A40_Mouth_Frown_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A41_Mouth_Frown_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A42_Mouth_Dimple_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A43_Mouth_Dimple_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A44_Mouth_Upper_Up_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A45_Mouth_Upper_Up_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A46_Mouth_Lower_Down_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A47_Mouth_Lower_Down_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A48_Mouth_Press_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A49_Mouth_Press_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A50_Mouth_Stretch_Left = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A51_Mouth_Stretch_Right = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float A52_Tongue_Out = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float HeadRot_X = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float HeadRot_Y = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float HeadRot_Z = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ADataMagnifier* dataMagnifier;
	
	// Sets default values for this actor's properties
	AExpressionClientA();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void updateProperties();
	void connectToPipe();

};
