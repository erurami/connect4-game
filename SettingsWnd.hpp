
#pragma once

#include <windows.h>

#define DEF_THINKDEPTH 10

typedef struct
{
    int m_ThinkingDepth;
} Connect4Settings;

// 0 : success
// 1 : failed to open file
int _Connect4SettingsSaveSettings(Connect4Settings* pSettings, HWND hWndErrorMsgBoxParent = NULL);

// 0 : success
// 1 : failed to open file
// 2 : config file didn't exist
int _Connect4SettingsLoadSettings(Connect4Settings* pSettings, HWND hWndErrorMsgBoxParent = NULL);


int _Connect4Settings(HWND hWndParent);

#include "SettingsWnd.ipp"

