
Sleep 6000
WinGetActiveTitle, PreviouslyActive
IfWinExist Brood War
{
    WinActivate
    Click 517, 426
}
IfWinExist, %PreviouslyActive%
{
    WinActivate
}

