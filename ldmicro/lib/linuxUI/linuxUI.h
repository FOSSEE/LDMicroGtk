#ifndef __LINUX_UI__
#define __LINUX_UI__

/// includes
#include <gtk/gtk.h>
#include <linux/limits.h>
#include <stdio.h>
#include <inttypes.h>
#include "linuxLD.h"

/// version control
#define LDMicro_VERSION_MAJOR 1
#define LDMicro_VERSION_MINOR 0

/// Flags
/// message box
#define MB_OK 0x00000000L
#define MB_OKCANCEL 0x00000001L
#define MB_YESNO 0x00000004L

#define MB_ICONERROR 0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONWARNING 0x00000030L
#define MB_ICONINFORMATION 0x00000040L

/// open/save file
#define OFN_PATHMUSTEXIST 0x00000800
#define OFN_HIDEREADONLY 0x00000004
#define OFN_OVERWRITEPROMPT 0x00000002


/// data types
typedef struct OpenFileInfoData {
    DWORD         lStructSize;
    HWID          parentWindow;
    LPCTSTR       lpstrFilter;
    LPTSTR        lpstrFile;
    DWORD         nMaxFile;
    LPCTSTR       lpstrTitle;
    DWORD         Flags;
    LPCTSTR       lpstrDefExt;
} OPENFILENAME;

/// functions
BOOL isFocus(HWID);
COLORREF RGB(int, int, int);
void MessageBox(HWID, char*, char*, UINT);
BOOL GetSaveFileName(HWID, char*);

#endif