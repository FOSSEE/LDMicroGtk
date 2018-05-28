#ifndef __LINUX_LD__
#define __LINUX_LD__

#include "linuxUI/linuxUI.h"

/// common windows referances for linux

/// definitions
#define MAX_PATH PATH_MAX
/// CALLBACK or __stdcall os defined empty
#define CALLBACK

/// typedefs
//typedef int64_t __int64;
typedef bool BOOL;
typedef unsigned char BYTE;
typedef unsigned int DWORD;

/// all handles will hold a GtkWindow* type
typedef void* PVOID;
typedef GtkWidget* HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HANDLE HMENU;

typedef GtkApplication* HAPP;

/// Check if system is x64 or x86
#if defined(__UNIX64)
typedef uint64_t UINT_PTR;
#else 
typedef unsigned int UINT_PTR;
#endif
 
typedef UINT_PTR WPARAM;
typedef unsigned int UINT;

/// custom classes
class COLORREF : public GdkRGBA{
    public:
    COLORREF()
    {
        this->red = 0.0;
        this->green = 0.0;
        this->blue = 0.0;
        this->alpha = 1.0;
    }
    COLORREF(int r, int g, int b)
    {
        this->red = r/255.0;
        this->green = g/255.0;
        this->blue = b/255.0;
        this->alpha = 1.0;
    }
};

#endif
