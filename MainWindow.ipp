

#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib" )
#pragma comment(lib, "Comdlg32.lib" )

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "MainWindow.hpp"

#define INCLUDE_4INAROW
#include "game/minigames.hpp"
#include "engine/engine.hpp"

#include "MainGUI.hpp"

#include "WndMenuId.h"

#include "SettingsWnd.hpp"

#define UNICODE
#include <windows.h>



LRESULT CALLBACK _Connect4GuiMainWndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp
        );

HWND _Connect4GuiSetupMainWindow(void);

int RunConnect4Game(void)
{
    // instance count check
    static int instance_count = 0;
    if (instance_count >= 1)
    {
        MessageBox(NULL, TEXT("Can't create more than 1 instance."), TEXT("error"), MB_ICONERROR | MB_OK);
        return 1;
    }
    instance_count++;


    // CreateWindow
    HWND hWnd;

    hWnd = _Connect4GuiSetupMainWindow();

    if (hWnd == NULL)
    {
        MessageBox(NULL, TEXT("window creation error"), TEXT("error"), MB_ICONERROR | MB_OK);
        return 1;
    }

    /// Mainloop
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}



HWND _Connect4GuiSetupMainWindow(void)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS winc;

    winc.style         = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc   = _Connect4GuiMainWndProc;
    winc.cbClsExtra    = 0;
    winc.cbWndExtra    = 0;
    winc.hInstance     = hInstance;
    winc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszMenuName  = TEXT("WNDMENU");
    winc.lpszClassName = TEXT("Connect4MainWnd");

    if (GetClassInfo(hInstance, winc.lpszClassName, &winc))
    {
    }
    else
    {
        if (!RegisterClass(&winc))
        {
            return NULL;
        }
    }

    HWND hWnd;

    hWnd = CreateWindow(
            TEXT("Connect4MainWnd"), TEXT("Connect4"),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500,
            500,
            NULL, NULL, hInstance, NULL
            );

    return hWnd;
}


#define _TIMER_ID_CHECKTHINKRESULT 1

