
#pragma once

#include "GameInitDlg.hpp"

#include "WndMenuId.h"

#include "MainWindow.hpp"


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
            SendDlgItemMessage(hWnd, IDC_NEWGAME_WIDTH_SPIN , UDM_SETRANGE, 0, MAKELPARAM(100, 1));
            SendDlgItemMessage(hWnd, IDC_NEWGAME_HEIGHT_SPIN, UDM_SETRANGE, 0, MAKELPARAM(100, 1));

            SendDlgItemMessage(hWnd, IDC_NEWGAME_WIDTH_SPIN , UDM_SETPOS, 0, DEFAULT_WIDTH);
            SendDlgItemMessage(hWnd, IDC_NEWGAME_HEIGHT_SPIN, UDM_SETPOS, 0, DEFAULT_HEIGHT);
            return TRUE;
            }

        case WM_CLOSE:
            EndDialog(hWnd, IDOK);
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wp))
            {
                case IDC_NEWGAME_START:
                    {
                    SendMessage(GetWindow(hWnd, GW_OWNER),
                                _C4CM_INITIALIZE,
                                SendDlgItemMessage(hWnd, IDC_NEWGAME_WIDTH_SPIN , UDM_GETPOS, 0, 0),
                                SendDlgItemMessage(hWnd, IDC_NEWGAME_HEIGHT_SPIN, UDM_GETPOS, 0, 0));
                    EndDialog(hWnd, IDOK);
                    return TRUE;
                    }

                case IDC_NEWGAME_CANCEL:
                    EndDialog(hWnd, IDOK);
                    return TRUE;
            }
            return FALSE;

    }
    return FALSE;
}


