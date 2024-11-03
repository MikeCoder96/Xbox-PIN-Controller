//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//

#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include <unknwn.h>
#include "CCredential.h"
#include "guid.h"




CCredential::CCredential():
    _cRef(1),
    _pCredProvCredentialEvents(nullptr),
    _pszUserSid(nullptr),
    _pszQualifiedUserName(nullptr),
    _fIsLocalUser(false),
    _fChecked(false),
    _fShowControls(false),
    _dwComboIndex(0)
{
    DllAddRef();
    ZeroMemory(_rgCredProvFieldDescriptors, sizeof(_rgCredProvFieldDescriptors));
    ZeroMemory(_rgFieldStatePairs, sizeof(_rgFieldStatePairs));
    ZeroMemory(_rgFieldStrings, sizeof(_rgFieldStrings));

}

CCredential::~CCredential()
{
    //if (_rgFieldStrings[SFI_PASSWORD])
    //{
    //    size_t lenPassword = wcslen(_rgFieldStrings[SFI_PASSWORD]);
    //    SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
    //}
    if (_hwndPopupWindow)
    {
        DestroyWindow(_hwndPopupWindow);
        _hwndPopupWindow = NULL;
    }
    for (int i = 0; i < ARRAYSIZE(_rgFieldStrings); i++)
    {
        CoTaskMemFree(_rgFieldStrings[i]);
        CoTaskMemFree(_rgCredProvFieldDescriptors[i].pszLabel);
    }
    xinputController.StopCapturing();
    CoTaskMemFree(_pszUserSid);
    CoTaskMemFree(_pszQualifiedUserName);
    DllRelease();
}

// Initializes one credential with the field information passed in.
// Set the value of the SFI_LARGE_TEXT field to pwzUsername.
HRESULT CCredential::Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
                                      _In_ CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR const *rgcpfd,
                                      _In_ FIELD_STATE_PAIR const *rgfsp,
                                      _In_ ICredentialProviderUser *pcpUser)
{
    HRESULT hr = S_OK;
    _cpus = cpus;

    GUID guidProvider;
    pcpUser->GetProviderID(&guidProvider);
    _fIsLocalUser = (guidProvider == Identity_LocalUserProvider);
    xinputController.StartCapturing(0);
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Create the popup window
    hr = CreatePopupWindow();

    // Copy the field descriptors for each field. This is useful if you want to vary the field
    // descriptors based on what Usage scenario the credential was created for.
    for (DWORD i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(_rgCredProvFieldDescriptors); i++)
    {
        _rgFieldStatePairs[i] = rgfsp[i];
        hr = FieldDescriptorCopy(rgcpfd[i], &_rgCredProvFieldDescriptors[i]);
    }

    // Initialize the String value of all the fields.
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Xbox PIN Login", &_rgFieldStrings[SFI_LABEL]);
    //}
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Use Windows Hello PIN option to login", &_rgFieldStrings[SFI_LARGE_TEXT]);
    }
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Edit Text", &_rgFieldStrings[SFI_EDIT_TEXT]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PIN_TEXT]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Submit", &_rgFieldStrings[SFI_SUBMIT_BUTTON]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Checkbox", &_rgFieldStrings[SFI_CHECKBOX]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Combobox", &_rgFieldStrings[SFI_COMBOBOX]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Launch helper window", &_rgFieldStrings[SFI_LAUNCHWINDOW_LINK]);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = SHStrDupW(L"Hide additional controls", &_rgFieldStrings[SFI_HIDECONTROLS_LINK]);
    //}
    if (SUCCEEDED(hr))
    {
        hr = pcpUser->GetStringValue(PKEY_Identity_QualifiedUserName, &_pszQualifiedUserName);
    }
    //if (SUCCEEDED(hr))
    //{
    //    PWSTR pszUserName;
    //    pcpUser->GetStringValue(PKEY_Identity_UserName, &pszUserName);
    //    if (pszUserName != nullptr)
    //    {
    //        wchar_t szString[256];
    //        StringCchPrintf(szString, ARRAYSIZE(szString), L"User Name: %s", pszUserName);
    //        hr = SHStrDupW(szString, &_rgFieldStrings[SFI_FULLNAME_TEXT]);
    //        CoTaskMemFree(pszUserName);
    //    }
    //    else
    //    {
    //        hr =  SHStrDupW(L"User Name is NULL", &_rgFieldStrings[SFI_FULLNAME_TEXT]);
    //    }
    //}
    //if (SUCCEEDED(hr))
    //{
    //    PWSTR pszDisplayName;
    //    pcpUser->GetStringValue(PKEY_Identity_DisplayName, &pszDisplayName);
    //    if (pszDisplayName != nullptr)
    //    {
    //        wchar_t szString[256];
    //        StringCchPrintf(szString, ARRAYSIZE(szString), L"Display Name: %s", pszDisplayName);
    //        hr = SHStrDupW(szString, &_rgFieldStrings[SFI_DISPLAYNAME_TEXT]);
    //        CoTaskMemFree(pszDisplayName);
    //    }
    //    else
    //    {
    //        hr = SHStrDupW(L"Display Name is NULL", &_rgFieldStrings[SFI_DISPLAYNAME_TEXT]);
    //    }
    //}
    //if (SUCCEEDED(hr))
    //{
    //    PWSTR pszLogonStatus;
    //    pcpUser->GetStringValue(PKEY_Identity_LogonStatusString, &pszLogonStatus);
    //    if (pszLogonStatus != nullptr)
    //    {
    //        wchar_t szString[256];
    //        StringCchPrintf(szString, ARRAYSIZE(szString), L"Logon Status: %s", pszLogonStatus);
    //        hr = SHStrDupW(szString, &_rgFieldStrings[SFI_LOGONSTATUS_TEXT]);
    //        CoTaskMemFree(pszLogonStatus);
    //    }
    //    else
    //    {
    //        hr = SHStrDupW(L"Logon Status is NULL", &_rgFieldStrings[SFI_LOGONSTATUS_TEXT]);
    //    }
    //}

    if (SUCCEEDED(hr))
    {
        hr = pcpUser->GetSid(&_pszUserSid);
    }

    return hr;
}

