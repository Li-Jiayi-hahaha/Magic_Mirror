// Fill out your copyright notice in the Description page of Project Settings.


#include "CovertToARKitExpression.h"



CovertToARKitExpression::CovertToARKitExpression()
{
}

CovertToARKitExpression::~CovertToARKitExpression()
{
}

void CovertToARKitExpression::transferArrToMaxineEC(float M[53])
{

	A[0] = 0;

	A[1] = 0.5 * (M[(int)browInnerUp_L] + M[(int)browInnerUp_R]);
	A[2] = M[(int)browDown_L];
	A[3] = M[(int)browDown_R];
	A[4] = M[(int)browOuterUp_L];
	A[5] = M[(int)browOuterUp_R];
	A[6] = M[(int)eyeLookUp_L];
	A[7] = M[(int)eyeLookUp_R];
	A[8] = M[(int)eyeLookDown_L];
	A[9] = M[(int)eyeLookDown_R];
	A[10] = M[(int)eyeLookOut_L];
	A[11] = M[(int)eyeLookIn_L];
	A[12] = M[(int)eyeLookIn_R];
	A[13] = M[(int)eyeLookOut_R];
	A[14] = M[(int)eyeBlink_L];
	A[15] = M[(int)eyeBlink_R];
	A[16] = M[(int)eyeSquint_L];
	A[17] = M[(int)eyeSquint_R];
	A[18] = M[(int)eyeWide_L];
	A[19] = M[(int)eyeWide_R];
	A[20] = 0.5 * (M[(int)cheekPuff_L] + M[(int)cheekPuff_R]);
	A[21] = M[(int)cheekSquint_L];
	A[22] = M[(int)cheekSquint_R];
	A[23] = M[(int)noseSneer_L];
	A[24] = M[(int)noseSneer_R];
	A[25] = M[(int)jawOpen];
	A[26] = M[(int)jawForward];
	A[27] = M[(int)jawLeft];
	A[28] = M[(int)jawRight];
	A[29] = M[(int)mouthFunnel];
	A[30] = M[(int)mouthPucker];
	A[31] = M[(int)mouthLeft];
	A[32] = M[(int)mouthRight];
	A[33] = M[(int)mouthRollUpper];
	A[34] = M[(int)mouthRollLower];
	A[35] = M[(int)mouthShrugUpper];
	A[36] = M[(int)mouthShrugLower];
	A[37] = M[(int)mouthClose];
	A[38] = M[(int)mouthSmile_L];
	A[39] = M[(int)mouthSmile_R];
	A[40] = M[(int)mouthFrown_L];
	A[41] = M[(int)mouthFrown_R];
	A[42] = M[(int)mouthDimple_L];
	A[43] = M[(int)mouthDimple_R];
	A[44] = M[(int)mouthUpperUp_L];
	A[45] = M[(int)mouthUpperUp_R];
	A[46] = M[(int)mouthLowerDown_L];
	A[47] = M[(int)mouthLowerDown_R];
	A[48] = M[(int)mouthPress_L];
	A[49] = M[(int)mouthPress_R];
	A[50] = M[(int)mouthStretch_L];
	A[51] = M[(int)mouthStretch_R];
	A[52] = 0;
}
