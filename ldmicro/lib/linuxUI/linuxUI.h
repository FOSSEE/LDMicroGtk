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
#define MB_OK 0x00000001L
#define MB_OKCANCEL 0x00000002L
#define MB_YESNO 0x00000004L
#define MB_YESNOCANCEL 0x00000008L

#define IDOK 1
#define IDCANCEL 2
#define IDYES 3
#define IDNO 4

#define MB_ICONERROR 0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONWARNING 0x00000040L
#define MB_ICONINFORMATION 0x00000080L

/// open/save file
#define OFN_PATHMUSTEXIST 0x00000100L
#define OFN_HIDEREADONLY 0x00000200L
#define OFN_OVERWRITEPROMPT 0x00000400L


/// data types
typedef struct OpenFileInfoData {
    DWORD         lStructSize;
    HWID          parentWindow;
    LPTSTR        lpstrFile;
    LPCTSTR       lpstrFilter;
    DWORD         nMaxFile;
    LPCTSTR       lpstrTitle;
    DWORD         Flags;
    LPCTSTR       lpstrDefExt;
} OPENFILENAME;

/// functions
BOOL isFocus(HWID);
COLORREF RGB(int, int, int);
int MessageBox(HWID, char*, char*, UINT);
BOOL GetSaveFileName(OPENFILENAME* );

#endif