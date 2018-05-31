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
int ScrollWidth;                                             // Width of scrolling
int ScrollHeight;                                            // Height of scrolling
HWID IoList;                                                 // Window for list view

// ldmicro.cpp
char CurrentSaveFile[MAX_PATH];

// Simulation Mode
BOOL InSimulationMode;

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