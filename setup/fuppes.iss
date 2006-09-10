[Setup]
AppName=Free UPnP Entertainment Service
AppVerName=Free UPnP Entertainment Service 0.5.2
AppPublisher=Ulrich V�lkel
AppPublisherURL=http://sourceforge.net/projects/fuppes
AppSupportURL=http://sourceforge.net/projects/fuppes
AppUpdatesURL=http://sourceforge.net/projects/fuppes
DefaultDirName={pf}\Free UPnP Entertainment Service\
DefaultGroupName=Free UPnP Entertainment Service
LicenseFile=..\setup\license-eng.rtf
OutputDir=..\setup
OutputBaseFilename=fuppes-0.5.2-win32-setup
SolidCompression=true
ShowTasksTreeLines=true
SourceDir=..\win32
AppCopyright=Ulrich V�lkel
WizardImageFile=compiler:wizmodernimage-is.bmp
WizardSmallImageFile=compiler:wizmodernsmallimage-is.bmp
ShowLanguageDialog=auto

[Languages]
Name: eng; Messagesfile: compiler:Default.isl; LicenseFile: ..\setup\license-eng.rtf
Name: ger; Messagesfile: compiler:Languages\German.isl; LicenseFile: ..\setup\license-eng.rtf

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: ..\win32\fuppes.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\win32\iconv.dll; DestDir: {app}
Source: ..\win32\pcre.dll; DestDir: {app}
Source: ..\AUTHORS; DestDir: {app}; Flags: ignoreversion
Source: ..\ChangeLog; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING; DestDir: {app}; Flags: ignoreversion
Source: ..\NEWS; DestDir: {app}; Flags: ignoreversion
Source: ..\README; DestDir: {app}; Flags: ignoreversion isreadme; DestName: README.txt
Source: ..\fuppes.cfg.win32; DestDir: {userappdata}\Free UPnP Entertainment Service\; DestName: fuppes.cfg; Flags: confirmoverwrite
Source: ..\win32\vorbisfile.dll; DestDir: {app}
Source: ..\win32\mpcdec.dll; DestDir: {app}
Source: ..\COPYING-MUSEPACK; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING-OGG; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING-VORBIS; DestDir: {app}; Flags: ignoreversion
Source: ..\win32\sqlite3.dll; DestDir: {app}

[Icons]
Name: {group}\Free UPnP Entertainment Service; Filename: {app}\fuppes.exe
Name: {group}\{cm:UninstallProgram,Free UPnP Entertainment Service}; Filename: {uninstallexe}
Name: {userdesktop}\Free UPnP Entertainment Service; Filename: {app}\fuppes.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Free UPnP Entertainment Service; Filename: {app}\fuppes.exe; Tasks: quicklaunchicon; Languages: 
Name: {group}\Edit configuration; Filename: {userappdata}\Free UPnP Entertainment Service\fuppes.cfg; Tasks: ; Languages: 

[Run]
Filename: {app}\fuppes.exe; Description: {cm:LaunchProgram,Free UPnP Entertainment Service}; Flags: nowait postinstall skipifsilent unchecked; Tasks: ; Languages: 
