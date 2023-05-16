#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <TlHelp32.h>

#include <fstream>
#include <sstream>

class BackendProcessMgr {
private:
    PROCESS_INFORMATION pi;

    bool modifyRunBatFile(std::filesystem::path batchFilePath, int camera_id, std::string camera_res)
    {
        std::ifstream inputFile(batchFilePath);
        std::ostringstream modifiedContent;

        std::string last_line = "ExpressionApp.exe --cam_id=" + std::to_string(camera_id) + " --cam_res=" + camera_res;

        if (inputFile)
        {
            std::string line;
            while (std::getline(inputFile, line))
            {
                if (line.find("ExpressionApp.exe") != std::string::npos)
                {
                    line = last_line;
                }
                modifiedContent << line << std::endl;
            }
            inputFile.close();
        }
        else
        {
            std::cerr << "Failed to open the run.bat file" << std::endl;
            return false;
        }

        std::ofstream outputFile(batchFilePath);
        if (outputFile)
        {
            outputFile << modifiedContent.str();
            outputFile.close();
            std::cout << "run.bat file modified successfully" << std::endl;
        }
        else
        {
            std::cerr << "Failed to write the modified content to run.bat" << std::endl;
            return false;
        }
        return true;
    }

public:
    BackendProcessMgr() {
        ZeroMemory(&pi, sizeof(pi));
    }

    bool createProcess(std::filesystem::path batchFilePath, int camera_id, std::string camera_res) {

        if (!modifyRunBatFile(batchFilePath, camera_id, camera_res)) return false;

        // Create a STARTUPINFO structure
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Hide the console window

        std::filesystem::path p = batchFilePath.parent_path();
        std::wstring workingDir = p.wstring();

        // Create the process for the batch file
        if (!CreateProcess(
            batchFilePath.c_str(),
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE, // No console window
            NULL,
            workingDir.c_str(), // Set the working directory
            &si,
            &pi)) {
            std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
            return false;
        }
        std::cout << "Backend Process Created!" << std::endl;
        return true;
    }

    void terminateProcess() {
        // Terminate ExpressionApp.exe process
        // First, we find the process using its name
        DWORD expressionAppProcessId;
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (Process32First(snapshot, &entry)) {
            while (Process32Next(snapshot, &entry)) {
                if (_wcsicmp(entry.szExeFile, L"ExpressionApp.exe") == 0) {
                    expressionAppProcessId = entry.th32ProcessID;
                    break;
                }
            }
        }
        CloseHandle(snapshot);

        HANDLE expressionAppProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, expressionAppProcessId);
        if (expressionAppProcessHandle) {
            TerminateProcess(expressionAppProcessHandle, 0);
            CloseHandle(expressionAppProcessHandle);
        }

        // Terminate the batch file process
        TerminateProcess(pi.hProcess, 0);

        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
};

class CommunicationMgr {
private:
    HANDLE hNamedPipe = NULL;
    
public:
    CommunicationMgr() {
        hNamedPipe = NULL;
    }

    bool createPipe() {
        hNamedPipe = CreateNamedPipe(
            TEXT("\\\\.\\pipe\\com_pipe"),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            0,
            0,
            NMPWAIT_USE_DEFAULT_WAIT,
            NULL);

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;
            return false;
        }
        std::cout << "Communication Pipe Created!" << std::endl;
        return true;
    }

    bool connectPipe() {
        if (!ConnectNamedPipe(hNamedPipe, NULL)) {
            std::cerr << "ConnectNamedPipe failed: " << GetLastError() << std::endl;
            CloseHandle(hNamedPipe);
            return false;
        }
        std::cout << "Communication Pipe Connected!" << std::endl;
        return true;
    }

    std::string waitMsg() {
        char info[1024];
        DWORD bytesRead;
        if (!ReadFile(hNamedPipe, info, sizeof(info), &bytesRead, NULL)) {
            std::cerr << "ReadFile failed: " << GetLastError() << std::endl;
            CloseHandle(hNamedPipe);
            return "";
        }
        std::string msg(info);
        std::cout << "Received information from UE: " << msg << std::endl;
        
        return msg;
    }

    bool sendMsg(char* msg) {
        DWORD bytesWritten;
        if (!WriteFile(hNamedPipe, msg, strlen(msg) + 1, &bytesWritten, NULL)) {
            std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
            CloseHandle(hNamedPipe);
            return false;
        }
        std::cout << "Sent message to UE: " << msg << std::endl;
        return true;
    }

    void closePipe() {
        CloseHandle(hNamedPipe);
    }

};

class UEProjectMgr {
    PROCESS_INFORMATION processInfo;

public:
    UEProjectMgr() {
        ZeroMemory(&processInfo, sizeof(processInfo));
    }

    bool createProcess(std::filesystem::path ueExePath) {

        STARTUPINFO startupInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        if (!CreateProcess(
                            ueExePath.c_str(),
                            NULL, 
                            NULL, 
                            NULL, 
                            FALSE,
                            0, NULL, 
                            ueExePath.parent_path().wstring().c_str(),
                            &startupInfo, 
                            &processInfo)) {
            std::cerr << "Create UE Process failed: " << GetLastError() << std::endl;
            return false;
        }

        std::cout << "UE Process Created!" << std::endl;
        return true;
    }

    void terminateProcess() {
        // Close process and thread handles
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::filesystem::path currentPath = std::filesystem::current_path();

    //for testing
    //std::filesystem::path rootPath = currentPath.parent_path();
    //for build
    std::filesystem::path rootPath = currentPath;

    std::filesystem::path ueExePath = rootPath / "Windows\\MyProject.exe";

    //TODO: add launch UE here
    UEProjectMgr ueMgr;
    ueMgr.createProcess(ueExePath);

    CommunicationMgr comMgr;
    if(!comMgr.createPipe()) return 0;
    if(!comMgr.connectPipe()) return 0;

    std::string input_str = comMgr.waitMsg();
    if (input_str.size()==0) return 0;
    
    std::cout << "str: " << input_str << std::endl;
    size_t colon_pos = input_str.find(":");
    int camera_id = 0;
    std::string camera_res = "640x480";
    if (colon_pos != std::string::npos) {
        // Get camera settings
        camera_id = std::stoi(input_str.substr(0, colon_pos));
        camera_res = input_str.substr(colon_pos + 1);
        std::cout << "Camera id: " << camera_id << std::endl;
        std::cout << "Camera resolution: " << camera_res << std::endl;
    }
    else {
        std::cout << "UE project exits before setting the camera" << std::endl;
        comMgr.closePipe();
        ueMgr.terminateProcess();
        return 0;
    }

    std::filesystem::path batchFilePath = rootPath / "MAXINE_AR_SDK\\samples\\ExpressionApp\\run.bat";
    
    BackendProcessMgr backendMgr;
    if (!backendMgr.createProcess(batchFilePath, camera_id, camera_res)) {
        char signal[] = "backend_failed";
        comMgr.sendMsg(signal);
        comMgr.closePipe();
        ueMgr.terminateProcess();
        return 0;
    }

    char signal[] = "backend_launched";
    comMgr.sendMsg(signal);

    //wait for UE project to quit
    comMgr.waitMsg();

    comMgr.closePipe();
    backendMgr.terminateProcess();
    ueMgr.terminateProcess();

    return 0;
}
