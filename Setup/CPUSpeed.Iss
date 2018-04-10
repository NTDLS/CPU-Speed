[Setup]
;-- Main Setup Information
 AppName                = CPUSpeed
 AppVerName             = CPUSpeed 1.0.0.3
 AppCopyright           = Copyright © 1995-2008 NetworkDLS.
 DefaultDirName         = {pf}\NetworkDLS\CPUSpeed
 DefaultGroupName       = NetworkDLS\CPUSpeed
 UninstallDisplayIcon   = {app}\CPUSpeed.Exe
 WizardImageFile        = \..\..\@Resources\Setup\LgSetup.bmp
 WizardSmallImageFile   = \..\..\@Resources\Setup\SmSetup.bmp
 PrivilegesRequired     = PowerUser
 Uninstallable          = Yes
 AppMutex               = CPUSpeed
 LicenseFile            = \..\..\@Resources\Setup\EULA.txt
 Compression            = ZIP/9
 OutputBaseFilename     = Setup
 MinVersion             = 0.0,5.0

;-- Windows 2000 & XP (Support Dialog)
 AppPublisher    = NetworkDLS
 AppPublisherURL = http://www.NetworkDLS.com/
 AppUpdatesURL   = http://www.NetworkDLS.com/
 AppVersion      = 1.0.0.3

[Files]
 Source: "..\Release\CPUSpd.Exe"; DestDir: "{app}";

[Icons]
 Name: "{group}\CPU Speed"; Filename: "{app}\CPUSpd.Exe";

[Registry]
 Root: HKLM; Subkey: "Software\NetworkDLS\CPUSpeed"; Flags: uninsdeletekey noerror;
 
[Run]
 Filename: "{app}\CPUSpd.Exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

