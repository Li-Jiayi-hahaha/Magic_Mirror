#include <iostream>
#include <windows.h>
#include <future>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

const int FRAME_SIZE = 57*sizeof(float);
int calculated = 0;
int transfered = 0;

class PipeServer {

private:
    HANDLE pipe = NULL;
    bool connected = false;
    OVERLAPPED overlapped;

public:
    PipeServer() {}

    void createPipe() { 
        // Create a pipe to send data
        pipe = CreateNamedPipe(
            TEXT("\\\\.\\pipe\\exp_pipe"), // name of the pipe
            PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
            PIPE_TYPE_MESSAGE, // send data as a byte stream
            30, // only allow 20 instances of this pipe
            10*57*sizeof(float), // outbound buffer
            10*57*sizeof(float), // inbound buffer
            0, // use default wait time
            NULL // use default security attributes
            );

        if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
            printf("Failed to create outbound pipe instance.");
            return;
        }

        connected = false;
        calculated = 0;
        transfered = 0;
        ZeroMemory(&overlapped, sizeof(OVERLAPPED));

        waitForConnection();            
    } 

    void waitForConnection(){
        // This call blocks until a client process connects to the pipe
        printf("wait for connection from client...\n");

        BOOL result = ConnectNamedPipe(pipe, NULL);
        if (!result) {
            printf("Failed to make connection on named pipe.");
            CloseHandle(pipe); // close the pipe
            return;
        }
        connected = true; 
        printf("The client has connected to the pipe.\n");
    }

    void sendMessage(vector<float> expressions, vector<float> rotations){
        if(!connected) return;

        if(calculated == 1000){
            printf("Total transfer over 1000: %d\n", transfered);
            calculated = 0;
            transfered = 0;
        }
        calculated += 1;

        if(!HasOverlappedIoCompleted(&overlapped)){
            //printf("last IO not completed");
            return;
        }
        
        
        float arr[57];
        std::copy(expressions.begin(), expressions.end(), arr);
        std::copy(rotations.begin(), rotations.end(), arr + 53);
        
        transfered += 1;

        ZeroMemory(&overlapped, sizeof(OVERLAPPED));

        DWORD numBytesWritten = 0;
        BOOL result = WriteFile(
            pipe, // handle to our outbound pipe
            &arr, // data to send
            FRAME_SIZE, // length of data to send (bytes)
            &numBytesWritten, // will store actual amount of data sent
            &overlapped // using overlapped IO
        );
        
    }

    void release(){
        CloseHandle(pipe);
    }
  

};