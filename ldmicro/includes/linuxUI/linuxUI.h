#ifndef __LINUX_UI__
#define __LINUX_UI__

/// includes
#include <gtk/gtk.h>
#include <linux/limits.h>
#include <stdio.h>
#include <inttypes.h>

/// version control
#define LDMicro_VERSION_MAJOR 1
#define LDMicro_VERSION_MINOR 0

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

/*
/// Check if system is x64 or x86 using GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
/// system is x64
typedef unsigned __int64 UINT_PTR;
#else
/// system is x86
typedef unsigned int UINT_PTR;
#endif
#endif
*/

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

/// common windows referances for linux - end

/// functions
BOOL isFocus(HWND);
COLORREF RGB(int, int, int);

#endif