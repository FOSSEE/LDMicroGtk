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
typedef GdkRGBA COLORREF;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef void* PVOID;
typedef PVOID HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HANDLE HMENU;

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