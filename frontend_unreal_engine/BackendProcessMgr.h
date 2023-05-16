// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <windows.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackendProcessMgr.generated.h"

UCLASS()
class MYPROJECT_API ABackendProcessMgr : public AActor
{
	GENERATED_BODY()

	int camera_id = 0;
	std::string camera_res = "960x540";
	HANDLE pipe = NULL;

public:	
	// Sets default values for this actor's properties
	ABackendProcessMgr();

	UFUNCTION(BlueprintCallable, Category = "Initialization")
		void setAndStart(int32 cid, FString res_str);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool sendMsg(char* msg) {
		DWORD bytesWritten;
		if (!WriteFile(pipe, msg, strlen(msg) + 1, &bytesWritten, NULL)) {
			std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
			CloseHandle(pipe);
			return false;
		}
		std::cout << "Sent message to UE: " << msg << std::endl;
		return true;
	}

	bool waitMsg() {
		char info[1024];
		DWORD bytesRead;
		if (!ReadFile(pipe, info, sizeof(info), &bytesRead, NULL)) {
			std::cerr << "ReadFile failed: " << GetLastError() << std::endl;
			CloseHandle(pipe);
			return NULL;
		}
		std::cout << "Received information from UE: " << info << std::endl;
		std::string msg_received(info);
		size_t found = msg_received.find("launched");
		if (found != std::string::npos) return true;
		return false;
	}

};
