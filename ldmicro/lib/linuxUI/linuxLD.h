#ifndef __LINUX_LD__
#define __LINUX_LD__

#include "linuxUI.h"
#include <ctype.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <sys/mman.h>

/// common windows referances for linux

/// definitions
#define MAX_PATH PATH_MAX
/// CALLBACK or __stdcall os defined empty
#define CALLBACK
#define CONST const

/// Meamory flags
#define HEAP_ZERO_MEMORY 0x00000008

/// Image loading flags
#define IMAGE_ICON 1
#define LDMICRO_ICON "../ldmicro.ico"

/// Macro functions
#define max(_A, _B) std::max(_A, _B)

/// Typedefs
//typedef int64_t __int64;
typedef bool BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef size_t SIZE_T;
typedef long LONG;
typedef wchar_t WCHAR;
typedef char CHAR;

typedef CONST WCHAR *LPCWSTR;
typedef CONST CHAR *LPCSTR; /// should be __nullterminated
typedef WORD ATOM;

#ifdef UNICODE
 typedef LPCWSTR LPCTSTR; 
#else
 typedef LPCSTR LPCTSTR;
#endif

typedef WCHAR *LPWSTR;
typedef CHAR *LPSTR;

#ifdef UNICODE
 typedef LPWSTR LPTSTR;
#else
 typedef LPSTR LPTSTR;
#endif

typedef void *PVOID;
typedef void *LPVOID;
typedef PVOID HMODULE;
typedef PVOID HHOOK;

typedef PVOID HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HGDIOBJ;

typedef cairo_t *HCRDC;
typedef GtkWidget *HWID;
typedef GtkWindow *HWND;
typedef GtkTreeModel *HLIST;
typedef GtkTreeIter ITLIST;
typedef GtkApplication *HAPP;
typedef GtkTreeViewColumn *HTVC;
typedef GdkPixbuf *HICON;
typedef GdkRectangle GDRECT;
typedef GDRECT *PGDRECT;
typedef HWID HMENU;
typedef ITLIST *HITLIST;

/// Check if system is x64 or x86
#if defined(__UNIX64)
typedef uint64_t UINT_PTR;
#else 
typedef unsigned int UINT_PTR;
#endif
 
typedef UINT_PTR WPARAM;

#if defined(__UNIX64)
 typedef __int64_t LONG_PTR; 
#else
 typedef long LONG_PTR;
#endif

typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

/// Classes
typedef class tagColorReferance: public GdkRGBA{
    public:
    tagColorReferance()
    {
        this->red = 0.0;
        this->green = 0.0;
        this->blue = 0.0;
        this->alpha = 1.0;
    }

    tagColorReferance(int r, int g, int b)
    {
        this->red = r/255.0;
        this->green = g/255.0;
        this->blue = b/255.0;
        this->alpha = 1.0;
    }

    bool operator== (tagColorReferance& arg1)
    {
        if( ((int)arg1.red == (int)this->red) && ((int)arg1.green == (int)this->green) && ((int)arg1.blue == (int)this->blue) )
            return true;
        else
            return false;
    }

} COLORREF, *HBRUSH;

/// Structures
typedef struct HeapRecordChunckTag{
    PVOID Chunck;
    SIZE_T dwSize;
} HEAPCHUNCK;

typedef struct HeapRecordTag{
    PVOID hHeap;
    DWORD HeapID;
    std::vector<HEAPCHUNCK> Element;
    SIZE_T dwMaximumSize;
    SIZE_T dwSize;
    SIZE_T dwAllocatedSizeOffset;
} HEAPRECORD;

typedef struct tagSCROLLINFO {
    UINT cbSize;
    UINT fMask;
    int  nMin;
    int  nMax;
    UINT nPage;
    int  nPos;
    int  nTrackPos;
} SCROLLINFO, *LPCSCROLLINFO;

typedef struct {
  UINT   mask;
  int    iItem;
  int    iSubItem;
//   UINT   state;
//   UINT   stateMask;
  LPTSTR pszText;
//   int    cchTextMax;
//   int    iImage;
//   LPARAM lParam;
//   int    iIndent;
//   int    iGroupId;
//   UINT   cColumns;
//   PUINT  puColumns;
//   int    *piColFmt;
//   int    iGroup;
} LVITEM, *LPLVITEM;

typedef struct tagNMHDR {
    HLIST    hlistFrom;
    HITLIST  hlistIter;   
    UINT     code;
    LVITEM item;
} NMHDR;

typedef struct FontTag {
    int     nHeight;
    int     nWidth;
    int     nOrientation;
    int     fnWeight;
    DWORD   fdwItalic;
    LPTSTR lpszFace;
} *HFONT, FONT;

typedef struct tagLOGBRUSH {
  UINT      lbStyle;
  COLORREF  lbColor;
//   ULONG_PTR lbHatch;
} LOGBRUSH, *PLOGBRUSH;

typedef struct _RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT;

typedef struct SimpleDialogDataTag {
    UINT uflag;
    int boxes;
    char **dests;
    char *str1;
    char *str2;
    char *str3;
    int *num1;
    int *num2;
} SimpleDialogData;

/// Variables
extern std::vector<HEAPRECORD> HeapRecord;

/// Functions
HANDLE HeapCreate(
    DWORD  flOptions,
    SIZE_T dwInitialSize,
    SIZE_T dwMaximumSize);

LPVOID HeapAlloc(
    HANDLE hHeap,
    DWORD  dwFlags,
    SIZE_T dwBytes);

BOOL HeapFree(
    HANDLE hHeap,
    DWORD  dwFlags,
    LPVOID lpMem);

HICON LoadImage(
    HINSTANCE hinst,
    LPCTSTR   lpszName,
    UINT      uType,
    int       cxDesired,
    int       cyDesired,
    UINT      fuLoad
);

void RECT_to_GDRECT(
    const RECT *rc,
    GDRECT     *gdrc);

/// functions to be ported
void OutputDebugString(char*);
double GetTickCount(void);

#endif