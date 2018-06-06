#include "linuxUI.h"

/// EnableMenuItem Variables
const UINT MF_ENABLED = 0;
const UINT MF_GRAYED = 1;
const UINT MF_CHECKED = 2;
const UINT MF_UNCHECKED = 3;

/// ListStore
GtkWidget* view;
GtkTreeViewColumn* column;
 
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

BOOL GetSaveFileName(OPENFILENAME *ofn)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new (ofn->lpstrTitle,
                                        GTK_WINDOW(ofn->parentWindow),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    //g_print("filter created\n");
    
    if (ofn->Flags & OFN_OVERWRITEPROMPT == OFN_OVERWRITEPROMPT)
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    GtkFileFilter *filter = gtk_file_filter_new ();
    char* strFilter = new char[strlen(ofn->lpstrFilter)];
    DWORD strFilterLen = 0;
    BOOL filterResetFlag = FALSE;
    
    for (int i = 0; !(ofn->lpstrFilter[i] == '\0' && ofn->lpstrFilter[i-1] == '\0'); ++i)   
    {
        memcpy (strFilter + strFilterLen, &ofn->lpstrFilter[i], 1 );
        ++strFilterLen;
        if (ofn->lpstrFilter[i] == '\0')
            if (filterResetFlag)
            {
                gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
                gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(dialog), filter);
                filter = gtk_file_filter_new ();
                strFilterLen = 0;
                //g_print("filter pat: %s\n", strFilter);
                //g_print("filter reset\n");
                filterResetFlag = FALSE;
            }
            else
            {
                gtk_file_filter_set_name (GTK_FILE_FILTER(filter), strFilter);
                //g_print("filter name: %s\n", strFilter);
                strFilterLen = 0;
                filterResetFlag = TRUE;
            }
    }
    //g_print("filter rules added\n");
    
    sprintf(strFilter, "*.%s", ofn->lpstrDefExt);
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
    //gtk_file_filter_set_name (filter, "int files");
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(dialog), filter);
    
    delete strFilter;

    //g_print("default filter set\n");

    BOOL exitStatus = gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (exitStatus)
    {
        char* str;
        str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
        strcpy(ofn->lpstrFile, str);
        g_free(str);
    }
    //g_print("file path saved: %s\n", ofn->lpstrFile);
    
    gtk_widget_destroy (dialog);

    //g_print("exit\n");

    return exitStatus;
}

void EnableMenuItem(HMENU MenuName, HMENU MenuItem, UINT CheckEnabledItem) {
    switch (CheckEnabledItem){
        case MF_ENABLED :
           gtk_widget_set_sensitive (MenuItem, true);
        break;
        case MF_GRAYED :
           gtk_widget_set_sensitive (MenuItem, false);
        break; 
    }
}

void CheckMenuItem(HMENU MenuName, HMENU MenuItem, UINT Check){
    switch (Check){
        case MF_CHECKED :
            gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(MenuItem), true);
        break;
        case MF_UNCHECKED :
            gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(MenuItem), false);
        break;
    }
}