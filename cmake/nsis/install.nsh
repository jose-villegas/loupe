; After the files: a desktop shortcut, and .apak files open in Loupe. The
; installer runs as admin, so SHCTX is HKLM and $DESKTOP the public desktop.
CreateShortCut "$DESKTOP\Loupe.lnk" "$INSTDIR\loupe.exe"
WriteRegStr SHCTX "Software\Classes\.apak" "" "Loupe.apak"
WriteRegStr SHCTX "Software\Classes\Loupe.apak" "" "Loupe scene pack"
WriteRegStr SHCTX "Software\Classes\Loupe.apak\DefaultIcon" "" "$INSTDIR\loupe.exe,0"
WriteRegStr SHCTX "Software\Classes\Loupe.apak\shell\open\command" "" '"$INSTDIR\loupe.exe" "%1"'
; SHCNE_ASSOCCHANGED, so Explorer shows the new icon without a sign-out.
System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, p 0, p 0)'
