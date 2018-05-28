#include "linuxUI/linuxUI.h"

/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWND window)
{
    return (BOOL) gtk_window_has_toplevel_focus(GTK_WINDOW(gtk_widget_get_parent_window(GTK_WIDGET(window))));
}

COLORREF RGB(int red, int green, int blue)
{
    COLORREF col;
    col.red = red/255.0;
    col.green = green/255.0;
    col.blue = blue/255.0;
    col.alpha = 1.0;

    return col;
}