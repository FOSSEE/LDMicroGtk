#include "linuxUI.h"

/// Menu Variables
HWID window;
HWID menu_box;                                              // Box for alignment
HWID packed_menu_box;                                       // Stores the packed box
HWID FileMenu;                                              // File Menu
HWID EditMenu;                                              // Edit Menu
HWID settings;                                              // Settings Menu
HWID ProcessorMenu;                                         // Processor Menu
HWID InstructionMenu;                                       // Instruction Menu
HWID compile;                                               // Compile Menu
HWID help;                                                  // Help Menu
HWID SimulateMenu;                                          // Simulate Menu
HWID menu_bar;                                              // Menu Bar
HWID file_label;                                            // File menu label
HWID edit_label;                                            // Edit menu label
HWID instruction_label;                                     // Instruction menu label
HWID settings_label;                                        // Settings menu label
HWID compile_label;                                         // Compile menu label
HWID help_label;                                            // Help menu label
HWID simulate_label;                                        // Simulate menu label
HWID file_menu_items;                                       // File menu item
HWID edit_menu_items;                                       // Edit menu item
HWID instruction_menu_items;                                // Instruction menu item
HWID settings_menu_items;                                   // Settings menu item
HWID processor_menu_items;                                  // Processor menu items
HWID compile_menu_items;                                    // Compile menu item
HWID help_menu_items;                                       // Help menu item
HWID simulate_menu_items;                                   // Simulate menu item
HWID file_menu_separator;                                   // File menu separator
HWID edit_menu_separator;                                   // Edit menu separator
HWID instruction_menu_separator;                            // Instruction menu separator
HWID simulate_menu_separator;                               // Simulate menu separator

//Scrollbars for the ladder logic area 
// int ScrollWidth;                                             // Width of scrolling
// int ScrollHeight;                                            // Height of scrolling
HWID IoList;                                                 // Window for list view

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

int MessageBox(HWID pWindow, char* message, char* title, UINT mFlags)
 {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkMessageType mType;
    
    if ((mFlags & MB_ICONERROR) == MB_ICONERROR)
        mType = GTK_MESSAGE_ERROR;
    else if ((mFlags & MB_ICONQUESTION) == MB_ICONQUESTION)
        mType = GTK_MESSAGE_QUESTION;
    else if ((mFlags & MB_ICONWARNING) == MB_ICONWARNING)
        mType = GTK_MESSAGE_WARNING;
    else if ((mFlags & MB_ICONINFORMATION) == MB_ICONINFORMATION)
        mType = GTK_MESSAGE_INFO;
    else
        mType = GTK_MESSAGE_OTHER;

    mType = GTK_MESSAGE_ERROR;
    HWID dialog = gtk_message_dialog_new (GTK_WINDOW(pWindow),
                                    flags,
                                    mType,
                                    GTK_BUTTONS_NONE,
                                    message);

    if ((mFlags & MB_OKCANCEL) == MB_OKCANCEL)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_OK", IDOK);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_CANCEL", IDCANCEL);
    }
    else if ((mFlags & MB_YESNO) == MB_YESNO)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_YES", IDYES);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_NO", IDNO);
    }
    else if ((mFlags & MB_YESNOCANCEL) == MB_YESNOCANCEL)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_YES", IDYES);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_NO", IDNO);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_CANCEL", IDCANCEL);
    }
    else
        gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", IDOK);
    
    gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG(dialog),
                               title);
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), message);
    int result = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    return result;
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