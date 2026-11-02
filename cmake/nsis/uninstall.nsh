; Undoes install.nsh: .apak opens what it opened before Loupe, unless
; another program has taken it since.
Delete "$DESKTOP\Loupe.lnk"
DeleteRegKey SHCTX "Software\Classes\Loupe.apak"
ReadRegStr $0 SHCTX "Software\Classes\.apak" ""
StrCmp $0 "Loupe.apak" 0 apak_done
ReadRegStr $1 SHCTX "Software\Classes\.apak" "Loupe.previous"
DeleteRegValue SHCTX "Software\Classes\.apak" ""
StrCmp $1 "" apak_done
WriteRegStr SHCTX "Software\Classes\.apak" "" $1
apak_done:
DeleteRegValue SHCTX "Software\Classes\.apak" "Loupe.previous"
DeleteRegKey /ifempty SHCTX "Software\Classes\.apak"
System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, p 0, p 0)'
; CPack removes the vendor's Loupe key but not the vendor's own.
DeleteRegKey /ifempty SHCTX "Software\jose-villegas"
