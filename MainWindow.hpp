
#pragma once

// (C)onnect(4) (C)ommand (M)essage
#define _C4CM_INITIALIZE        (WM_APP + 1)
#define _C4CM_INITIALIZEWITHWZD (WM_APP + 2)
#define _C4CM_PUTCOIN           (WM_APP + 3)
#define _C4CM_UNDO              (WM_APP + 4)
#define _C4CM_STARTTHINKING     (WM_APP + 5)
#define _C4CM_STOPTHINKING      (WM_APP + 6)
#define _C4CM_GETTURN           (WM_APP + 7)
#define _C4CM_GETSTATE          (WM_APP + 8)
#define _C4CM_GETGAMEWIDTH      (WM_APP + 9)
#define _C4CM_GETGAMEHEIGHT     (WM_APP + 10)
#define _C4CM_GETAT             (WM_APP + 11)
#define _C4CM_ISINITIALIZED     (WM_APP + 12)
#define _C4CM_SETTINGS          (WM_APP + 13)
#define _C4CM_LOADSETTINGS      (WM_APP + 14)
// (C)onnect(4) (W)indow (M)essage
#define _C4WM_ADJUSTCHILDWND    (WM_APP + 15)

int RunConnect4Game(void);

#include "MainWindow.ipp"

