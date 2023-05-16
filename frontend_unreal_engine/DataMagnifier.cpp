// Fill out your copyright notice in the Description page of Project Settings.


#include "DataMagnifier.h"

using Mec = CovertToARKitExpression::Mec;

// Sets default values
ADataMagnifier::ADataMagnifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADataMagnifier::BeginPlay()
{
	Super::BeginPlay();

	brown_up_mag = 1.0;
	brown_down_mag = 1.0;
	eyeball_movement_mag = 1.0;
	eye_blink_mag = 1.0;
	eye_squint_mag = 1.0;
	eye_wide_mag = 1.0;
	mouth_two_sides_mag = 1.0;
	mouth_middle_mag = 1.0;
	jaw_movement_mag = 1.0;
	head_rotation_mag = 1.0;
	
}

void ADataMagnifier::magnifyOneValue(float& value, float mag)
{
	value = 1 - pow(1 - value, mag);
}

void ADataMagnifier::applyMagifier(float* M)
{
	//brown_up
	magnifyOneValue(M[(int)Mec::browInnerUp_L], brown_up_mag);
	magnifyOneValue(M[(int)Mec::browInnerUp_R], brown_up_mag);
	magnifyOneValue(M[(int)Mec::browOuterUp_L], brown_up_mag);
	magnifyOneValue(M[(int)Mec::browOuterUp_R], brown_up_mag);

	//brown_down
	magnifyOneValue(M[(int)Mec::browDown_L], brown_down_mag);
	magnifyOneValue(M[(int)Mec::browDown_R], brown_down_mag);

	//eyeball_movement
	magnifyOneValue(M[(int)Mec::eyeLookDown_L], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookDown_R], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookIn_L], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookIn_R], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookOut_L], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookOut_R], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookUp_L], eyeball_movement_mag);
	magnifyOneValue(M[(int)Mec::eyeLookUp_R], eyeball_movement_mag);

	//eye_blink
	magnifyOneValue(M[(int)Mec::eyeBlink_L], eye_blink_mag);
	magnifyOneValue(M[(int)Mec::eyeBlink_R], eye_blink_mag);

	//eye_squint
	magnifyOneValue(M[(int)Mec::eyeSquint_L], eye_squint_mag);
	magnifyOneValue(M[(int)Mec::eyeSquint_R], eye_squint_mag);

	//eye_wide
	magnifyOneValue(M[(int)Mec::eyeWide_L], eye_wide_mag);
	magnifyOneValue(M[(int)Mec::eyeWide_R], eye_wide_mag);

	//mouth_two_sides
	magnifyOneValue(M[(int)Mec::mouthDimple_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthDimple_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthFrown_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthFrown_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthLeft], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthRight], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthLowerDown_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthLowerDown_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthSmile_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthSmile_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthStretch_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthStretch_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthUpperUp_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::mouthUpperUp_R], mouth_two_sides_mag);

	magnifyOneValue(M[(int)Mec::cheekSquint_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::cheekSquint_R], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::noseSneer_L], mouth_two_sides_mag);
	magnifyOneValue(M[(int)Mec::noseSneer_R], mouth_two_sides_mag);

	//mouth_middle
	magnifyOneValue(M[(int)Mec::mouthFunnel], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthPress_L], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthPress_R], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthPucker], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthRollLower], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthRollUpper], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthShrugLower], mouth_middle_mag);
	magnifyOneValue(M[(int)Mec::mouthShrugUpper], mouth_middle_mag);

	//jaw_movement
	magnifyOneValue(M[(int)Mec::jawForward], jaw_movement_mag);
	magnifyOneValue(M[(int)Mec::jawLeft], jaw_movement_mag);
	magnifyOneValue(M[(int)Mec::jawRight], jaw_movement_mag);
	magnifyOneValue(M[(int)Mec::jawOpen], jaw_movement_mag);

	return;
}

void ADataMagnifier::applyHeadRotationMagifier(float* M) {
	magnifyOneValue(M[0], head_rotation_mag);
	magnifyOneValue(M[1], head_rotation_mag);
	magnifyOneValue(M[2], head_rotation_mag);
}
