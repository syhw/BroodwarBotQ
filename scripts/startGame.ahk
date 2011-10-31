
Sleep 6000
WinGetActiveTitle, PreviouslyActive
MouseGetPos, xpos, ypos
IfWinExist Brood War
{
    WinActivate
    Click 517, 426
}
IfWinExist, %PreviouslyActive%
    WinActivate
MouseMove, %xpos%, %ypos%, 0


