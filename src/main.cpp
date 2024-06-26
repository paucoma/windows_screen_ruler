/**
 * @file main.cpp
 * @author Ahad Rauf (ahadrauf@stanford.edu)
 * @brief A simple program to find the distances and angles between points on a Windows screen. No bloatware, and easy to use.
 * @version 1.0
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022 Ahad Rauf. Licensed under MIT License.
 * 
 */
#ifndef UNICODE
#define UNICODE
#endif

#include <stdint.h>

#include <iostream>
#include <cmath>
#include <windows.h>
#include <windef.h>
#include <winuser.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <objidl.h>
#pragma comment (lib, "Gdiplus.lib")

#include "menu.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

struct RulerInfo {
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t clicked_x1;
    uint16_t clicked_y1;
    uint16_t clicked_x2;
    uint16_t clicked_y2;
    uint8_t currPoint;
};
RulerInfo *pState = new (std::nothrow) RulerInfo;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
void updateMousePosition(HWND hwnd, RulerInfo *pState, bool updateClickedPoints);

HINSTANCE hinst;  //Used for Menu

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Main Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    RegisterClass(&wc);

    hinst = hInstance;  //Used for Menu

    // Define the state
    if (pState == NULL) {
        return 0;
    } else {
        pState->pos_x = 0;
        pState->pos_y = 0;
        pState->clicked_x1 = 0;
        pState->clicked_y1 = 0;
        pState->clicked_x2 = 0;
        pState->clicked_y2 = 0;
        pState->currPoint = 0;
    }

    // Create the window.
    // 10, 10, 300, 150,
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Screen Ruler",                // Window text
        WS_OVERLAPPEDWINDOW,            // Window style, prev WS_OVERLAPPEDWINDOW

        // Position and Size
        10, 10, GetSystemMetrics(SM_CXSCREEN) - 20, GetSystemMetrics(SM_CYSCREEN) - 20,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        pState        // Additional application data
        );

    if (hwnd == NULL) {
        return 0;
    }

    // SetCursor(LoadCursor(hInstance, IDC_ARROW));

    SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), (BYTE)150, LWA_ALPHA);
    // SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

