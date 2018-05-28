#ifndef __LINUX_UI__
#define __LINUX_UI__

/// includes
#include <gtk/gtk.h>
#include <linux/limits.h>
#include <stdio.h>
#include <inttypes.h>
#include "linuxUI/linuxLD.h"

/// version control
#define LDMicro_VERSION_MAJOR 1
#define LDMicro_VERSION_MINOR 0

/// functions
BOOL isFocus(HWND);
COLORREF RGB(int, int, int);

#endif