

#ifndef _MENU_H
#define _MENU_H

#include <windows.h>

#define ID_FILE_ABOUT 1
#define ID_FILE_EXIT  2


BOOL WINAPI OnContextMenu(HWND hwnd, int x, int y);
VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt);

#endif