//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// Common controls in the main window. The main window consists of the drawing
// area, where the ladder diagram is displayed, plus various controls for
// scrolling, I/O list, menus.
// Jonathan Westhues, Nov 2004
//-----------------------------------------------------------------------------
#include "linuxUI.h"
#include <typeinfo>
//#include <commctrl.h>
//#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include "ldmicro.h"

// Menu IDs
HMENU MNU_NEW;
HMENU MNU_OPEN;
HMENU MNU_SAVE;
HMENU MNU_SAVE_AS;
HMENU MNU_EXPORT;
HMENU MNU_EXIT;

HMENU MNU_UNDO;
HMENU MNU_REDO;
HMENU MNU_PUSH_RUNG_UP;
HMENU MNU_PUSH_RUNG_DOWN;
HMENU MNU_INSERT_RUNG_BEFORE;
HMENU MNU_INSERT_RUNG_AFTER;
HMENU MNU_DELETE_ELEMENT;
HMENU MNU_DELETE_RUNG;

HMENU MNU_INSERT_COMMENT;
HMENU MNU_INSERT_CONTACTS;
HMENU MNU_INSERT_COIL;
HMENU MNU_INSERT_TON;
HMENU MNU_INSERT_TOF;
HMENU MNU_INSERT_RTO;
HMENU MNU_INSERT_RES;
HMENU MNU_INSERT_OSR;
HMENU MNU_INSERT_OSF;
HMENU MNU_INSERT_CTU;
HMENU MNU_INSERT_CTD;
HMENU MNU_INSERT_CTC;
HMENU MNU_INSERT_ADD;
HMENU MNU_INSERT_SUB;
HMENU MNU_INSERT_MUL;
HMENU MNU_INSERT_DIV;
HMENU MNU_INSERT_MOV;
HMENU MNU_INSERT_READ_ADC;
HMENU MNU_INSERT_SET_PWM;
HMENU MNU_INSERT_UART_SEND;
HMENU MNU_INSERT_UART_RECV;
HMENU MNU_INSERT_EQU;
HMENU MNU_INSERT_NEQ;
HMENU MNU_INSERT_GRT;
HMENU MNU_INSERT_GEQ;
HMENU MNU_INSERT_LES;
HMENU MNU_INSERT_LEQ;
HMENU MNU_INSERT_OPEN;
HMENU MNU_INSERT_SHORT;
HMENU MNU_INSERT_MASTER_RLY;
HMENU MNU_INSERT_SHIFT_REG;
HMENU MNU_INSERT_LUT;
HMENU MNU_INSERT_FMTD_STR;
HMENU MNU_INSERT_PERSIST;
HMENU MNU_MAKE_NORMAL;
HMENU MNU_NEGATE;
HMENU MNU_MAKE_SET_ONLY;
HMENU MNU_MAKE_RESET_ONLY;
HMENU MNU_INSERT_PWL;

HMENU MNU_MCU_SETTINGS;
HMENU MNU_PROCESSOR[NUM_SUPPORTED_MCUS+1];
HMENU MNU_MICRO_CONTROLLER;                                         // Item for Microcontroller

HMENU MNU_SIMULATION_MODE;
HMENU MNU_START_SIMULATION;
HMENU MNU_STOP_SIMULATION;
HMENU MNU_SINGLE_CYCLE;

HMENU MNU_COMPILE;
HMENU MNU_COMPILE_AS;

HMENU MNU_MANUAL;
HMENU MNU_ABOUT;

// scrollbars for the ladder logic area
// static HWND         HorizScrollBar;
// static HWND         VertScrollBar;
// int                 ScrollWidth;
// int                 ScrollHeight;
// BOOL                NeedHoriz;

// status bar at the bottom of the screen, to display settings
static HMENU        StatusBar;

// have to get back to the menus to gray/ungray, check/uncheck things
static HMENU        FileMenu;
static HMENU        EditMenu;
static HMENU        InstructionMenu;
static HMENU        ProcessorMenu;
static HMENU        SimulateMenu;
static HMENU        TopMenu;                                            // Menu Bar
static HMENU        settings;
static HMENU        compile;
static HMENU        help;
HMENU        ScrollWindow;

// listview used to maintain the list of I/O pins with symbolic names, plus
// the internal relay too
HLIST               IoList;
static int          IoListSelectionPoint;
static BOOL         IoListOutOfSync;
int                 IoListHeight;
int                 IoListTop;
GtkTreeIter* iter = new GtkTreeIter;
GtkTreeModel **IoListPtr = (GtkTreeModel**)GTK_TREE_MODEL (IoList);

