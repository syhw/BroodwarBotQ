Sleep 100
WinGetActiveTitle, PreviouslyActive
MouseGetPos, xpos, ypos
IfWinExist Brood War
{
    WinActivate
    Click 36, 362
    Click 450, 106
    Click 245, 363
}
IfWinExist, %PreviouslyActive%
    WinActivate
MouseMove, %xpos%, %ypos%, 0