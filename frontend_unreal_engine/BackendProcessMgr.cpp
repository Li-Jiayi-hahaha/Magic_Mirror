// Fill out your copyright notice in the Description page of Project Settings.

#include "BackendProcessMgr.h"
#include <windows.h>
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/WindowsPlatformProcess.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include <fstream>
#include <sstream>

#include "Userenv.h" // Include the Userenv.h header

#pragma comment(lib, "Userenv.lib") // Link the Userenv.lib library


// Sets default values
ABackendProcessMgr::ABackendProcessMgr()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    pipe = NULL;
}

void ABackendProcessMgr::BeginPlay()
{
    Super::BeginPlay();
    // Connect to the named pipe
    pipe = CreateFile(
        TEXT("\\\\.\\pipe\\com_pipe"),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE) {
        UE_LOG(LogTemp, Error, TEXT("Communication Pipe Connection Failed."));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Communication Pipe Connected!"));
}


void ABackendProcessMgr::setAndStart(int32 cid, FString res_str) {
    camera_id = cid;
    camera_res = TCHAR_TO_UTF8(*res_str);
    std::string combined_str = std::to_string(camera_id) + ':' + camera_res;

    size_t combined_length = combined_str.length();
    char* msg_sent = new char[combined_length + 1];
    std::strcpy(msg_sent, combined_str.c_str());

    if(!sendMsg(msg_sent)) return;

    if(waitMsg()){
        UE_LOG(LogTemp, Warning, TEXT("Backend Launched successfully!"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Launch Backend Process Failed."));
    }
}


void ABackendProcessMgr::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (pipe != INVALID_HANDLE_VALUE) {
        sendMsg("Exit");
        CloseHandle(pipe);
    }

	Super::EndPlay(EndPlayReason);
}





