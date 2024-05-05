// Check if controller is connected
#include "Gamepad.h"
#include <vector>
#include <fstream>
#include <sstream>

// Constructor
XInputController::XInputController() : capturing_(false) {}

// Destructor
XInputController::~XInputController() {
    StopCapturing();
}

bool XInputController::IsControllerConnected(DWORD dwUserIndex) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // Get the state of the controller
    DWORD result = XInputGetState(dwUserIndex, &state);
    return result == ERROR_SUCCESS;
}

// Get controller state
XINPUT_STATE XInputController::GetControllerState(DWORD dwUserIndex) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // Get the state of the controller
    XInputGetState(dwUserIndex, &state);

    return state;
}

// Start capturing input in a separate thread
void XInputController::StartCapturing(DWORD dwUserIndex) {
    if (!capturing_) {
        capturing_ = true;
        userIndex_ = dwUserIndex;
        captureThread_ = std::thread(&XInputController::CaptureInput, this);
    }
}

// Stop capturing input
void XInputController::StopCapturing() {
    if (capturing_) {
        capturing_ = false;
        if (captureThread_.joinable()) {
            captureThread_.join();
        }
    }
}

// Function to simulate pressing a key on the keyboard
void presskeys(const std::wstring& str)
{
    int len = str.length();
    if (len == 0) return;

    std::vector<INPUT> in(len * 2);
    ZeroMemory(&in[0], in.size() * sizeof(INPUT));

    int i = 0, idx = 0;
    while (i < len)
    {
        WORD ch = (WORD)str[i++];

        if ((ch < 0xD800) || (ch > 0xDFFF))
        {
            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = ch;
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx] = in[idx - 1];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;
        }
        else
        {
            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = ch;
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = (WORD)str[i++];
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx] = in[idx - 2];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;

            in[idx] = in[idx - 2];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;
        }
    }

    SendInput(in.size(), &in[0], sizeof(INPUT));
}

void pressKey(WORD key) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));

    // Release the key
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

// Function to capture input continuously
void XInputController::CaptureInput() {
    std::wstring pin = L"";

    while (capturing_) {
        if (IsControllerConnected(userIndex_)) {
            XINPUT_STATE state = GetControllerState(userIndex_);

            // Example usage of controller state
            if (state.dwPacketNumber != 0) {
                // Example usage of button presses
                for (WORD i = 0; i < 16; ++i) {
                    if (state.Gamepad.wButtons & (1 << i)) {
                        int buttonNumber = i;

                        if (buttonNumber == 12) {
                            //MessageBoxW(0, L"Pin", pin.c_str(), 0);
                            pressKey(VK_RETURN);
                            Sleep(500);
                            break;
                        }

                        if (buttonNumber == 13) {
                            pressKey(VK_BACK);
                            break;
                        }

                        switch (buttonNumber) {
                            case 0:
                                buttonNumber = 1;
                                break;
                            case 1:
                                buttonNumber = 3;
                                break;
                            case 2:
                                buttonNumber = 2;
                                break;
                            case 3:
                                buttonNumber = 4;
                                break;
                            case 4:
                                buttonNumber = 0;
                                break;
                            case 8:
                                buttonNumber = 7;
                                break;
                            case 9:
                                buttonNumber = 8;
                                break;
                            case 15:
                                buttonNumber = 9;
                                break;
                        }
                        
                        std::wstring tmpPin = std::to_wstring(buttonNumber);
                        pin += tmpPin;
                        presskeys(tmpPin);
                        break;
                    }
                }

                bool leftTrigger = state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
                bool rightTrigger = state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

                if (leftTrigger) {
                    std::wstring tmpPin = L"5";
                    pin += tmpPin;
                    presskeys(tmpPin);             
                }
                if (rightTrigger) {
                    std::wstring tmpPin = L"6";
                    pin += tmpPin;
                    presskeys(tmpPin); 
                }


                // Sleep for a short duration to avoid high CPU usage
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust this value as needed
            }
        }
        else {
            // Optionally, wait for controller connection
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second before checking again
        }
    }
}
