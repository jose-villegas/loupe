; Undoes install.nsh. .apak is left alone if another program has taken it since.
Delete "$DESKTOP\Loupe.lnk"
DeleteRegKey SHCTX "Software\Classes\Loupe.apak"
ReadRegStr $0 SHCTX "Software\Classes\.apak" ""
StrCmp $0 "Loupe.apak" 0 +2
DeleteRegKey SHCTX "Software\Classes\.apak"
System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, p 0, p 0)'
