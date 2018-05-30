#include "linuxUI/linuxUI.h"

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

/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWND window)
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