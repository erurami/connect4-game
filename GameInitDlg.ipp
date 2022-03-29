
#pragma once

#include "GameInitDlg.hpp"

#include "WndMenuId.h"


BOOL CALLBACK _Connect4InitializeDlgProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp
        );


void GameInitializeDlg(HWND hWndParent)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    DialogBox(
            hInstance,
            TEXT("CONNECT4INITIALIZE"),
            hWndParent,
            (DLGPROC)_Connect4InitializeDlgProc
            );
}


BOOL CALLBACK _Connect4InitializeDlgProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp
        )
{

    switch (msg)
    {
        case WM_INITDIALOG:
            {
            SendDlgItemMessage(hWnd, IDC_NEWGAME_WIDTH_SPIN, UDM_SETRANGE, 0, MAKELPARAM(100, 0));

            SendDlgItemMessage(hWnd, IDC_NEWGAME_WIDTH_SPIN, UDM_SETPOS, 0, DEFAULT_WIDTH);
            return TRUE;
            }

        case WM_CLOSE:
            EndDialog(hWnd, IDOK);
            return TRUE;

        case WM_COMMAND:
            // switch (LOWORD(wp))
            // {
            //     case IDC_SETTINGS_OK:
            //         {
            //         Connect4Settings settings;

            //         settings.m_ThinkingDepth = (int)SendDlgItemMessage(hWnd, IDC_SETTINGS_THINKDEPTH_SPIN, UDM_GETPOS, 0, 0);

            //         _Connect4SettingsSaveSettings(&settings, hWnd);
            //         }
            //     case IDC_SETTINGS_CANCEL:
            //         EndDialog(hWnd, IDOK);
            //         return TRUE;
            // }
            return FALSE;

    }
    return FALSE;
}