// whether the simulation is running in real time
static BOOL         RealTimeSimulationRunning;

//-----------------------------------------------------------------------------
// Create the top-level menu bar for the main window. Mostly static, but we
// create the "select processor" menu from the list in mcutable.h dynamically.
//-----------------------------------------------------------------------------
HMENU MakeMainWindowMenus(void)
{   
    HMENU MenuBox;                                                  // Box for alignment
    HMENU PackedMenuBox;                                            // Stores the packed box
    HMENU TopMenu;                                                  // Menu Bar
    HWID  FileLabel;                                                // File menu label
    HWID  EditLabel;                                                // Edit menu label
    HWID  InstructionLabel;                                         // Instruction menu label
    HWID  SettingsLabel;                                            // Settings menu label
    HWID  CompileLabel;                                             // Compile menu label
    HWID  HelpLabel;                                                // Help menu label
    HWID  SimulateLabel;                                            // Simulate menu label
    HMENU ProcessorMenuItems;                                       // Processor menu items
    HMENU  FileMenuSeparator;                                       // File menu separator
    HMENU  EditMenuSeparator;                                       // Edit menu separator
    HMENU  InstructionMenuSeparator;                                // Instruction menu separator
    HMENU  SimulateMenuSeparator;                                   // Simulate menu separator

    int i;
    // Creating a box for desired orientation
    MenuBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Create new menu bar to hold menu and add it to window
    TopMenu = gtk_menu_bar_new();

    // Creating various menus
    FileMenu = gtk_menu_new();
    EditMenu = gtk_menu_new();
    settings = gtk_menu_new();
    ProcessorMenu = gtk_menu_new();
    InstructionMenu = gtk_menu_new();
    SimulateMenu = gtk_menu_new();
    compile = gtk_menu_new();
    help = gtk_menu_new();

    // Declaring the accelerator group for keyboard shortcuts
    AccelGroup = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (MainWindow), AccelGroup);

    // Creating labels for each menu
    FileLabel = gtk_menu_item_new_with_mnemonic("_File");
    EditLabel = gtk_menu_item_new_with_mnemonic("_Edit");
    SettingsLabel = gtk_menu_item_new_with_mnemonic("_Settings");  
    InstructionLabel = gtk_menu_item_new_with_mnemonic("_Instructions");
    SimulateLabel = gtk_menu_item_new_with_mnemonic("_Simulate");
    CompileLabel = gtk_menu_item_new_with_mnemonic("_Compile");
    HelpLabel = gtk_menu_item_new_with_mnemonic("_Help");

    // Creating labels for File Menu
    MNU_NEW = gtk_menu_item_new_with_mnemonic("_New");
    MNU_OPEN = gtk_menu_item_new_with_mnemonic("_Open");
    MNU_SAVE = gtk_menu_item_new_with_mnemonic("_Save");
    MNU_SAVE_AS = gtk_menu_item_new_with_mnemonic("_Save As");
    MNU_EXPORT = gtk_menu_item_new_with_mnemonic("_Export As Text");
    MNU_EXIT = gtk_menu_item_new_with_mnemonic("_Exit");

    // Creating keyboard shortcuts for File menu
    gtk_widget_add_accelerator (MNU_NEW, "activate", AccelGroup, GDK_KEY_N,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_OPEN, "activate", AccelGroup, GDK_KEY_O,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_SAVE, "activate", AccelGroup, GDK_KEY_S,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_EXPORT, "activate", AccelGroup, GDK_KEY_E,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
    // Appending menu items (labels) to File menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_NEW);       // Appending menu items
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_OPEN);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_SAVE);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_SAVE_AS);
    FileMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), FileMenuSeparator);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_EXPORT);
    FileMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), FileMenuSeparator);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), MNU_EXIT);

    // Creating labels for Edit Menu
    MNU_UNDO = gtk_menu_item_new_with_mnemonic("_Undo");
    MNU_REDO = gtk_menu_item_new_with_mnemonic("_Redo");
    MNU_INSERT_RUNG_BEFORE = gtk_menu_item_new_with_mnemonic("_Insert rung Before");
    MNU_INSERT_RUNG_AFTER = gtk_menu_item_new_with_mnemonic("_Insert Rung After");
    MNU_PUSH_RUNG_UP = gtk_menu_item_new_with_mnemonic("_Move Selected Rung Up");
    MNU_PUSH_RUNG_DOWN = gtk_menu_item_new_with_mnemonic("_Move Selected Rung Down");
    MNU_DELETE_ELEMENT = gtk_menu_item_new_with_mnemonic("_Delete Selected Element");
    MNU_DELETE_RUNG = gtk_menu_item_new_with_mnemonic("_Delete Rung");

    // Creating keyboard shortcuts for Edit menu
    gtk_widget_add_accelerator (MNU_UNDO, "activate", AccelGroup, GDK_KEY_Z,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_REDO, "activate", AccelGroup, GDK_KEY_Y,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_RUNG_BEFORE, "activate", AccelGroup, GDK_KEY_F6,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_RUNG_AFTER, "activate", AccelGroup, GDK_KEY_V,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_PUSH_RUNG_UP, "activate", AccelGroup, GDK_KEY_uparrow,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_PUSH_RUNG_DOWN, "activate", AccelGroup, GDK_KEY_downarrow,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_DELETE_ELEMENT, "activate", AccelGroup, GDK_KEY_Delete,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_DELETE_RUNG, "activate", AccelGroup, GDK_KEY_Delete,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    // Appending menu items to Edit menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_UNDO);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_REDO);
    EditMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(EditMenu), EditMenuSeparator);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_INSERT_RUNG_BEFORE);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_INSERT_RUNG_AFTER);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_PUSH_RUNG_UP);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_PUSH_RUNG_DOWN);
    EditMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(EditMenu), EditMenuSeparator);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_DELETE_ELEMENT);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), MNU_DELETE_RUNG);

    // Creating labels for Settings Menu
    MNU_MCU_SETTINGS = gtk_menu_item_new_with_mnemonic ("_MCU Parameters...");
    MNU_MICRO_CONTROLLER = gtk_menu_item_new_with_mnemonic ("_Microcontroller");

    // Appending menu items to Settings menu
    gtk_menu_shell_append (GTK_MENU_SHELL (settings), MNU_MCU_SETTINGS);
    gtk_menu_shell_append (GTK_MENU_SHELL (settings), MNU_MICRO_CONTROLLER);

    // Appending the microcontroller names to "Microcontroller" item
    for (i = 0; i < NUM_SUPPORTED_MCUS; i++){
    MNU_PROCESSOR[i] = gtk_check_menu_item_new_with_label (SupportedMcus[i].mcuName);
    gtk_menu_shell_append (GTK_MENU_SHELL (ProcessorMenu), MNU_PROCESSOR[i]);
    }

    MNU_PROCESSOR[NUM_SUPPORTED_MCUS] = gtk_check_menu_item_new_with_label ("(no microcontroller)");
    gtk_menu_shell_append (GTK_MENU_SHELL (ProcessorMenu), MNU_PROCESSOR[NUM_SUPPORTED_MCUS]);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(MNU_MICRO_CONTROLLER), ProcessorMenu);

    // Creating labels for Instruction Menu and adding separators
    MNU_INSERT_COMMENT = gtk_menu_item_new_with_mnemonic("_Insert Comment");
    MNU_INSERT_CONTACTS = gtk_menu_item_new_with_mnemonic("_Insert Contacts");
    MNU_INSERT_OSR = gtk_menu_item_new_with_mnemonic("_Insert OSR (One Shot Rising)");
    MNU_INSERT_OSF = gtk_menu_item_new_with_mnemonic("_Insert OSF (One Shot Falling)");
    MNU_INSERT_TON = gtk_menu_item_new_with_mnemonic("_Insert TON (Delayed Turn On)");
    MNU_INSERT_TOF = gtk_menu_item_new_with_mnemonic("_Insert TOF (Delayed Turn Off)");
    MNU_INSERT_RTO = gtk_menu_item_new_with_mnemonic("_Insert RTO (Retentive Delayed Turn On)");
    MNU_INSERT_CTU = gtk_menu_item_new_with_mnemonic("_Insert CTU (Count Up)");
    MNU_INSERT_CTD = gtk_menu_item_new_with_mnemonic("_Insert CTD (Count Down)");
    MNU_INSERT_CTC = gtk_menu_item_new_with_mnemonic("_Insert CTC (Count Circular)");
    MNU_INSERT_EQU = gtk_menu_item_new_with_mnemonic("_Insert EQU (Compare for Equals)");
    MNU_INSERT_NEQ = gtk_menu_item_new_with_mnemonic("_Insert NEQ (Compare for Not Equals)");
    MNU_INSERT_GRT = gtk_menu_item_new_with_mnemonic("_Insert GRT (Compare for Greater Than)");
    MNU_INSERT_GEQ = gtk_menu_item_new_with_mnemonic("_Insert GEQ (Compare for Greater Than or Equal)");
    MNU_INSERT_LES = gtk_menu_item_new_with_mnemonic("_Insert LES (Compare for Less Than)");
    MNU_INSERT_LEQ = gtk_menu_item_new_with_mnemonic("_Insert LEQ (Compare for Less Than or Equal)");
    MNU_INSERT_OPEN = gtk_menu_item_new_with_mnemonic("_Insert Open Circuit");
    MNU_INSERT_SHORT = gtk_menu_item_new_with_mnemonic("_Insert Short Circuit");
    MNU_INSERT_MASTER_RLY = gtk_menu_item_new_with_mnemonic("_Insert Master Control Relay");
    MNU_INSERT_COIL = gtk_menu_item_new_with_mnemonic("_Insert Coil");
    MNU_INSERT_RES = gtk_menu_item_new_with_mnemonic("_Insert RES (Counter/RTO Reset)");
    MNU_INSERT_MOV = gtk_menu_item_new_with_mnemonic("_Insert MOV (Move)");
    MNU_INSERT_ADD = gtk_menu_item_new_with_mnemonic("_Insert ADD (16-bit Integer Ad)");
    MNU_INSERT_SUB = gtk_menu_item_new_with_mnemonic("_Insert SUB (16-bit Integer Subtract)");
    MNU_INSERT_MUL = gtk_menu_item_new_with_mnemonic("_Insert MUL (16-bit Integer Multiply)");
    MNU_INSERT_DIV = gtk_menu_item_new_with_mnemonic("_Insert DIV (16-bit Integer Division)");
    MNU_INSERT_SHIFT_REG = gtk_menu_item_new_with_mnemonic("_Insert Shift Register");
    MNU_INSERT_LUT = gtk_menu_item_new_with_mnemonic("_Insert Look-Up Table");
    MNU_INSERT_PWL = gtk_menu_item_new_with_mnemonic("_Insert Piecewise Linear");
    MNU_INSERT_FMTD_STR = gtk_menu_item_new_with_mnemonic("_Insert Formatted String Over UART");
    MNU_INSERT_UART_SEND = gtk_menu_item_new_with_mnemonic("_Insert UART Send");
    MNU_INSERT_UART_RECV = gtk_menu_item_new_with_mnemonic("_Insert UART Receive");
    MNU_INSERT_SET_PWM = gtk_menu_item_new_with_mnemonic("_Insert Set PWM Output");
    MNU_INSERT_READ_ADC = gtk_menu_item_new_with_mnemonic("_Insert A/D Converter Read");
    MNU_INSERT_PERSIST = gtk_menu_item_new_with_mnemonic("_Insert Make Persistent");
    MNU_MAKE_NORMAL = gtk_menu_item_new_with_mnemonic("_Make Normal");
    MNU_NEGATE = gtk_menu_item_new_with_mnemonic("_Make Negated");
    MNU_MAKE_SET_ONLY = gtk_menu_item_new_with_mnemonic("_Make Set-Only");
    MNU_MAKE_RESET_ONLY = gtk_menu_item_new_with_mnemonic("_Make Reset-Only");

    // Creating keyboard shortcuts for Instructions menu
    gtk_widget_add_accelerator (MNU_INSERT_COMMENT, "activate", AccelGroup, GDK_KEY_semicolon,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_CONTACTS, "activate", AccelGroup, GDK_KEY_C,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_OSR, "activate", AccelGroup, GDK_KEY_backslash,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_OSF, "activate", AccelGroup, GDK_KEY_slash,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_TON, "activate", AccelGroup, GDK_KEY_O,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_TOF, "activate", AccelGroup, GDK_KEY_F,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_RTO, "activate", AccelGroup, GDK_KEY_T,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_CTU, "activate", AccelGroup, GDK_KEY_U,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_CTD, "activate", AccelGroup, GDK_KEY_I,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_CTC, "activate", AccelGroup, GDK_KEY_J,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_EQU, "activate", AccelGroup, GDK_KEY_equal,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_GRT, "activate", AccelGroup, GDK_KEY_greater,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_GEQ, "activate", AccelGroup, GDK_KEY_Stop,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_LES, "activate", AccelGroup, GDK_KEY_less,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_LEQ, "activate", AccelGroup, GDK_KEY_comma,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_COIL, "activate", AccelGroup, GDK_KEY_L,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_RES, "activate", AccelGroup, GDK_KEY_E,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_MOV, "activate", AccelGroup, GDK_KEY_M,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_ADD, "activate", AccelGroup, GDK_KEY_plus,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_SUB, "activate", AccelGroup, GDK_KEY_minus,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_MUL, "activate", AccelGroup, GDK_KEY_multiply,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_DIV, "activate", AccelGroup, GDK_KEY_D,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_INSERT_READ_ADC, "activate", AccelGroup, GDK_KEY_P,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_MAKE_NORMAL, "activate", AccelGroup, GDK_KEY_A,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_NEGATE, "activate", AccelGroup, GDK_KEY_N,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_MAKE_SET_ONLY, "activate", AccelGroup, GDK_KEY_S,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_MAKE_RESET_ONLY, "activate", AccelGroup, GDK_KEY_R,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);

    // Appending menu items to Instruction menu and adding separators
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_COMMENT);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_CONTACTS);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_OSR);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_OSF);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_TON);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_TOF);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_RTO);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_CTU);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_CTD);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_CTC);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_EQU);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_NEQ);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_GRT);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_GEQ);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_LES);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_LEQ);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_OPEN);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_SHORT);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_MASTER_RLY);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_COIL);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_RES);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_MOV);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_ADD);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_SUB);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_MUL);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_DIV);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_SHIFT_REG);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_LUT);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_PWL);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_FMTD_STR);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_UART_SEND);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_UART_RECV);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_SET_PWM);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_READ_ADC);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_INSERT_PERSIST);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_MAKE_NORMAL);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_NEGATE);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_MAKE_SET_ONLY);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MNU_MAKE_RESET_ONLY);

    // Creating labels for Compile Menu
    MNU_COMPILE = gtk_menu_item_new_with_mnemonic("_Compile");
    MNU_COMPILE_AS = gtk_menu_item_new_with_mnemonic("_Compile As...");

    // Creating keyboard shortcuts for Compile menu
    gtk_widget_add_accelerator (MNU_COMPILE, "activate", AccelGroup, GDK_KEY_F5,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);

    // Appending menu items to Compile menu
    gtk_menu_shell_append(GTK_MENU_SHELL (compile), MNU_COMPILE);
    gtk_menu_shell_append(GTK_MENU_SHELL (compile), MNU_COMPILE_AS);

    // Creating labels for Help Menu
    MNU_MANUAL = gtk_menu_item_new_with_mnemonic("_Manual...");
    MNU_ABOUT = gtk_menu_item_new_with_mnemonic("_About...");

    // Creating keyboard shortcuts for Help menu
    gtk_widget_add_accelerator (MNU_MANUAL, "activate", AccelGroup, GDK_KEY_F1,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);

    // Appending menu items to Help menu
    gtk_menu_shell_append(GTK_MENU_SHELL (help), MNU_MANUAL);
    gtk_menu_shell_append(GTK_MENU_SHELL (help), MNU_ABOUT);

    // Creating labels for Simulation Menu
    MNU_SIMULATION_MODE = gtk_check_menu_item_new_with_mnemonic("_Simulation Mode");
    MNU_START_SIMULATION = gtk_menu_item_new_with_mnemonic("_Start Real-Time Simulation");
    MNU_STOP_SIMULATION = gtk_menu_item_new_with_mnemonic("_Halt Simulation");
    MNU_SINGLE_CYCLE = gtk_menu_item_new_with_mnemonic("_Single Cycle");

    // Creating keyboard shortcuts for Edit menu
    gtk_widget_add_accelerator (MNU_SIMULATION_MODE, "activate", AccelGroup, GDK_KEY_M,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_START_SIMULATION, "activate", AccelGroup, GDK_KEY_R,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_STOP_SIMULATION, "activate", AccelGroup, GDK_KEY_H,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MNU_SINGLE_CYCLE, "activate", AccelGroup, GDK_KEY_space,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);

    // Appending menu items to Simulate menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), MNU_SIMULATION_MODE);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), MNU_START_SIMULATION);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), MNU_STOP_SIMULATION);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), MNU_SINGLE_CYCLE);
    SimulateMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(SimulateMenu), SimulateMenuSeparator);

    // Creating submenus for each menu  
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(FileLabel), FileMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(EditLabel), EditMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(SettingsLabel), settings);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(InstructionLabel), InstructionMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(SimulateLabel), SimulateMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(CompileLabel), compile);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(HelpLabel), help);

    // Appending the menu item to the menu bar
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), FileLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), EditLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), SettingsLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), InstructionLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), SimulateLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), CompileLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(TopMenu), HelpLabel);

    // Packing the menu bar into the box for alignment
    gtk_box_pack_start(GTK_BOX(MenuBox), TopMenu, FALSE, FALSE, 0);

    return MenuBox;
}

