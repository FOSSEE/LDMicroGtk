#ifndef __windows_h_
#define __windoew_h_
#include "linuxUI.h"
#include <ctype.h>
typedef void* HFONT;
typedef void* HMODULE;
typedef void* HHOOK;
typedef void* HBRUSH;
typedef void* HFONT;

#if defined(__UNIX64)
 typedef __int64_t LONG_PTR; 
#else
 typedef long LONG_PTR;
#endif

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef struct tagSCROLLINFO {
  UINT cbSize;
  UINT fMask;
  int  nMin;
  int  nMax;
  UINT nPage;
  int  nPos;
  int  nTrackPos;
} SCROLLINFO, *LPCSCROLLINFO;

typedef struct tagNMHDR {
  HWND     hwndFrom;
  UINT_PTR idFrom;
  UINT     code;
} NMHDR;

void OutputDebugString(char*);
double GetTickCount(void);

#endif