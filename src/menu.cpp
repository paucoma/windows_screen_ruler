#include "menu.h"

#ifndef UNICODE
#define UNICODE
#endif

//#include <stdint.h>
//#include <iostream>
//#include <winuser.h>
//#include <windows.h>
//#include <windowsx.h>
//#include <windef.h>

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
 /*
    HMENU hmenu;            // top-level menu 
    HMENU hmenuTrackPopup;  // shortcut menu 
 
    // Load the menu resource. 
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadmenua
    if ((hmenu = LoadMenu(hinst, "ShortcutExample")) == NULL) 
        return; 
 */
    //HMENU hmenuTrackPopup = CreateMenu();
    // main menu
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createpopupmenu
    HMENU hMenu = CreatePopupMenu();

    // menu items
    AppendMenu(hMenu, MF_STRING, ID_FILE_ABOUT, TEXT("About"));
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_FILE_EXIT, TEXT("Exit"));
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