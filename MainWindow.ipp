

#pragma once

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib" )

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "MainWindow.hpp"

#define INCLUDE_4INAROW
#include "game/minigames.hpp"
#include "engine/engine.hpp"

#include "MainGUI.hpp"

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
    winc.lpszMenuName  = NULL;
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
            p_game->Undo();
            while (p_game->GetWhichTurn() != 1)
            {
                p_game->Undo();
            }
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




