#pragma once
#include <windows.h>
#include <xinput.h>
#include <thread>
#pragma comment(lib,"xinput.lib")


// Define XInput class
class XInputController {
public:
    XInputController();
    ~XInputController();

    bool IsControllerConnected(DWORD dwUserIndex);
    XINPUT_STATE GetControllerState(DWORD dwUserIndex);

    void StartCapturing(DWORD dwUserIndex);
    void StopCapturing();

private:
    bool capturing_;
    std::thread captureThread_;
    DWORD userIndex_;

    void CaptureInput();
};
