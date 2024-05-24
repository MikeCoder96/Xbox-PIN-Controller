# 🎮 Xbox-Pin-Controller

Welcome to the **Xbox-Pin-Controller** repository! This project enables Windows users to use their Xbox controllers to input the Windows Hello PIN, utilizing the same PIN schema from the Xbox.

## ✨ Features

- **Seamless Integration**: Easily log into your Windows system using your Xbox controller.
- **Familiar Experience**: Use the same PIN input method as on your Xbox console ([PIN Schema](https://miro.medium.com/v2/resize:fit:1400/1*Hu3hz4ktrbt4n35X5_5ulA.png)).
- **Simple Installation and Removal**: Straightforward steps to set up and remove the controller functionality.

## 📋 Installation Guide

Follow these steps to install the Xbox-Pin-Controller:

1. **Copy the DLL File**:
   - 📂 Copy `XboxPINController.dll` to `C:\Windows\System32`.

2. **Register the DLL**:
   - 🛠 Execute `register.reg` as an Administrator.
  
**NOTE:**
In case the controller will control the UI in winlogon, add this key on registry:
\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Input\Settings\ControllerProcessor\ControllerToVKMapping 

And add this value:
Enabled = 0 (DWORD32)

## 🗑️ Removal Guide

If you wish to remove the Xbox-Pin-Controller, follow these steps:

1. **Unregister the DLL**:
   - 🛠 Execute `Unregister.reg` as an Administrator.

2. **Delete the DLL File**:
   - 🗂 Delete `XboxPINController.dll` from `C:\Windows\System32`.

## WARNING

These files edit your registry info so I will not assume any responsibility about how you will use this files

## 🤝 Contributing

Contributions are welcome! If you have any ideas, issues, or suggestions, feel free to open an issue or submit a pull request.

---

Feel free to reach out if you have any questions or need further assistance.

