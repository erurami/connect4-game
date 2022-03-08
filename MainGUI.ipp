
#pragma once

#include "WinColor/WinColor.hpp"
#include "MainGUI.hpp"

#include "MainWindow.hpp"

#include <windows.h>



LRESULT CALLBACK _Connect4GuiGuiWndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

int _Connect4GuiRegisterMainGuiWndClass(LPTSTR strWndClassName)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS winc;

    winc.style         = 0;
    winc.lpfnWndProc   = _Connect4GuiGuiWndProc;
    winc.cbClsExtra    = 0;
    winc.cbWndExtra    = 0;
    winc.hInstance     = hInstance;
    winc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    winc.lpszMenuName  = NULL;
    winc.lpszClassName = strWndClassName;

    if (GetClassInfo(hInstance, winc.lpszClassName, &winc))
    {
    }
    else
    {
        if (!RegisterClass(&winc))
        {
            return 1;
        }
    }
    return 0;
}


#define _BUTTON_ID_NEWGAME 1
#define _BUTTON_ID_UNDO    2
#define _BUTTON_ID_STOP    3

typedef struct __Connect4GuiGamePaintInfos
{
    // win32 vars
    HWND m_hWndMain;

    // style vars
    RECT m_clientRect;
    RECT m_boardRect;

    // game status vars
    bool m_isInitialized;
    int  m_gameState;

    // no ghost
    int  m_ghostCoinColumn;

} _Connect4GuiGamePaintInfos;

void _Connect4GuiGuiDrawGameBoard(HDC hdc, _Connect4GuiGamePaintInfos* pPaintInfos);

void _Connect4GuiBiggestRectangle(LPRECT pRect, int x, int y);


