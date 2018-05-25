#include "linuxUI/linuxUI.h"

/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWND window)
{
    return (BOOL) gtk_window_has_toplevel_focus(GTK_WINDOW(window));
}