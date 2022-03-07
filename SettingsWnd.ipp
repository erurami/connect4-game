
#pragma once

#include "SettingsWnd.hpp"

#include "MainWindow.hpp"

#include <commctrl.h>
#include <strsafe.h>
#include <windows.h>

BOOL CALLBACK _Connect4SettingsDlgProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp
        );


int _Connect4Settings(HWND hWndParent)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    DialogBox(
            hInstance,
            TEXT("CONNECT4SETTINGS"),
            hWndParent,
            (DLGPROC)_Connect4SettingsDlgProc
            );

    SendMessage(hWndParent, _C4CM_LOADSETTINGS, 0, 0);

    return 1;
}


BOOL CALLBACK _Connect4SettingsDlgProc(
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
            SendDlgItemMessage(hWnd, IDC_SETTINGS_THINKDEPTH_SPIN, UDM_SETRANGE, 0, MAKELPARAM(40, 0));

            Connect4Settings settings;
            _Connect4SettingsLoadSettings(&settings);
            SendDlgItemMessage(hWnd, IDC_SETTINGS_THINKDEPTH_SPIN, UDM_SETPOS, 0, settings.m_ThinkingDepth);
            return TRUE;
            }

        case WM_CLOSE:
            EndDialog(hWnd, IDOK);
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wp))
            {
                case IDC_SETTINGS_OK:
                    {
                    Connect4Settings settings;

                    settings.m_ThinkingDepth = (int)SendDlgItemMessage(hWnd, IDC_SETTINGS_THINKDEPTH_SPIN, UDM_GETPOS, 0, 0);

                    _Connect4SettingsSaveSettings(&settings, hWnd);
                    }
                case IDC_SETTINGS_CANCEL:
                    EndDialog(hWnd, IDOK);
                    return TRUE;
            }
            return FALSE;

    }
    return FALSE;
}

int _Connect4SettingsSaveSettings(Connect4Settings* pSettings, HWND hWndErrorMsgBoxParent)
{
    FILE* pSettingsFile;

    while (fopen_s(&pSettingsFile, "connect4.cfg", "wb") != 0)
    {
        if (MessageBox(hWndErrorMsgBoxParent, TEXT("failed to open settings file"), TEXT("error"), MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
        {
            return 1;
        }
    }

    putc((char)(pSettings->m_ThinkingDepth), pSettingsFile);

    fclose(pSettingsFile);

    return 0;
}


int _Connect4SettingsLoadSettings(Connect4Settings* pSettings, HWND hWndErrorMsgBoxParent)
{
    FILE* pSettingsFile;

    if (fopen_s(&pSettingsFile, "connect4.cfg", "rb") != 0)
    {
        // save file and then retry
        Connect4Settings settings;
        settings.m_ThinkingDepth = DEF_THINKDEPTH;
        if (_Connect4SettingsSaveSettings(&settings, hWndErrorMsgBoxParent) != 0)
        {
            return 1;
        }

        while (fopen_s(&pSettingsFile, "connect4.cfg", "rb") != 0)
        {
            if (MessageBox(hWndErrorMsgBoxParent, TEXT("failed to open settings file."), TEXT("error"), MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
            {
                return 1;
            }
        }
    }

    pSettings->m_ThinkingDepth = getc(pSettingsFile);

    fclose(pSettingsFile);

    return 0;
}