LRESULT CALLBACK _Connect4GuiGuiWndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{
    // api vars
    static HINSTANCE hInstance;

    static HWND hWnd_parent;

    static HWND hWnd_button_newgame;
    static HWND hWnd_button_undo;
    static HWND hWnd_button_stop_thinking;

    static HFONT hFont_main;


    // style vars
    static int toolbar_height = 50;

    static RECT rect_client;
    static RECT rect_game_board;

    static bool game_board_need_recalculate;

    // game vars
    static bool is_game_initialized;

    static int game_state;
    // 0 : uninitialized
    // 1 : user turn
    // 2 : bot turn
    // 3 : user won
    // 4 : bot won
    // 5 : draw

    static int game_board_width;
    static int game_board_height;

    switch (msg)
    {

        case WM_CREATE:
            GetClientRect(hWnd, &rect_client);
            game_board_need_recalculate = true;

            hInstance = GetModuleHandle(NULL);
            hWnd_parent = GetParent(hWnd);

            is_game_initialized = (SendMessage(hWnd_parent, _C4CM_ISINITIALIZED, 0, 0) == TRUE);

            hFont_main = CreateFont(
                    32, 0,
                    0, 0, FW_NORMAL,
                    FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    PROOF_QUALITY,
                    VARIABLE_PITCH | FF_SWISS,
                    NULL
                    );


            hWnd_button_newgame = CreateWindow(
                    TEXT("BUTTON"), TEXT("New"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    0, 0, 50, toolbar_height,
                    hWnd, (HMENU)_BUTTON_ID_NEWGAME, hInstance, NULL
                    );

            hWnd_button_undo = CreateWindow(
                    TEXT("BUTTON"), TEXT("Undo"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    50, 0, 50, toolbar_height,
                    hWnd, (HMENU)_BUTTON_ID_UNDO, hInstance, NULL
                    );

            hWnd_button_stop_thinking = CreateWindow(
                    TEXT("BUTTON"), TEXT("Stop"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    100, 0, 50, toolbar_height,
                    hWnd, (HMENU)_BUTTON_ID_STOP, hInstance, NULL
                    );

            InvalidateRect(hWnd, NULL, FALSE);

            ShowWindow(hWnd, SW_SHOW);
            return 0;


        case WM_DESTROY:
            DeleteObject(hFont_main);
            return 0;




        case WM_COMMAND:
            switch (LOWORD(wp))
            {

                case _BUTTON_ID_NEWGAME:
                    SendMessage(hWnd_parent, _C4CM_INITIALIZEWITHWZD, 0, 0);
                    break;

                case _BUTTON_ID_UNDO:
                    SendMessage(hWnd_parent, _C4CM_UNDO, 0, 0);
                    break;

                case _BUTTON_ID_STOP:
                    SendMessage(hWnd_parent, _C4CM_STOPTHINKING, 0, 0);

            }
            return 0;


        case WM_LBUTTONUP:
            if (LOWORD(lp) >= rect_game_board.left && LOWORD(lp) < rect_game_board.right  &&
                HIWORD(lp) >= rect_game_board.top  && HIWORD(lp) < rect_game_board.bottom &&
                game_state == 1)
            {
                int cell_width;
                cell_width = (rect_game_board.right - rect_game_board.left) / (int)SendMessage(hWnd_parent, _C4CM_GETGAMEWIDTH, 0, 0);
                int column_put;
                column_put = (LOWORD(lp) - rect_game_board.left) / cell_width;

                PostMessage(hWnd_parent, _C4CM_PUTCOIN, column_put, 0);
            }
            return 0;




        case WM_SIZE:
            GetClientRect(hWnd, &rect_client);
            game_board_need_recalculate = true;
            SendMessage(hWnd, _C4GWM_REDRAW, 0, 0);
            return 0;


        case WM_PAINT:
            SendMessage(hWnd, _C4GWM_REDRAW, 0, 0);
            return 0;




        case WM_ERASEBKGND:
            return 1;




        case _C4GWM_REDRAW:
            {
            is_game_initialized = (SendMessage(hWnd_parent, _C4CM_ISINITIALIZED, 0, 0) == TRUE);

            _Connect4GuiGamePaintInfos paint_infos;


            if (is_game_initialized)
            {
                int game_board_width_now  = (int)SendMessage(hWnd_parent, _C4CM_GETGAMEWIDTH , 0, 0);
                int game_board_height_now = (int)SendMessage(hWnd_parent, _C4CM_GETGAMEHEIGHT, 0, 0);

                game_board_need_recalculate = game_board_need_recalculate || (game_board_width_now != game_board_width) || (game_board_height_now != game_board_height);

                game_board_width  = game_board_width_now;
                game_board_height = game_board_height_now;

                if (SendMessage(hWnd_parent, _C4CM_GETSTATE, 0, 0) == 0)
                {
                    game_state = (int)SendMessage(hWnd_parent, _C4CM_GETTURN, 0, 0);
                }
                else
                {
                    game_state = (int)SendMessage(hWnd_parent, _C4CM_GETSTATE, 0, 0) + 2;
                }
            }
            else
            {
                game_state = 0;
            }

            if (game_board_need_recalculate)
            {
                rect_game_board = rect_client;
                rect_game_board.top = toolbar_height;
                _Connect4GuiBiggestRectangle(&rect_game_board, game_board_width, game_board_height);
                game_board_need_recalculate = false;
            }

            paint_infos.m_hWndMain = hWnd_parent;

            paint_infos.m_clientRect = rect_client;
            paint_infos.m_boardRect = rect_game_board;

            paint_infos.m_gameState = game_state;
            paint_infos.m_isInitialized = is_game_initialized;



            HDC hdc_virtual;
            HDC hdc;

            hdc = GetDC(hWnd);

            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect_client.right - rect_client.left, rect_client.bottom - rect_client.top);

            hdc_virtual = CreateCompatibleDC(hdc);
            SelectObject(hdc_virtual, hBitmap);

            SelectObject(hdc_virtual, hFont_main);

            _Connect4GuiGuiDrawGameBoard(hdc_virtual, &paint_infos);

            ReleaseDC(hWnd, hdc);
            DeleteObject(hBitmap);

            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);

            BitBlt(
                    hdc,
                    0, 0,
                    rect_client.right  - rect_client.left,
                    rect_client.bottom - rect_client.top,
                    hdc_virtual,
                    0, 0,
                    SRCCOPY
                    );

            EndPaint(hWnd, &ps);


            switch (game_state)
            {
                case 3:
                case 4:
                case 5:
                    EnableWindow(hWnd_button_newgame      , TRUE );
                    EnableWindow(hWnd_button_undo         , TRUE );
                    EnableWindow(hWnd_button_stop_thinking, FALSE);
                    break;
                case 0:
                    EnableWindow(hWnd_button_newgame      , TRUE );
                    EnableWindow(hWnd_button_undo         , FALSE);
                    EnableWindow(hWnd_button_stop_thinking, FALSE);
                    break;
                case 1:
                    EnableWindow(hWnd_button_newgame      , TRUE );
                    EnableWindow(hWnd_button_undo         , TRUE );
                    EnableWindow(hWnd_button_stop_thinking, FALSE);
                    break;
                case 2:
                    EnableWindow(hWnd_button_newgame      , TRUE );
                    EnableWindow(hWnd_button_undo         , FALSE);
                    EnableWindow(hWnd_button_stop_thinking, TRUE );
                    break;
            }

            return 0;
            }




        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}

#undef _BUTTON_ID_NEWGAME
#undef _BUTTON_ID_UNDO
#undef _BUTTON_ID_STOP

COLORREF _Connect4GuiGuiBlendColor(COLORREF color1, COLORREF color2, int ratio1, int ratio2);

void _Connect4GuiGuiDrawTextCenter(HDC hdc, LPTSTR string, RECT rect);

void _Connect4GuiGuiDrawGameBoard(HDC hdc, _Connect4GuiGamePaintInfos* pPaintInfos)
{
    HBRUSH hBrush_background = CreateSolidBrush(GetBlendedTheme("B9B4F2"));

    FillRect(hdc, &pPaintInfos->m_clientRect, hBrush_background);

    DeleteObject(hBrush_background);


    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, GetBlendedTheme("F9F4B2"));



    TEXTMETRIC textmetric;
    GetTextMetrics(hdc, &textmetric);

    if (pPaintInfos->m_isInitialized == false)
    {
        _Connect4GuiGuiDrawTextCenter(hdc, TEXT("Click \"new\" to start new game"), pPaintInfos->m_boardRect);
        return;
    }


    COLORREF color_Player1 = _Connect4GuiGuiBlendColor(GetBlendedTheme("F3B1"), RGB(255, 0, 0), 1, 5);
    COLORREF color_Player2 = _Connect4GuiGuiBlendColor(GetBlendedTheme("F3B1"), RGB(0, 255, 0), 1, 5);
    COLORREF color_Line    = GetBlendedTheme("A5F1");

    if (pPaintInfos->m_gameState >= 3)
    {
        color_Player1 = _Connect4GuiGuiBlendColor(color_Player1, GetBlendedTheme("F1B5"), 1, 3);
        color_Player2 = _Connect4GuiGuiBlendColor(color_Player2, GetBlendedTheme("F1B5"), 1, 3);
        color_Line    = _Connect4GuiGuiBlendColor(color_Line   , GetBlendedTheme("F1B5"), 1, 3);
    }

    HWND hWnd_main = pPaintInfos->m_hWndMain;

    int game_width  = (int)SendMessage(hWnd_main, _C4CM_GETGAMEWIDTH , 0, 0);
    int game_height = (int)SendMessage(hWnd_main, _C4CM_GETGAMEHEIGHT, 0, 0);

    int board_x_length = (pPaintInfos->m_boardRect.right  - pPaintInfos->m_boardRect.left);
    int board_y_length = (pPaintInfos->m_boardRect.bottom - pPaintInfos->m_boardRect.top );

    int cell_width  = board_x_length / game_width;
    int cell_height = board_y_length / game_height;

    int cell_roundness = CONNECT4_CELL_ROUNDNESS;
    int cell_gap = CONNECT4_CELL_GAP;

    HBRUSH hBrush_Player1 = CreateSolidBrush(color_Player1);
    HBRUSH hBrush_Player2 = CreateSolidBrush(color_Player2);

    HPEN hPen_Line = CreatePen(PS_SOLID, 0, color_Line);

    for (int x = 0; x < game_width; x++)
    {
        for (int y = 0; y < game_height; y++)
        {
            int cell_left   = pPaintInfos->m_boardRect.left + cell_width  * x + cell_gap;
            int cell_top    = pPaintInfos->m_boardRect.top  + cell_height * y + cell_gap;
            int cell_right  = pPaintInfos->m_boardRect.left + cell_width  * (x + 1) - cell_gap;
            int cell_bottom = pPaintInfos->m_boardRect.top  + cell_height * (y + 1) - cell_gap;

            SelectObject(hdc, hPen_Line);
            SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));

            RoundRect(
                    hdc,
                    cell_left, cell_top, cell_right, cell_bottom,
                    cell_roundness, cell_roundness
                    );

            SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
            switch (SendMessage(hWnd_main, _C4CM_GETAT, x, y))
            {
                case 1:
                    SelectObject(hdc, hBrush_Player1);
                    break;
                case 2:
                    SelectObject(hdc, hBrush_Player2);
                    break;
            }

            Ellipse(
                    hdc,
                    cell_left + 2, cell_top + 2, cell_right - 1, cell_bottom - 1
                   );
        }
    }

    DeleteObject(hBrush_Player1);
    DeleteObject(hBrush_Player2);

    DeleteObject(hPen_Line);

    switch (pPaintInfos->m_gameState)
    {
        case 3:
            _Connect4GuiGuiDrawTextCenter(hdc, TEXT("You won!\nClick \"new\" to start new game."), pPaintInfos->m_boardRect);
            break;
        case 4:
            _Connect4GuiGuiDrawTextCenter(hdc, TEXT("Bot won!\nClick \"new\" to start new game."), pPaintInfos->m_boardRect);
            break;
        case 5:
            _Connect4GuiGuiDrawTextCenter(hdc, TEXT("draw.\nClick \"new\" to start new game."), pPaintInfos->m_boardRect);
            break;
    }
}