HRESULT CCredential::CreatePopupWindow()
{
    // Register window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = PopupWindowProc;
    wc.hInstance = g_hinst;
    wc.lpszClassName = L"SampleCredentialPopup";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Add cursor for better UX

    RegisterClassEx(&wc);

    // Get the size of the working area (excluding taskbar)
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    // Calculate window position (e.g., top-right corner)
    int windowWidth = 500;  // Adjust as needed
    int windowHeight = 400; // Adjust as needed
    int xPos = workArea.right - windowWidth - 20;  // 20px margin from right
    int yPos = 20;  // 20px margin from top

    // Create the popup window
    _hwndPopupWindow = CreateWindowEx(
        WS_EX_TOPMOST |              // Always on top
        WS_EX_LAYERED |             // For transparency
        WS_EX_TOOLWINDOW |          // Hide from taskbar
        WS_EX_NOACTIVATE |          // Prevent activation
        WS_EX_TRANSPARENT,          // Click-through
        L"SampleCredentialPopup",
        L"Authentication",
        WS_POPUP |                  // Popup window
        WS_VISIBLE,                 // Make it visible
        xPos, yPos,
        windowWidth, windowHeight,
        NULL, NULL,
        g_hinst,
        this
    );

    if (_hwndPopupWindow)
    {
        // Store the this pointer
        SetWindowLongPtr(_hwndPopupWindow, GWLP_USERDATA, (LONG_PTR)this);

        // Make window semi-transparent
        SetLayeredWindowAttributes(_hwndPopupWindow, 0, 200, LWA_ALPHA);

        // Load and show the image
        LoadAndDisplayImage();

        ShowWindow(_hwndPopupWindow, SW_SHOW);
        UpdateWindow(_hwndPopupWindow);

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CCredential::LoadAndDisplayImage()
{
    if (!_hwndPopupWindow)
        return E_FAIL;

    // Get DC for the window
    HDC hdcWindow = GetDC(_hwndPopupWindow);
    if (!hdcWindow)
        return E_FAIL;

    RECT rcWindow;
    GetClientRect(_hwndPopupWindow, &rcWindow);

    // Create a memory DC and bitmap for double buffering
    HDC hdcMem = CreateCompatibleDC(hdcWindow);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, rcWindow.right, rcWindow.bottom);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // Create solid background
    HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30));
    FillRect(hdcMem, &rcWindow, hBrush);
    DeleteObject(hBrush);

    // Load and draw the image
    Gdiplus::Graphics graphics(hdcMem);
    Gdiplus::Image* pImage = new Gdiplus::Image(L"C:\\Windows\\System32\\combo.png");

    if (pImage && pImage->GetLastStatus() == Gdiplus::Ok)
    {
        // Calculate position to center the image
        float imageWidth = (float)pImage->GetWidth();
        float imageHeight = (float)pImage->GetHeight();
        float windowWidth = (float)(rcWindow.right - rcWindow.left);
        float windowHeight = (float)(rcWindow.bottom - rcWindow.top);

        // Scale image to fit window while maintaining aspect ratio
        float scale = min(windowWidth / imageWidth, windowHeight / imageHeight);
        float scaledWidth = imageWidth * scale;
        float scaledHeight = imageHeight * scale;

        // Center the image
        float x = (windowWidth - scaledWidth) / 2;
        float y = (windowHeight - scaledHeight) / 2;

        graphics.DrawImage(pImage, x, y, scaledWidth, scaledHeight);
        delete pImage;
    }

    // Copy the memory DC to the window DC
    BitBlt(hdcWindow, 0, 0, rcWindow.right, rcWindow.bottom, hdcMem, 0, 0, SRCCOPY);

    // Clean up
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(_hwndPopupWindow, hdcWindow);

    return S_OK;
}

