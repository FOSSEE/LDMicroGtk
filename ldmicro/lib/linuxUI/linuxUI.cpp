#include "linuxUI.h"

/// Menu Variables
 HWID  window;
 
/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWID window)
{
    return (BOOL) gtk_window_has_toplevel_focus(GTK_WINDOW(window));
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

// BOOL GetSaveFileName(OPENFILENAME ofn)
// {
//     GtkWidget *dialog;
//     GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

//     dialog = gtk_file_chooser_dialog_new ("Open File",
//                                         GTK_WINDOW(window),
//                                         action,
//                                         "_Cancel",
//                                         GTK_RESPONSE_CANCEL,
//                                         "_Open",
//                                         GTK_RESPONSE_ACCEPT,
//                                         NULL);

//     GtkFileFilter *filter = gtk_file_filter_new ();
//     gtk_file_filter_add_pattern (filter, "*");
//     gtk_file_filter_set_name (filter, "All files");
//     gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);

//     filter = gtk_file_filter_new ();
//     gtk_file_filter_add_pattern (filter, "*.c");
//     gtk_file_filter_add_pattern (filter, "*.cpp");
//     gtk_file_filter_set_name (filter, "C source files");
//     gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);

//     filter = gtk_file_filter_new ();
//     gtk_file_filter_add_pattern (filter, "*.h");
//     gtk_file_filter_set_name (filter, "C hedder files");
//     gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    
//     filter = gtk_file_filter_new ();
//     gtk_file_filter_add_pattern (filter, "*.hex");
//     gtk_file_filter_set_name (filter, "hex files");
//     gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    
//     filter = gtk_file_filter_new ();
//     gtk_file_filter_add_pattern (filter, "*.int");
//     gtk_file_filter_set_name (filter, "int files");
//     gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
//     gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (dialog), filter);
    
//     BOOL exitStatus = gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT;
//     if (exitStatus)
//         filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
//     gtk_widget_destroy (dialog);

//     return exitStatus;
// }