// [Writing the Window Procedure](https://learn.microsoft.com/en-us/windows/win32/learnwin32/writing-the-window-procedure)
// [Window Notifications](https://learn.microsoft.com/en-us/windows/win32/winmsg/window-notifications)
// https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#system-defined-messages
// https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#application-defined-messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RulerInfo *pState;
    if (uMsg == WM_CREATE) {
        // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-create
        // Sent when an application requests that a window be created by calling the CreateWindowEx or CreateWindow function. 
        //  (The message is sent before the function returns.)
        //  This is basically our initialization, we create our Struct
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pState = reinterpret_cast<RulerInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
    } else {
        // Extract Ruler Info
        LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        pState = reinterpret_cast<RulerInfo*>(ptr);
    }
   
    switch (uMsg)
    {        
    case WM_DESTROY:
        //https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
        //  Sent when a window is being destroyed.
        //  End of Our Application
        PostQuitMessage(0);
        return 0;
    
    case WM_MOUSEMOVE:
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
        {
            pState->pos_x = GET_X_LPARAM(lParam);
            pState->pos_y = GET_Y_LPARAM(lParam);
            updateMousePosition(hwnd, pState, false);
        }
        return 0;

    case WM_LBUTTONDOWN:
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondown
        {
            pState->pos_x = GET_X_LPARAM(lParam);
            pState->pos_y = GET_Y_LPARAM(lParam);
            if (pState->currPoint == 0) {
                pState->clicked_x1 = pState->pos_x;
                pState->clicked_y1 = pState->pos_y;
                pState->currPoint = 1;
            } else {
                pState->clicked_x2 = pState->pos_x;
                pState->clicked_y2 = pState->pos_y;
                pState->currPoint = 0;
            }
            updateMousePosition(hwnd, pState, true);
            // HWND hwnd = FindWindowA("Main Window Class", "Screen Ruler");
            // UpdateWindow(hwnd);
        }
        return 0;

    case WM_PAINT:
        // https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            // FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(255, 255, 255)));
            TCHAR text[60];
            swprintf_s(text, 60, L"(%d, %d)      ", pState->pos_x, pState->pos_y);
            TextOut(hdc, 10, 10, text, wcslen(text));

            swprintf_s(text, 60, L"Clicked Pt1: (%d, %d)      ", pState->clicked_x1, pState->clicked_y1);
            TextOut(hdc, 10, 25, text, wcslen(text));

            swprintf_s(text, 60, L"Clicked Pt2: (%d, %d)      ", pState->clicked_x2, pState->clicked_y2);
            TextOut(hdc, 10, 40, text, wcslen(text));

            if (pState->clicked_x2 != 0 && pState->clicked_y2 != 0) {
                int distanceX = pState->clicked_x2 - pState->clicked_x1;
                int distanceY = pState->clicked_y1 - pState->clicked_y2;
                double distance = sqrt(distanceX*distanceX + distanceY*distanceY);
                double angle = atan2(distanceY, distanceX) * 180 / M_PI;
                swprintf_s(text, 60, L"Distance: (%d, %d): %f px      ", distanceX, distanceY, distance);
                TextOut(hdc, 10, 55, text, wcslen(text));
                swprintf_s(text, 60, L"Angle: %f degrees      ", angle);
                TextOut(hdc, 10, 70, text, wcslen(text));
            } else {
                swprintf_s(text, 60, L"Distance: (N/A, N/A): %s px      ", L"N/A");
                TextOut(hdc, 10, 55, text, wcslen(text));
                swprintf_s(text, 60, L"Angle: %s degrees      ", L"N/A");
                TextOut(hdc, 10, 70, text, wcslen(text));
            }

            swprintf_s(text, 60, L"Current Pt: %d      ", pState->currPoint);
            TextOut(hdc, 10, 85, text, wcslen(text));

            EndPaint(hwnd, &ps);
        }
        return 0;
    case WM_CONTEXTMENU:
    // https://learn.microsoft.com/en-us/windows/win32/menurc/using-menus#processing-the-wm_contextmenu-message
        {
            if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) 
                return DefWindowProc(hwnd, uMsg, wParam, lParam); 
        }
        break;
    case WM_COMMAND:
        switch ((wParam))
        {
            case ID_FILE_ABOUT:
                MessageBox(hwnd, L"About menu item clicked", L"Notice", MB_OK | MB_ICONINFORMATION);
            break;
            case ID_FILE_EXIT:
                PostQuitMessage(0);
                return 0;
            break;
        }
        // break;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
    //  If you don't handle a particular message in your window procedure, 
    //  pass the message parameters directly to the DefWindowProc function. 
    //  This function performs the default action for the message, which varies by message type.
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void updateMousePosition(HWND hwnd, RulerInfo *pState, bool updateClickedPoints) {
    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);
    if (updateClickedPoints) {
        graphics.Clear(Gdiplus::Color(255, 255, 255));
    }

    // PAINTSTRUCT ps;
    // FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(255, 255, 255)));
    TCHAR text[60];
    swprintf_s(text, 60, L"(%d, %d)      ", pState->pos_x, pState->pos_y);
    TextOut(hdc, 10, 10, text, wcslen(text));

    swprintf_s(text, 60, L"Clicked Pt1: (%d, %d)      ", pState->clicked_x1, pState->clicked_y1);
    TextOut(hdc, 10, 25, text, wcslen(text));

    swprintf_s(text, 60, L"Clicked Pt2: (%d, %d)      ", pState->clicked_x2, pState->clicked_y2);
    TextOut(hdc, 10, 40, text, wcslen(text));

    if (pState->clicked_x2 != 0 || pState->clicked_y2 != 0) {
        int distanceX = pState->clicked_x2 - pState->clicked_x1;
        int distanceY = pState->clicked_y1 - pState->clicked_y2;
        double distance = sqrt(distanceX*distanceX + distanceY*distanceY);
        double angle = atan2(distanceY, distanceX) * 180 / M_PI;
        swprintf_s(text, 60, L"Distance: (%d, %d): %f px      ", distanceX, distanceY, distance);
        TextOut(hdc, 10, 55, text, wcslen(text));
        swprintf_s(text, 60, L"Angle: %f degrees      ", angle);
        TextOut(hdc, 10, 70, text, wcslen(text));
    } else {
        swprintf_s(text, 60, L"Distance: (N/A, N/A): %s px      ", L"N/A");
        TextOut(hdc, 10, 55, text, wcslen(text));
        swprintf_s(text, 60, L"Angle: %s degrees      ", L"N/A");
        TextOut(hdc, 10, 70, text, wcslen(text));
    }

    swprintf_s(text, 60, L"Current Pt: %d      ", pState->currPoint);
    TextOut(hdc, 10, 85, text, wcslen(text));

    if (updateClickedPoints) {
        if (pState->clicked_x2 != 0 || pState->clicked_y2 != 0) {
            Gdiplus::Pen blackPen(Gdiplus::Color(255, 0, 0, 0), 1);
            graphics.DrawLine(&blackPen, pState->clicked_x1, pState->clicked_y1, pState->clicked_x2, pState->clicked_y2);
            Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0), 3);
            graphics.DrawLine(&pen, pState->clicked_x2 - 5, pState->clicked_y2 - 5, pState->clicked_x2 + 5, pState->clicked_y2 + 5);
            graphics.DrawLine(&pen, pState->clicked_x2 + 5, pState->clicked_y2 - 5, pState->clicked_x2 - 5, pState->clicked_y2 + 5);
        }
        if (pState->clicked_x1 != 0 || pState->clicked_y1 != 0) {
            Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 255), 3);
            graphics.DrawLine(&pen, pState->clicked_x1 - 5, pState->clicked_y1 - 5, pState->clicked_x1 + 5, pState->clicked_y1 + 5);
            graphics.DrawLine(&pen, pState->clicked_x1 + 5, pState->clicked_y1 - 5, pState->clicked_x1 - 5, pState->clicked_y1 + 5);
        }
    }

    ReleaseDC(hwnd, hdc);
}