// Window procedure for handling window movement
LRESULT CALLBACK CCredential::PopupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CCredential* pProvider = (CCredential*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!pProvider)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        // Start window drag
        pProvider->_isDragging = true;
        SetCapture(hwnd);
        GetCursorPos(&pProvider->_lastMousePos);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if (pProvider->_isDragging)
        {
            POINT currentPos;
            GetCursorPos(&currentPos);

            // Calculate the distance moved
            int dx = currentPos.x - pProvider->_lastMousePos.x;
            int dy = currentPos.y - pProvider->_lastMousePos.y;

            // Get current window position
            RECT rcWindow;
            GetWindowRect(hwnd, &rcWindow);

            // Move the window
            SetWindowPos(hwnd, NULL,
                rcWindow.left + dx,
                rcWindow.top + dy,
                0, 0,
                SWP_NOSIZE | SWP_NOZORDER);

            pProvider->_lastMousePos = currentPos;
        }
        return 0;
    }

    case WM_LBUTTONUP:
    {
        // End window drag
        if (pProvider->_isDragging)
        {
            pProvider->_isDragging = false;
            ReleaseCapture();
        }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        pProvider->LoadAndDisplayImage();
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1; // Prevent flickering
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// LogonUI calls this in order to give us a callback in case we need to notify it of anything.
HRESULT CCredential::Advise(_In_ ICredentialProviderCredentialEvents *pcpce)
{
    if (_pCredProvCredentialEvents != nullptr)
    {
        _pCredProvCredentialEvents->Release();
    }
    return pcpce->QueryInterface(IID_PPV_ARGS(&_pCredProvCredentialEvents));
}

// LogonUI calls this to tell us to release the callback.
HRESULT CCredential::UnAdvise()
{
    if (_pCredProvCredentialEvents)
    {
        _pCredProvCredentialEvents->Release();
    }
    _pCredProvCredentialEvents = nullptr;
    return S_OK;
}

// LogonUI calls this function when our tile is selected (zoomed)
// If you simply want fields to show/hide based on the selected state,
// there's no need to do anything here - you can set that up in the
// field definitions. But if you want to do something
// more complicated, like change the contents of a field when the tile is
// selected, you would do it here.
HRESULT CCredential::SetSelected(_Out_ BOOL *pbAutoLogon)
{
    *pbAutoLogon = FALSE;
    return S_OK;
}

// Similarly to SetSelected, LogonUI calls this when your tile was selected
// and now no longer is. The most common thing to do here (which we do below)
// is to clear out the password field.
HRESULT CCredential::SetDeselected()
{
    HRESULT hr = S_OK;
    //if (_rgFieldStrings[SFI_PASSWORD])
    //{
    //    size_t lenPassword = wcslen(_rgFieldStrings[SFI_PASSWORD]);
    //    SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
    //
    //    CoTaskMemFree(_rgFieldStrings[SFI_PASSWORD]);
    //    hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);
    //
    //    if (SUCCEEDED(hr) && _pCredProvCredentialEvents)
    //    {
    //        _pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, _rgFieldStrings[SFI_PASSWORD]);
    //    }
    //}

    return hr;
}

// Get info for a particular field of a tile. Called by logonUI to get information
// to display the tile.
HRESULT CCredential::GetFieldState(DWORD dwFieldID,
                                         _Out_ CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs,
                                         _Out_ CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis)
{
    HRESULT hr;

    // Validate our parameters.
    if ((dwFieldID < ARRAYSIZE(_rgFieldStatePairs)))
    {
        *pcpfs = _rgFieldStatePairs[dwFieldID].cpfs;
        *pcpfis = _rgFieldStatePairs[dwFieldID].cpfis;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

// Sets ppwsz to the string value of the field at the index dwFieldID
HRESULT CCredential::GetStringValue(DWORD dwFieldID, _Outptr_result_nullonfailure_ PWSTR *ppwsz)
{
    HRESULT hr;
    *ppwsz = nullptr;

    // Check to make sure dwFieldID is a legitimate index
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors))
    {
        // Make a copy of the string and return that. The caller
        // is responsible for freeing it.
        hr = SHStrDupW(_rgFieldStrings[dwFieldID], ppwsz);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Get the image to show in the user tile
HRESULT CCredential::GetBitmapValue(DWORD dwFieldID, _Outptr_result_nullonfailure_ HBITMAP *phbmp)
{
    HRESULT hr;
    *phbmp = nullptr;

    if ((SFI_TILEIMAGE == dwFieldID))
    {
        HBITMAP hbmp = LoadBitmap(HINST_THISDLL, MAKEINTRESOURCE(IDB_TILE_IMAGE));
        if (hbmp != nullptr)
        {
            hr = S_OK;
            *phbmp = hbmp;
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Sets pdwAdjacentTo to the index of the field the submit button should be
// adjacent to. We recommend that the submit button is placed next to the last
// field which the user is required to enter information in. Optional fields
// should be below the submit button.
HRESULT CCredential::GetSubmitButtonValue(DWORD dwFieldID, _Out_ DWORD *pdwAdjacentTo)
{
    HRESULT hr;
    hr = S_OK;

    //if (SFI_SUBMIT_BUTTON == dwFieldID)
    //{
    //    // pdwAdjacentTo is a pointer to the fieldID you want the submit button to
    //    // appear next to.
    //    *pdwAdjacentTo = SFI_PASSWORD;
    //    hr = S_OK;
    //}
    //else
    //{
    //    hr = E_INVALIDARG;
    //}
    return hr;
}

// Sets the value of a field which can accept a string as a value.
// This is called on each keystroke when a user types into an edit field
HRESULT CCredential::SetStringValue(DWORD dwFieldID, _In_ PCWSTR pwz)
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_EDIT_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft ||
        CPFT_PASSWORD_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        PWSTR *ppwszStored = &_rgFieldStrings[dwFieldID];
        CoTaskMemFree(*ppwszStored);
        hr = SHStrDupW(pwz, ppwszStored);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Returns whether a checkbox is checked or not as well as its label.
HRESULT CCredential::GetCheckboxValue(DWORD dwFieldID, _Out_ BOOL *pbChecked, _Outptr_result_nullonfailure_ PWSTR *ppwszLabel)
{
    HRESULT hr;
    *ppwszLabel = nullptr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        *pbChecked = _fChecked;
        //hr = SHStrDupW(_rgFieldStrings[SFI_CHECKBOX], ppwszLabel);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Sets whether the specified checkbox is checked or not.
HRESULT CCredential::SetCheckboxValue(DWORD dwFieldID, BOOL bChecked)
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        _fChecked = bChecked;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Returns the number of items to be included in the combobox (pcItems), as well as the
// currently selected item (pdwSelectedItem).
HRESULT CCredential::GetComboBoxValueCount(DWORD dwFieldID, _Out_ DWORD *pcItems, _Deref_out_range_(<, *pcItems) _Out_ DWORD *pdwSelectedItem)
{
    HRESULT hr;
    *pcItems = 0;
    *pdwSelectedItem = 0;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        *pcItems = ARRAYSIZE(s_rgComboBoxStrings);
        *pdwSelectedItem = 0;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Called iteratively to fill the combobox with the string (ppwszItem) at index dwItem.
HRESULT CCredential::GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, _Outptr_result_nullonfailure_ PWSTR *ppwszItem)
{
    HRESULT hr;
    *ppwszItem = nullptr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        hr = SHStrDupW(s_rgComboBoxStrings[dwItem], ppwszItem);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Called when the user changes the selected item in the combobox.
HRESULT CCredential::SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem)
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        _dwComboIndex = dwSelectedItem;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Called when the user clicks a command link.
HRESULT CCredential::CommandLinkClicked(DWORD dwFieldID)
{
    HRESULT hr = S_OK;

    CREDENTIAL_PROVIDER_FIELD_STATE cpfsShow = CPFS_HIDDEN;

    // Validate parameter.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
        (CPFT_COMMAND_LINK == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        HWND hwndOwner = nullptr;
        switch (dwFieldID)
        {
        //case SFI_LAUNCHWINDOW_LINK:
        //    if (_pCredProvCredentialEvents)
        //    {
        //        _pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
        //    }
        //
        //    // Pop a messagebox indicating the click.
        //    ::MessageBox(hwndOwner, L"Command link clicked", L"Click!", 0);
        //    break;
        //case SFI_HIDECONTROLS_LINK:
        //    _pCredProvCredentialEvents->BeginFieldUpdates();
        //    cpfsShow = _fShowControls ? CPFS_DISPLAY_IN_SELECTED_TILE : CPFS_HIDDEN;
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_FULLNAME_TEXT, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_DISPLAYNAME_TEXT, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_LOGONSTATUS_TEXT, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_CHECKBOX, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_EDIT_TEXT, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldState(nullptr, SFI_COMBOBOX, cpfsShow);
        //    _pCredProvCredentialEvents->SetFieldString(nullptr, SFI_HIDECONTROLS_LINK, _fShowControls? L"Hide additional controls" : L"Show additional controls");
        //    _pCredProvCredentialEvents->EndFieldUpdates();
        //    _fShowControls = !_fShowControls;
        //    break;
        default:
            hr = E_INVALIDARG;
        }

    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Collect the username and password into a serialized credential for the correct usage scenario
// (logon/unlock is what's demonstrated in this sample).  LogonUI then passes these credentials
// back to the system to log on.
HRESULT CCredential::GetSerialization(_Out_ CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
                                            _Out_ CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs,
                                            _Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
                                            _Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon)
{
    HRESULT hr = E_UNEXPECTED;
    *pcpgsr = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
    *ppwszOptionalStatusText = nullptr;
    *pcpsiOptionalStatusIcon = CPSI_NONE;
    ZeroMemory(pcpcs, sizeof(*pcpcs));

    // For local user, the domain and user name can be split from _pszQualifiedUserName (domain\username).
    // CredPackAuthenticationBuffer() cannot be used because it won't work with unlock scenario.
    //if (_fIsLocalUser)
    //{
    //    PWSTR pwzProtectedPassword;
    //    hr = ProtectIfNecessaryAndCopyPassword(_rgFieldStrings[SFI_PASSWORD], _cpus, &pwzProtectedPassword);
    //    if (SUCCEEDED(hr))
    //    {
    //        PWSTR pszDomain;
    //        PWSTR pszUsername;
    //        hr = SplitDomainAndUsername(_pszQualifiedUserName, &pszDomain, &pszUsername);
    //        if (SUCCEEDED(hr))
    //        {
    //            KERB_INTERACTIVE_UNLOCK_LOGON kiul;
    //            hr = KerbInteractiveUnlockLogonInit(pszDomain, pszUsername, pwzProtectedPassword, _cpus, &kiul);
    //            if (SUCCEEDED(hr))
    //            {
    //                // We use KERB_INTERACTIVE_UNLOCK_LOGON in both unlock and logon scenarios.  It contains a
    //                // KERB_INTERACTIVE_LOGON to hold the creds plus a LUID that is filled in for us by Winlogon
    //                // as necessary.
    //                hr = KerbInteractiveUnlockLogonPack(kiul, &pcpcs->rgbSerialization, &pcpcs->cbSerialization);
    //                if (SUCCEEDED(hr))
    //                {
    //                    ULONG ulAuthPackage;
    //                    hr = RetrieveNegotiateAuthPackage(&ulAuthPackage);
    //                    if (SUCCEEDED(hr))
    //                    {
    //                        pcpcs->ulAuthenticationPackage = ulAuthPackage;
    //                        pcpcs->clsidCredentialProvider = CLSID_CSample;
    //                        // At this point the credential has created the serialized credential used for logon
    //                        // By setting this to CPGSR_RETURN_CREDENTIAL_FINISHED we are letting logonUI know
    //                        // that we have all the information we need and it should attempt to submit the
    //                        // serialized credential.
    //                        *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
    //                    }
    //                }
    //            }
    //            CoTaskMemFree(pszDomain);
    //            CoTaskMemFree(pszUsername);
    //        }
    //        CoTaskMemFree(pwzProtectedPassword);
    //    }
    //}
    //else
    //{
    //    DWORD dwAuthFlags = CRED_PACK_PROTECTED_CREDENTIALS | CRED_PACK_ID_PROVIDER_CREDENTIALS;
    //
    //    // First get the size of the authentication buffer to allocate
    //    if (!CredPackAuthenticationBuffer(dwAuthFlags, _pszQualifiedUserName, const_cast<PWSTR>(_rgFieldStrings[SFI_PASSWORD]), nullptr, &pcpcs->cbSerialization) &&
    //        (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
    //    {
    //        pcpcs->rgbSerialization = static_cast<byte *>(CoTaskMemAlloc(pcpcs->cbSerialization));
    //        if (pcpcs->rgbSerialization != nullptr)
    //        {
    //            hr = S_OK;
    //
    //            // Retrieve the authentication buffer
    //            if (CredPackAuthenticationBuffer(dwAuthFlags, _pszQualifiedUserName, const_cast<PWSTR>(_rgFieldStrings[SFI_PASSWORD]), pcpcs->rgbSerialization, &pcpcs->cbSerialization))
    //            {
    //                ULONG ulAuthPackage;
    //                hr = RetrieveNegotiateAuthPackage(&ulAuthPackage);
    //                if (SUCCEEDED(hr))
    //                {
    //                    pcpcs->ulAuthenticationPackage = ulAuthPackage;
    //                    pcpcs->clsidCredentialProvider = CLSID_CSample;
    //
    //                    // At this point the credential has created the serialized credential used for logon
    //                    // By setting this to CPGSR_RETURN_CREDENTIAL_FINISHED we are letting logonUI know
    //                    // that we have all the information we need and it should attempt to submit the
    //                    // serialized credential.
    //                    *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
    //                }
    //            }
    //            else
    //            {
    //                hr = HRESULT_FROM_WIN32(GetLastError());
    //                if (SUCCEEDED(hr))
    //                {
    //                    hr = E_FAIL;
    //                }
    //            }
    //
    //            if (FAILED(hr))
    //            {
    //                CoTaskMemFree(pcpcs->rgbSerialization);
    //            }
    //        }
    //        else
    //        {
    //            hr = E_OUTOFMEMORY;
    //        }
    //    }
    //}
    return hr;
}

struct REPORT_RESULT_STATUS_INFO
{
    NTSTATUS ntsStatus;
    NTSTATUS ntsSubstatus;
    PWSTR     pwzMessage;
    CREDENTIAL_PROVIDER_STATUS_ICON cpsi;
};

static const REPORT_RESULT_STATUS_INFO s_rgLogonStatusInfo[] =
{
    { STATUS_LOGON_FAILURE, STATUS_SUCCESS, L"Incorrect password or username.", CPSI_ERROR, },
    { STATUS_ACCOUNT_RESTRICTION, STATUS_ACCOUNT_DISABLED, L"The account is disabled.", CPSI_WARNING },
};

// ReportResult is completely optional.  Its purpose is to allow a credential to customize the string
// and the icon displayed in the case of a logon failure.  For example, we have chosen to
// customize the error shown in the case of bad username/password and in the case of the account
// being disabled.
HRESULT CCredential::ReportResult(NTSTATUS ntsStatus,
                                        NTSTATUS ntsSubstatus,
                                        _Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
                                        _Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon)
{
    *ppwszOptionalStatusText = nullptr;
    *pcpsiOptionalStatusIcon = CPSI_NONE;

    DWORD dwStatusInfo = (DWORD)-1;

    // Look for a match on status and substatus.
    for (DWORD i = 0; i < ARRAYSIZE(s_rgLogonStatusInfo); i++)
    {
        if (s_rgLogonStatusInfo[i].ntsStatus == ntsStatus && s_rgLogonStatusInfo[i].ntsSubstatus == ntsSubstatus)
        {
            dwStatusInfo = i;
            break;
        }
    }

    if ((DWORD)-1 != dwStatusInfo)
    {
        if (SUCCEEDED(SHStrDupW(s_rgLogonStatusInfo[dwStatusInfo].pwzMessage, ppwszOptionalStatusText)))
        {
            *pcpsiOptionalStatusIcon = s_rgLogonStatusInfo[dwStatusInfo].cpsi;
        }
    }

    // If we failed the logon, try to erase the password field.
    if (FAILED(HRESULT_FROM_NT(ntsStatus)))
    {
        if (_pCredProvCredentialEvents)
        {
            //_pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, L"");
        }
    }

    // Since nullptr is a valid value for *ppwszOptionalStatusText and *pcpsiOptionalStatusIcon
    // this function can't fail.
    return S_OK;
}

// Gets the SID of the user corresponding to the credential.
HRESULT CCredential::GetUserSid(_Outptr_result_nullonfailure_ PWSTR *ppszSid)
{
    *ppszSid = nullptr;
    HRESULT hr = E_UNEXPECTED;
    if (_pszUserSid != nullptr)
    {
        hr = SHStrDupW(_pszUserSid, ppszSid);
    }
    // Return S_FALSE with a null SID in ppszSid for the
    // credential to be associated with an empty user tile.

    return hr;
}

// GetFieldOptions to enable the password reveal button and touch keyboard auto-invoke in the password field.
HRESULT CCredential::GetFieldOptions(DWORD dwFieldID,
                                           _Out_ CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS *pcpcfo)
{
    *pcpcfo = CPCFO_NONE;

    //if (dwFieldID == SFI_PASSWORD)
    //{
    //    *pcpcfo = CPCFO_ENABLE_PASSWORD_REVEAL;
    //}
    //else if (dwFieldID == SFI_TILEIMAGE)
    //{
    //    *pcpcfo = CPCFO_ENABLE_TOUCH_KEYBOARD_AUTO_INVOKE;
    //}

    return S_OK;
}
