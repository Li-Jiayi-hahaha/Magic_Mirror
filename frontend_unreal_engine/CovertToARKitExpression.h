// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataMagnifier.h"



class MYPROJECT_API CovertToARKitExpression
{
public:
	enum Mec {
		browDown_L = 0,
		browDown_R = 1,
		browInnerUp_L = 2,
		browInnerUp_R = 3,
		browOuterUp_L = 4,
		browOuterUp_R = 5,
		cheekPuff_L = 6,
		cheekPuff_R = 7,
		cheekSquint_L = 8,
		cheekSquint_R = 9,
		eyeBlink_L = 10,
		eyeBlink_R = 11,
		eyeLookDown_L = 12,
		eyeLookDown_R = 13,
		eyeLookIn_L = 14,
		eyeLookIn_R = 15,
		eyeLookOut_L = 16,
		eyeLookOut_R = 17,
		eyeLookUp_L = 18,
		eyeLookUp_R = 19,
		eyeSquint_L = 20,
		eyeSquint_R = 21,
		eyeWide_L = 22,
		eyeWide_R = 23,
		jawForward = 24,
		jawLeft = 25,
		jawOpen = 26,
		jawRight = 27,
		mouthClose = 28,
		mouthDimple_L = 29,
		mouthDimple_R = 30,
		mouthFrown_L = 31,
		mouthFrown_R = 32,
		mouthFunnel = 33,
		mouthLeft = 34,
		mouthLowerDown_L = 35,
		mouthLowerDown_R = 36,
		mouthPress_L = 37,
		mouthPress_R = 38,
		mouthPucker = 39,
		mouthRight = 40,
		mouthRollLower = 41,
		mouthRollUpper = 42,
		mouthShrugLower = 43,
		mouthShrugUpper = 44,
		mouthSmile_L = 45,
		mouthSmile_R = 46,
		mouthStretch_L = 47,
		mouthStretch_R = 48,
		mouthUpperUp_L = 49,
		mouthUpperUp_R = 50,
		noseSneer_L = 51,
		noseSneer_R = 52
	};

public:
	CovertToARKitExpression();
	~CovertToARKitExpression();
	float A[53] = { 0 };

	void transferArrToMaxineEC(float M[53]);
};
