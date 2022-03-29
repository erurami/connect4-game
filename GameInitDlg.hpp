
#pragma once

#include <windows.h>

#define DEFAULT_WIDTH  7

typedef struct _Connect4InitInfos
{
    int width;
    int height;
} Connect4InitInfos;

void GameInitializeDlg(HWND hWndParent);

#include "GameInitDlg.ipp"