void _Connect4GuiGuiDrawTextCenter(HDC hdc, LPTSTR string, RECT rect)
{
    RECT rect_text = rect;

    DrawText(hdc, string, -1, &rect_text, DT_CALCRECT);

    int text_width  = rect_text.right  - rect_text.left;
    int text_height = rect_text.bottom - rect_text.top;

    int rect_width  = rect.right  - rect.left;
    int rect_height = rect.bottom - rect.top;

    rect.left += (rect_width  - text_width ) / 2;
    rect.top  += (rect_height - text_height) / 2;

    rect.right  = rect.left + text_width;
    rect.bottom = rect.top  + text_height;

    DrawText(hdc, string, -1, &rect, DT_CENTER);
}

COLORREF _Connect4GuiGuiBlendColor(COLORREF color1, COLORREF color2, int ratio1, int ratio2)
{
    int result_r = 0;
    int result_g = 0;
    int result_b = 0;

    result_r +=  GetRValue(color1) * ratio1;
    result_r +=  GetRValue(color2) * ratio2;
    result_r /= (ratio1 + ratio2);

    result_g +=  GetGValue(color1) * ratio1;
    result_g +=  GetGValue(color2) * ratio2;
    result_g /= (ratio1 + ratio2);

    result_b +=  GetBValue(color1) * ratio1;
    result_b +=  GetBValue(color2) * ratio2;
    result_b /= (ratio1 + ratio2);

    return RGB(result_r, result_g, result_b);
}


void _Connect4GuiBiggestRectangle(LPRECT pRect, int x, int y)
{
    if (x == 0 || y == 0)
    {
        return;
    }

    int frame_width  = pRect->right  - pRect->left;
    int frame_height = pRect->bottom - pRect->top;

    int biggest_width  = frame_width;
    int biggest_height = frame_width * y / x;

    if (biggest_height > frame_height)
    {
        biggest_width  = frame_height * x / y;
        biggest_height = frame_height;
    }


    pRect->left = pRect->left + frame_width  / 2 - biggest_width  / 2;
    pRect->top  = pRect->top  + frame_height / 2 - biggest_height / 2;

    pRect->right  = pRect->left + biggest_width ;
    pRect->bottom = pRect->top  + biggest_height;
}




