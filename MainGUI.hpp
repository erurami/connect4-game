
#pragma once

// (C)onnect(4) (G)UI (W)indow (M)essage
#define _C4GWM_REDRAW      (WM_APP + 1)

#define CONNECT4_CELL_GAP 5
#define CONNECT4_CELL_ROUNDNESS (cell_width / 10)

int _Connect4GuiRegisterMainGuiWndClass(LPTSTR strWndClassName);

#include "MainGUI.ipp"

