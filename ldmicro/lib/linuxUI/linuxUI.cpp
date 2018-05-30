#include "linuxUI.h"

/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWID window)
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

void MessageBox(HWID pWindow, char* message, char* title, UINT mFlags)
 {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkMessageType mType;
    GtkButtonsType mButtons;

    if (mFlags & MB_OK == MB_OK)
        mButtons = GTK_BUTTONS_OK;
    else if (mFlags & MB_OKCANCEL == MB_OKCANCEL)
        mButtons = GTK_BUTTONS_OK_CANCEL;
    else if (mFlags & MB_YESNO == MB_YESNO)
        mButtons = GTK_BUTTONS_YES_NO;
    else
        mButtons = GTK_BUTTONS_CLOSE;
    
    if (mFlags & MB_ICONERROR == MB_ICONERROR)
        mType = GTK_MESSAGE_ERROR;
    else if (mFlags & MB_ICONQUESTION == MB_ICONQUESTION)
        mType = GTK_MESSAGE_QUESTION;
    else if (mFlags & MB_ICONWARNING == MB_ICONWARNING)
        mType = GTK_MESSAGE_WARNING;
    else if (mFlags & MB_ICONINFORMATION == MB_ICONINFORMATION)
        mType = GTK_MESSAGE_INFO;
    else
        mType = GTK_MESSAGE_OTHER;

    mType = GTK_MESSAGE_ERROR;
    HWID dialog = gtk_message_dialog_new (GTK_WINDOW(pWindow),
                                    flags,
                                    mType,
                                    mButtons,
                                    message);
    gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG(dialog),
                               title);
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
 }