// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpressionClientA.h"
#include <windows.h>


// Sets default values
AExpressionClientA::AExpressionClientA()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pipe = NULL;
    for (int i = 0; i < 53; i++) expression[i] = 0;
    for (int i = 0; i < 4; i++) headrot[i] = 0;

    ca = *(new CovertToARKitExpression());
    dataMagnifier = NULL;
    connected = false;

}

// Called when the game starts or when spawned
void AExpressionClientA::BeginPlay()
{
	Super::BeginPlay();
    connectToPipe();
}

void AExpressionClientA::connectToPipe() {

    UE_LOG(LogTemp, Warning, TEXT("Connecting to pipe..."));

    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    pipe = CreateFile(
        TEXT("\\\\.\\pipe\\exp_pipe"),
        GENERIC_READ, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {

        UE_LOG(LogTemp, Warning, TEXT("Failed to connect to pipe."));
        // look up error code here using GetLastError()
        //system("pause");
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Connected!"));
        connected = true;
    }

}

void AExpressionClientA::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    CloseHandle(pipe);
    Super::EndPlay(EndPlayReason);
}

// Called every frame
void AExpressionClientA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (pipe == INVALID_HANDLE_VALUE) return;

    count_thread += 1;

    if (count_thread == 1000) {
        UE_LOG(LogTemp, Warning, TEXT("Avg receive/showing rate: %Lf"), (double)count_receive / (double)count_thread);
        count_thread = 0;
        count_receive = 0;
    }

    DWORD bytesRead;
    BOOL result = ReadFile(
        pipe,
        &data_buffer, // the data from the pipe will be put here
        BUFFER_SIZE, // number of bytes allocated
        &bytesRead,
        NULL
    );

    count_receive += bytesRead / sizeof(float) / 57;
    std::copy(data_buffer, data_buffer + 53, expression);
    std::copy(data_buffer+ 53, data_buffer + 57, headrot);

    updateProperties();
    

    return;
}

void AExpressionClientA::updateProperties() {

    if (dataMagnifier != NULL) {
        dataMagnifier->applyMagifier(expression);
        dataMagnifier->applyHeadRotationMagifier(headrot);
    }

    ca.transferArrToMaxineEC(expression);

    A01_Brow_Inner_Up = ca.A[1];
    A02_Brow_Down_Left = ca.A[2];
    A03_Brow_Down_Right = ca.A[3];
    A04_Brow_Outer_Up_Left = ca.A[4];
    A05_Brow_Outer_Up_Right = ca.A[5];
    A06_Eye_Look_Up_Left = ca.A[6];
    A07_Eye_Look_Up_Right = ca.A[7];
    A08_Eye_Look_Down_Left = ca.A[8];
    A09_Eye_Look_Down_Right = ca.A[9];
    A10_Eye_Look_Out_Left = ca.A[10];
    A11_Eye_Look_In_Left = ca.A[11];
    A12_Eye_Look_In_Right = ca.A[12];
    A13_Eye_Look_Out_Right = ca.A[13];
    A14_Eye_Blink_Left = ca.A[14];
    A15_Eye_Blink_Right = ca.A[15];

    
    A16_Eye_Squint_Left = ca.A[16];
    A17_Eye_Squint_Right = ca.A[17];
    A18_Eye_Wide_Left = ca.A[18];
    A19_Eye_Wide_Right = ca.A[19];
    A20_Cheek_Puff = ca.A[20];
    A21_Cheek_Squint_Left = ca.A[21];
    A22_Cheek_Squint_Right = ca.A[22];
    A23_Nose_Sneer_Left = ca.A[23];
    A24_Nose_Sneer_Right = ca.A[24];
    A25_Jaw_Open = ca.A[25];
    A26_Jaw_Forward = ca.A[26];
    A27_Jaw_Left = ca.A[27];
    A28_Jaw_Right = ca.A[28];
    A29_Mouth_Funnel = ca.A[29];
    A30_Mouth_Pucker = ca.A[30];

    A31_Mouth_Left = ca.A[31];
    A32_Mouth_Right = ca.A[32];
    A33_Mouth_Roll_Upper = ca.A[33];
    A34_Mouth_Roll_Lower = ca.A[34];
    A35_Mouth_Shrug_Upper = ca.A[35];
    A36_Mouth_Shrug_Lower = ca.A[36];
    A37_Mouth_Close = ca.A[37];
    A38_Mouth_Smile_Left = ca.A[38];
    A39_Mouth_Smile_Right = ca.A[39];
    A40_Mouth_Frown_Left = ca.A[40];
    A41_Mouth_Frown_Right = ca.A[41];
    A42_Mouth_Dimple_Left = ca.A[42];
    A43_Mouth_Dimple_Right = ca.A[43];
    A44_Mouth_Upper_Up_Left = ca.A[44];
    A45_Mouth_Upper_Up_Right = ca.A[45];

    A46_Mouth_Lower_Down_Left = ca.A[46];
    A47_Mouth_Lower_Down_Right = ca.A[47];
    A48_Mouth_Press_Left = ca.A[48];
    A49_Mouth_Press_Right = ca.A[49];
    A50_Mouth_Stretch_Left = ca.A[50];
    A51_Mouth_Stretch_Right = ca.A[51];
    A52_Tongue_Out = ca.A[52];

    HeadRot_X = headrot[0];
    HeadRot_Y = headrot[1];
    HeadRot_Z = headrot[2];
}