/*
BOOL WINAPI OnContextMenu(HWND hwnd, int x, int y) 
{ 
    RECT rc;                    // client area of window 
    POINT pt = { x, y };        // location of mouse click 
 
    // Get the bounding rectangle of the client area. 
 
    GetClientRect(hwnd, &rc); 
 
    // Convert the mouse position to client coordinates. 
 
    ScreenToClient(hwnd, &pt); 
 
    // If the position is in the client area, display a  
    // shortcut menu. 
 
    if (PtInRect(&rc, pt)) 
    { 
        ClientToScreen(hwnd, &pt); 
        DisplayContextMenu(hwnd, pt); 
        return TRUE; 
    } 
 
    // Return FALSE if no menu is displayed. 
 
    return FALSE; 
} 



VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt) 
{ 
 
 //   HMENU hmenu;            // top-level menu 
 //   HMENU hmenuTrackPopup;  // shortcut menu 
 
    // Load the menu resource. 
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadmenua
 //   if ((hmenu = LoadMenu(hinst, "ShortcutExample")) == NULL) 
 //       return; 
 
    //HMENU hmenuTrackPopup = CreateMenu();
    // main menu
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createpopupmenu
    HMENU hMenu = CreatePopupMenu();

    // menu items
    AppendMenu(hMenu, MF_STRING, ID_FILE_ABOUT, L"About");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
    //AppendMenu(hmenuTrackPopup, MF_POPUP, (UINT_PTR)hMenu, (LPCWSTR)"Dummy Popup");
    // TrackPopupMenu cannot display the menu bar so get 
    // a handle to the first shortcut menu. 
 
    //hmenuTrackPopup = GetSubMenu(hMenu, 0); 
 
    // Display the shortcut menu. Track the right mouse 
    // button. 
 
    //TrackPopupMenu(hmenuTrackPopup, 
    //https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-trackpopupmenu
    TrackPopupMenu(hMenu, 
            TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
            pt.x, pt.y, 0, hwnd, NULL); 
 
    // Destroy the menu. 
 
    DestroyMenu(hMenu); 
} 
*/