LRESULT CALLBACK _Connect4GuiMainWndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{
    // win32 vars
    static HWND hWnd_main_gui;
    static HINSTANCE hInstance;

    static int game_state;
    // 0 : Uninitialized
    // 1 : Player thinking
    // 2 : Bot thinking

    static FourInARow::Game* p_game;
    static Connect4Ai* p_engine;


    // settings vars
    static int think_depth;

    switch (msg)
    {


        case WM_CLOSE:
            {
            int response;
            response = MessageBox(hWnd, TEXT("quit?"), TEXT("Message"), MB_ICONQUESTION | MB_YESNO);

            if (response == IDNO)
            {
                return 0;
            }
            return DefWindowProc(hWnd, msg, wp, lp);
            }


        case WM_DESTROY:
            if (p_game != NULL)
            {
                delete p_game;
            }
            if (p_engine != NULL)
            {
                delete p_engine;
            }
            PostQuitMessage(0);
            return 0;




        case WM_CREATE:
            p_game = NULL;
            p_engine = NULL;

            hInstance = GetModuleHandle(NULL);

            SendMessage(hWnd, _C4CM_LOADSETTINGS, ((CREATESTRUCT*)lp)->cx, ((CREATESTRUCT*)lp)->cy);

            _Connect4GuiRegisterMainGuiWndClass(TEXT("_Connect4MainGui"));
            hWnd_main_gui = CreateWindow(
                    TEXT("_Connect4MainGui"), TEXT(""),
                    WS_CHILD,
                    0, 0,
                    10, 10,
                    hWnd, NULL, hInstance, NULL
                    );

            SendMessage(hWnd, _C4WM_ADJUSTCHILDWND, ((CREATESTRUCT*)lp)->cx, ((CREATESTRUCT*)lp)->cy);

            ShowWindow(hWnd, SW_SHOW);
            return 0;


        case WM_SIZE:
            SendMessage(hWnd, _C4WM_ADJUSTCHILDWND, LOWORD(lp), HIWORD(lp));
            return 0;


        case WM_COMMAND:
            if (lp == 0)
            {
                switch (LOWORD(wp))
                {
                    case MENUID_EXIT:
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                        break;

                    case MENUID_UNDO:
                        PostMessage(hWnd, _C4CM_UNDO, 0, 0);
                        break;

                    case MENUID_NEWGAME:
                        PostMessage(hWnd, _C4CM_INITIALIZEWITHWZD, 0, 0);
                        break;

                    case MENUID_SETTINGS:
                        PostMessage(hWnd, _C4CM_SETTINGS, 0, 0);
                        break;

                    case MENUID_SAVE:
                        PostMessage(hWnd, _C4CM_SAVEAS, 1, 0);
                        break;

                    case MENUID_SAVEAS:
                        PostMessage(hWnd, _C4CM_SAVEAS, 0, 0);
                        break;

                    case MENUID_LOAD:
                        PostMessage(hWnd, _C4CM_OPEN, 1, 0);
                        break;

                    case MENUID_OPEN:
                        PostMessage(hWnd, _C4CM_OPEN, 0, 0);
                        break;
                }
            }
            return 0;


        case WM_KEYUP:
            switch (wp)
            {
                case 'N':
                    if (GetKeyState(VK_CONTROL) < 0 &&
                        GetKeyState(VK_SHIFT  ) >= 0)
                    {
                        PostMessage(hWnd, _C4CM_INITIALIZEWITHWZD, 0, 0);
                    }
                    break;
            }
            return 0;




        case _C4WM_ADJUSTCHILDWND:
            SetWindowPos(
                    hWnd_main_gui, HWND_NOTOPMOST,
                    0, 0,
                    (int)wp, (int)lp,
                    SWP_NOZORDER
                    );
            InvalidateRect(hWnd_main_gui, NULL, FALSE);
            return 0;




        case _C4CM_INITIALIZE:
            if (p_game != NULL)
            {
                delete p_game;
            }
            p_game = new FourInARow::Game((int)wp, (int)lp);
            game_state = 1;
            InvalidateRect(hWnd_main_gui, NULL, FALSE);
            return 0;


        case _C4CM_INITIALIZEWITHWZD:
            // TODO:
            SendMessage(hWnd, _C4CM_INITIALIZE, 7, 6);
            return 0;


        case _C4CM_PUTCOIN:
            {
            int result = p_game->PutCoin((int)wp);
            game_state = p_game->GetWhichTurn();
            InvalidateRect(hWnd_main_gui, NULL, FALSE);
            if (result == 0)
            {
                SendMessage(hWnd, _C4CM_STARTTHINKING, 0, 0);
            }
            else
            {
                MessageBox(hWnd, TEXT("failed to put that column"), TEXT("error"), MB_ICONERROR);
            }
            return result;
            }


        case _C4CM_UNDO:
            if (p_game == NULL)
            {
                return 0;
            }
            if (p_game->GetWhichTurn() == 1)
            {
                p_game->Undo();
            }
            p_game->Undo();
            InvalidateRect(hWnd_main_gui, NULL, FALSE);
            return 0;


        case _C4CM_STARTTHINKING:
            p_engine = new Connect4Ai(p_game);
            p_engine->StartThinkingNextMove();
            SetTimer(hWnd, _TIMER_ID_CHECKTHINKRESULT, 100, NULL);
            return 0;


        case _C4CM_STOPTHINKING:
            if (p_engine == NULL)
            {
                MessageBox(hWnd, TEXT("StopThinking"), TEXT("Stopthinking"), MB_ICONERROR);
                return 0;
            }
            p_engine->KillThinkingProcess();
            InvalidateRect(hWnd_main_gui, NULL, FALSE);
            return 0;


        case _C4CM_GETTURN:
            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("GetTurn"), TEXT("GetTurn"), MB_ICONERROR);
                return 0;
            }
            return p_game->GetWhichTurn();


        case _C4CM_GETSTATE:
            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("GetState"), TEXT("GetState"), MB_ICONERROR);
                return 0;
            }
            return p_game->GetGameStatus();


        case _C4CM_GETGAMEWIDTH:
            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("GetWidth"), TEXT("GetWidth"), MB_ICONERROR);
                return 0;
            }
            return p_game->GetBoardWidth();


        case _C4CM_GETGAMEHEIGHT:
            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("GetHeight"), TEXT("GetHeight"), MB_ICONERROR);
                return 0;
            }
            return p_game->GetBoardHeight();


        case _C4CM_GETAT:
            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("GetAt"), TEXT("GetAt"), MB_ICONERROR);
                return 0;
            }
            return p_game->GetAt((int)wp, (int)lp);


        case _C4CM_ISINITIALIZED:
            if (p_game == NULL)
            {
                return FALSE;
            }
            return TRUE;


        case _C4CM_SETTINGS:
            _Connect4Settings(hWnd);
            return 0;


        case _C4CM_LOADSETTINGS:
            {
            Connect4Settings settings;
            _Connect4SettingsLoadSettings(&settings, hWnd);

            think_depth = settings.m_ThinkingDepth;
            Connect4Ai::SetThinkDepth(think_depth);
            return 0;
            }


        case _C4CM_SAVEAS:
            {

            if (p_game == NULL)
            {
                MessageBox(hWnd, TEXT("game is not initialized.\npress \"new\" to start new game."), TEXT("error"), MB_ICONERROR);
                return 0;
            }

            FILE* p_save_file;

            TCHAR str_save_path[MAX_PATH] = TEXT("save.co4");

            if (wp == 0)
            {
                OPENFILENAME ofn = {0};

                ofn.lStructSize = sizeof (OPENFILENAME);
                ofn.hwndOwner = hWnd;

                ofn.lpstrFilter = TEXT("Co4 files {*.co4*}\0*.co4*\0");
                ofn.lpstrCustomFilter = NULL;
                ofn.nFilterIndex = 0;

                ofn.lpstrFile = str_save_path;
                ofn.nMaxFile = MAX_PATH;

                ofn.Flags = OFN_EXPLORER;

                ofn.lpstrDefExt = NULL;

                GetSaveFileName(&ofn);
            }

            if (fopen_s(&p_save_file, str_save_path, "w") != 0)
            {
                MessageBox(hWnd, TEXT("error opening save file"), TEXT("error"), MB_ICONERROR);
                return 0;
            }

            char* save_data;
            save_data = new char [p_game->GetSaveSize()];

            p_game->Export(save_data, p_game->GetSaveSize());

            fprintf(p_save_file, "%s", save_data);

            delete [] save_data;

            fclose(p_save_file);

            return 0;
            }


        case _C4CM_OPEN:
            {

            TCHAR str_open_path[MAX_PATH] = TEXT("save.co4");

            if (wp == 0)
            {
                OPENFILENAME ofn = {0};

                ofn.lStructSize = sizeof (OPENFILENAME);
                ofn.hwndOwner = hWnd;

                ofn.lpstrFilter = TEXT("Co4 files {*.co4*}\0*.co4*\0");
                ofn.lpstrCustomFilter = NULL;
                ofn.nFilterIndex = 0;

                ofn.lpstrFile = str_open_path;
                ofn.nMaxFile = MAX_PATH;

                ofn.Flags = OFN_EXPLORER;

                ofn.lpstrDefExt = NULL;

                GetOpenFileName(&ofn);
            }

            FILE* p_save_file;

            if (fopen_s(&p_save_file, str_open_path, "r") != 0)
            {
                MessageBox(hWnd, TEXT("error opening save file"), TEXT("error"), MB_ICONERROR);
                return 0;
            }

            fseek(p_save_file, 0L, SEEK_END);
            int save_size = ftell(p_save_file);
            fseek(p_save_file, 0L, SEEK_SET);

            char* save_data;
            save_data = new char [save_size + 1];

            fgets(save_data, save_size + 1, p_save_file);


            if (p_game == NULL)
            {
                SendMessage(hWnd, _C4CM_INITIALIZE, 1, 1);
            }

            if (p_game->Import(save_data) == 0)
            {
                MessageBox(hWnd, TEXT("error importing file"), TEXT("error"), MB_ICONERROR);
            }
            InvalidateRect(hWnd_main_gui, NULL, FALSE);

            fclose(p_save_file);
            return 0;
            }




        case WM_TIMER:
            switch (wp)
            {
                case _TIMER_ID_CHECKTHINKRESULT:
                    if (p_engine->GetResult() >= 0)
                    {
                        KillTimer(hWnd, _TIMER_ID_CHECKTHINKRESULT);
                        p_game->PutCoin(p_engine->GetResult());
                        delete p_engine;
                        p_engine = NULL;
                        InvalidateRect(hWnd_main_gui, NULL, FALSE);
                    }
                    break;
            }
            return 0;




        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}

#undef _TIMER_ID_CHECKTHINKRESULT