//-----------------------------------------------------------------------------
// Create the standard Windows controls used in the main window: a Listview
// for the I/O list, and a status bar for settings.
//-----------------------------------------------------------------------------
void MakeMainWindowControls(void)
{
    HWID PackBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    HWID grid = gtk_grid_new();
    HWID pane = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

    IoList = gtk_list_store_new (5, 
                                G_TYPE_STRING,   
                                G_TYPE_STRING,    
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING);

    int typeWidth = 85;
    int pinWidth = 100;
    int portWidth = 90;

    // Creating a list
    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(IoList));
    gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL (IoList));

    column = gtk_tree_view_column_new_with_attributes("Name",
                                                    gtk_cell_renderer_text_new(),
                                                    "text", LV_IO_NAME,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    gtk_tree_view_column_set_min_width (column, 250);

    column = gtk_tree_view_column_new_with_attributes("Type",
                                                    gtk_cell_renderer_spin_new(),
                                                    "text", LV_IO_TYPE,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    gtk_tree_view_column_set_min_width (column, typeWidth);

    column = gtk_tree_view_column_new_with_attributes("State",
                                                    gtk_cell_renderer_text_new(),
                                                    "text", LV_IO_STATE,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    gtk_tree_view_column_set_min_width (column, 100);

    column = gtk_tree_view_column_new_with_attributes("Pin on Processor",
                                                    gtk_cell_renderer_text_new(),
                                                    "text", LV_IO_PIN,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    gtk_tree_view_column_set_min_width (column, pinWidth);

    column = gtk_tree_view_column_new_with_attributes("MCU Port",
                                                    gtk_cell_renderer_text_new(),
                                                    "text", LV_IO_PORT,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    gtk_tree_view_column_set_min_width (column, portWidth);

    // Appending Menus to grid
    gtk_grid_attach (GTK_GRID (grid), MakeMainWindowMenus(), 0, 0, 1, 1);

    // Creating Scrolled Window
    ScrollWindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (ScrollWindow),
				                          GTK_POLICY_AUTOMATIC, 
				                          GTK_POLICY_ALWAYS);

    // Creating a pane to separate Scrolled Window and other widgets
    gtk_paned_add1 (GTK_PANED (pane), ScrollWindow);
    gtk_paned_set_position (GTK_PANED (pane), 0);
    gtk_widget_set_vexpand (ScrollWindow, TRUE);
    gtk_widget_set_hexpand (ScrollWindow, TRUE);

    // Appending tree view to pane and pane to grid
    gtk_paned_pack2 (GTK_PANED(pane), view, FALSE, FALSE);
    gtk_paned_set_position (GTK_PANED (pane), 250);
    gtk_grid_attach (GTK_GRID (grid), pane, 0, 0, 1, 1); 

    // Creating Status Bar and attaching to grid
    StatusBar = gtk_statusbar_new();
    gtk_statusbar_push (GTK_STATUSBAR (StatusBar),
                        gtk_statusbar_get_context_id (GTK_STATUSBAR (StatusBar), "Introduction"), 
                        "LDMicro Started");

    // Appneding Status Bar to box which is then added to Main Window
    gtk_box_pack_start(GTK_BOX(PackBox), grid, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(PackBox), StatusBar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(MainWindow), PackBox);
}

//-----------------------------------------------------------------------------
// Set up the title bar text for the main window; indicate whether we are in
// simulation or editing mode, and indicate the filename.
//-----------------------------------------------------------------------------
void UpdateMainWindowTitleBar(void)
{
    char line[PATH_MAX+100];
    if(InSimulationMode) {
        if(RealTimeSimulationRunning) {
            strcpy(line, "LDmicro - Simulation (Running)");
        } else {
            strcpy(line, "LDmicro - Simulation (Stopped)");
        }
    } else {
        strcpy(line, "LDmicro - Program Editor");
    }
    if(strlen(CurrentSaveFile) > 0) {
        sprintf(line+strlen(line), " - %s", CurrentSaveFile);
    } else {
        strcat(line, " - (not yet saved)");
    }
    
  gtk_window_set_title (GTK_WINDOW (MainWindow), line);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the logic menu items to reflect where we are on
// the schematic (e.g. can't insert two coils in series).
//-----------------------------------------------------------------------------
void SetMenusEnabled(BOOL canNegate, BOOL canNormal, BOOL canResetOnly,
    BOOL canSetOnly, BOOL canDelete, BOOL canInsertEnd, BOOL canInsertOther,
    BOOL canPushDown, BOOL canPushUp, BOOL canInsertComment)
{
    EnableMenuItem(EditMenu, MNU_PUSH_RUNG_UP,
        canPushUp ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_PUSH_RUNG_DOWN,
        canPushDown ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_DELETE_RUNG,
        (Prog.numRungs > 1) ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, MNU_NEGATE,
        canNegate ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_NORMAL,
        canNormal ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_RESET_ONLY,
        canResetOnly ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_SET_ONLY,
        canSetOnly ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, MNU_INSERT_COMMENT,
        canInsertComment ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(EditMenu, MNU_DELETE_ELEMENT,
        canDelete ? MF_ENABLED : MF_GRAYED);

    int t;
    t = canInsertEnd ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, MNU_INSERT_COIL, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_RES, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_MOV, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_ADD, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SUB, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_MUL, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_DIV, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTC, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_PERSIST, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_READ_ADC, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SET_PWM, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_MASTER_RLY, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SHIFT_REG, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_LUT, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_PWL, t);

    t = canInsertOther ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, MNU_INSERT_TON, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_TOF, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OSR, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OSF, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_RTO, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CONTACTS, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTU, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTD, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_EQU, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_NEQ, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_GRT, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_GEQ, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_LES, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_LEQ, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SHORT, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OPEN, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_UART_SEND, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_UART_RECV, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_FMTD_STR, t);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the undo/redo menus.
//-----------------------------------------------------------------------------
void SetUndoEnabled(BOOL undoEnabled, BOOL redoEnabled)
{
    // EnableMenuItem(EditMenu, MNU_UNDO, undoEnabled ? MF_ENABLED : MF_GRAYED);
    // EnableMenuItem(EditMenu, MNU_REDO, redoEnabled ? MF_ENABLED : MF_GRAYED);
}

//-----------------------------------------------------------------------------
// Toggle whether we are in simulation mode. A lot of options are only
// available in one mode or the other.
//-----------------------------------------------------------------------------
void ToggleSimulationMode(void)
{
    InSimulationMode = !InSimulationMode;
    if(InSimulationMode) {
        EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
        EnableMenuItem(SimulateMenu, MNU_SINGLE_CYCLE, MF_ENABLED);

        EnableMenuItem(FileMenu, MNU_OPEN, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_SAVE, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_SAVE_AS, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_NEW, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_EXPORT, MF_GRAYED);

        EnableMenuItem(TopMenu, EditMenu, MF_GRAYED);
        EnableMenuItem(TopMenu, settings, MF_GRAYED);
        EnableMenuItem(TopMenu, InstructionMenu, MF_GRAYED);
        EnableMenuItem(TopMenu, compile, MF_GRAYED);
    
        CheckMenuItem(SimulateMenu, MNU_SIMULATION_MODE, MF_CHECKED);

        // ClearSimulationData(); // simulation.cpp, ldmicro.h
        // Recheck InSimulationMode, because there could have been a compile
        // error, which would have kicked us out of simulation mode.
        // if(UartFunctionUsed() && InSimulationMode) {
        //     ShowUartSimulationWindow(); // simulate.cpp
        // }
    }
    else {
        RealTimeSimulationRunning = FALSE;
        // KillTimer(MainWindow, TIMER_SIMULATE);

        EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_GRAYED);
        EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
        EnableMenuItem(SimulateMenu, MNU_SINGLE_CYCLE, MF_GRAYED);

        EnableMenuItem(FileMenu, MNU_OPEN, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_SAVE, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_SAVE_AS, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_NEW, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_EXPORT, MF_ENABLED);

        EnableMenuItem(TopMenu, EditMenu, MF_ENABLED);
        EnableMenuItem(TopMenu, settings, MF_ENABLED);
        EnableMenuItem(TopMenu, InstructionMenu, MF_ENABLED);
        EnableMenuItem(TopMenu, compile, MF_ENABLED);

        CheckMenuItem(SimulateMenu, MNU_SIMULATION_MODE, MF_UNCHECKED);

        // if(UartFunctionUsed()) {
        //     DestroyUartSimulationWindow();
        // }
        }

    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Cause the status bar and the list view to be in sync with the actual data
// structures describing the settings and the I/O configuration. Listview
// does callbacks to get the strings it displays, so it just needs to know
// how many elements to populate.
//-----------------------------------------------------------------------------
void RefreshControlsToSettings(void)
{   
    int i;
    gtk_tree_model_get_iter_first (GTK_TREE_MODEL(IoList), iter);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
                                    GTK_SELECTION_SINGLE);
    if(!IoListOutOfSync) {
        IoListSelectionPoint = -1;
        for(i = 0; i < Prog.io.count; i++) {
            gtk_tree_model_iter_next (GTK_TREE_MODEL(IoList), iter);
            if(gtk_tree_selection_get_selected (gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
                                        IoListPtr, iter)) {
                IoListSelectionPoint = i;
                break;
            }
        }
    }
    gtk_list_store_clear (IoList);
    IoListOutOfSync = FALSE;

    if(Prog.mcu) {
        gtk_statusbar_push (GTK_STATUSBAR (StatusBar),
                        gtk_statusbar_get_context_id (GTK_STATUSBAR (StatusBar), "MCU Name"), 
                        (gchar*)Prog.mcu->mcuName);
        // SendMessage(StatusBar, SB_SETTEXT, 0, (LPARAM)Prog.mcu->mcuName);
    } 
    else {
        // SendMessage(StatusBar, SB_SETTEXT, 0, (LPARAM)_("no MCU selected"));
        gtk_statusbar_push (GTK_STATUSBAR (StatusBar),
                        gtk_statusbar_get_context_id (GTK_STATUSBAR (StatusBar), "MCU Name"), 
                        "no MCU selected");
    }
    for(i = 0; i < NUM_SUPPORTED_MCUS; i++) {
        if(&SupportedMcus[i] == Prog.mcu) {
            CheckMenuItem(ProcessorMenu, MNU_PROCESSOR[i], MF_CHECKED);
        }
        else {
            CheckMenuItem(ProcessorMenu, MNU_PROCESSOR[i], MF_UNCHECKED);
        }
    }
    // `(no microcontroller)' setting
    if (!Prog.mcu){
        CheckMenuItem(ProcessorMenu, MNU_PROCESSOR[NUM_SUPPORTED_MCUS], MF_CHECKED);
    }
    else {
        CheckMenuItem(ProcessorMenu, MNU_PROCESSOR[NUM_SUPPORTED_MCUS], MF_UNCHECKED);
    }
}

//-----------------------------------------------------------------------------
// Regenerate the I/O list, keeping the selection in the same place if
// possible.
//-----------------------------------------------------------------------------
void GenerateIoListDontLoseSelection(void)
{
    int i;
    gtk_tree_model_get_iter_first (GTK_TREE_MODEL(IoList), iter);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
                                    GTK_SELECTION_SINGLE);
    IoListSelectionPoint = -1;
    for(i = 0; i < Prog.io.count; i++) {
        gtk_tree_model_iter_next (GTK_TREE_MODEL (IoList), iter);
        if(gtk_tree_selection_get_selected (gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
                                        IoListPtr, iter)) {
            IoListSelectionPoint = i;
            break;
        }
    }
    // IoListSelectionPoint = GenerateIoList(IoListSelectionPoint);
    
    // can't just update the listview index; if I/O has been added then the
    // new selection point might be out of range till we refill it
    
    IoListOutOfSync = TRUE;
    RefreshControlsToSettings();
}

//-----------------------------------------------------------------------------
// Called when the main window has been resized. Adjust the size of the
// status bar and the listview to reflect the new window size.
//-----------------------------------------------------------------------------
void MainWindowResized(void)
{
    // Make sure that the I/O list can't disappear entirely.
    if(IoListHeight < 30) {
        IoListHeight = 30;
    }
}

//-----------------------------------------------------------------------------
// Start real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StartSimulation(void)
{
    RealTimeSimulationRunning = TRUE;
    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_GRAYED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_ENABLED);
    // StartSimulationTimer();
    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Stop real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StopSimulation(void)
{
    RealTimeSimulationRunning = FALSE;

    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
    // KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}