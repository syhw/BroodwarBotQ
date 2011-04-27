Sleep 100
WinGetActiveTitle, PreviouslyActive
MouseGetPos, xpos, ypos
IfWinExist Brood War
{
    WinActivate
    Click 36, 362 ; click the "Observers Menu" (the eye in the bottom left corner above the minimap)
    Loop, %0%  ; For each parameter, click the vision/cross out! The first player is at (450, 106) and then delta of ~(0, 26)
    {
        param := %A_Index%
        yValue := 106 + (%param% * 26)
        Click 450, %yValue%
    }
    Click 245, 363 ; click "Accept|
}
IfWinExist, %PreviouslyActive%
    WinActivate
MouseMove, %xpos%, %ypos%, 0