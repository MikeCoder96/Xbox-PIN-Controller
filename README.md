# Xbox Input Controller

In case the controller will control the UI in winlogon, add this key on registry:
\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Input\Settings\ControllerProcessor\ControllerToVKMapping 
And add this value:
Enabled = 0 (DWORD32)
