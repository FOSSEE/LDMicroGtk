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
HMENU NewMenu;
HMENU OpenMenu;
HMENU SaveMenu;
HMENU SaveAsMenu_AS;
HMENU ExportMenu;
HMENU ExitMenu;

HMENU UndoMenu;
HMENU RedoMenu;
HMENU PushRungUpMenu;
HMENU PushRungDownMenu;
HMENU InsertRungBeforeMenu;
HMENU InsertRungAfterMenu;
HMENU DeleteElementMenu;
HMENU DeleteRungMenu;

HMENU InsertCommentMenu;
HMENU InsertContactsMenu;
HMENU InsertCoilMenu;
HMENU InsertTonMenu;
HMENU InsertTofMenu;
HMENU InsertRtoMenu;
HMENU InsertResMenu;
HMENU InsertOsrMenu;
HMENU InsertOsfMenu;
HMENU InsertCtuMenu;
HMENU InsertCtdMenu;
HMENU InsertCtcMenu;
HMENU InsertAddMenu;
HMENU InsertSubMenu;
HMENU InsertMulMenu;
HMENU InsertDivMenu;
HMENU InsertMovMenu;
HMENU InsertReadAdcMenu;
HMENU InsertSetPwmMenu;
HMENU InsertUartSendMenu;
HMENU InsertUartRecvMenu;
HMENU InsertEquMenu;
HMENU InsertNeqMenu;
HMENU InsertGrtMenu;
HMENU InsertGeqMenu;
HMENU InsertLesMenu;
HMENU InsertLeqMenu;
HMENU InsertOpenMenu;
HMENU InsertShortMenu;
HMENU InsertMasterRlyMenu;
HMENU InsertShiftRegMenu;
HMENU InsertLutMenu;
HMENU InsertFmtdStrMenu;
HMENU InsertPersistMenu;
HMENU MakeNormalMenu;
HMENU NegateMenu;
HMENU MakeSetOnlyMenu;
HMENU MakeResetOnlyMenu;
HMENU InsertPwlMenu;

HMENU McuSettingsMenu;
HMENU ProcessorMenuItems[NUM_SUPPORTED_MCUS+1];
HMENU MicroControllerMenu;                                         // Item for Microcontroller

HMENU SimulationModeMenu;
HMENU StartSimulationMenu;
HMENU StopSimulationMenu;
HMENU SingleCycleMenu;

HMENU CompileMenu;
HMENU CompileAsMenu;

HMENU ManualMenu;
HMENU AboutMenu;

// scrollbars for the ladder logic area
// static HWND         HorizScrollBar;
// static HWND         VertScrollBar;
int                 ScrollWidth;
int                 ScrollHeight;
BOOL                NeedHoriz;

// status bar at the bottom of the screen, to display settings
static HMENU        StatusBar;

// have to get back to the menus to gray/ungray, check/uncheck things
static HMENU        FileMenu;
static HMENU        EditMenu;
static HMENU        InstructionMenu;
static HMENU        ProcessorMenu;
static HMENU        SimulateMenu;
static HMENU        TopMenu;                                            // Menu Bar
static HMENU        Settings;
static HMENU        Compile;
static HMENU        Help;
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

// Displaying keyboard shortcuts for each menu item
void AddMenuAccelerators (void){
    // Declaring the accelerator group for keyboard shortcuts
    AccelGroup = gtk_accel_group_new ();

    // Creating keyboard shortcuts for File menu
    gtk_widget_add_accelerator (NewMenu, "activate", AccelGroup, GDK_KEY_N,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (OpenMenu, "activate", AccelGroup, GDK_KEY_O,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (SaveMenu, "activate", AccelGroup, GDK_KEY_S,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (ExportMenu, "activate", AccelGroup, GDK_KEY_E,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Creating keyboard shortcuts for Edit menu
    gtk_widget_add_accelerator (UndoMenu, "activate", AccelGroup, GDK_KEY_Z,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (RedoMenu, "activate", AccelGroup, GDK_KEY_Y,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertRungBeforeMenu, "activate", AccelGroup, GDK_KEY_F6,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertRungAfterMenu, "activate", AccelGroup, GDK_KEY_V,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (PushRungUpMenu, "activate", AccelGroup, GDK_KEY_uparrow,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (PushRungDownMenu, "activate", AccelGroup, GDK_KEY_downarrow,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (DeleteElementMenu, "activate", AccelGroup, GDK_KEY_Delete,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (DeleteRungMenu, "activate", AccelGroup, GDK_KEY_Delete,
                                GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    
    // Creating keyboard shortcuts for Instructions menu
    gtk_widget_add_accelerator (InsertCommentMenu, "activate", AccelGroup, GDK_KEY_semicolon,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertContactsMenu, "activate", AccelGroup, GDK_KEY_C,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertOsrMenu, "activate", AccelGroup, GDK_KEY_backslash,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertOsfMenu, "activate", AccelGroup, GDK_KEY_slash,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertTonMenu, "activate", AccelGroup, GDK_KEY_O,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertTofMenu, "activate", AccelGroup, GDK_KEY_F,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertRtoMenu, "activate", AccelGroup, GDK_KEY_T,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertCtuMenu, "activate", AccelGroup, GDK_KEY_U,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertCtdMenu, "activate", AccelGroup, GDK_KEY_I,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertCtcMenu, "activate", AccelGroup, GDK_KEY_J,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertEquMenu, "activate", AccelGroup, GDK_KEY_equal,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertGrtMenu, "activate", AccelGroup, GDK_KEY_greater,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertGeqMenu, "activate", AccelGroup, GDK_KEY_Stop,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertLesMenu, "activate", AccelGroup, GDK_KEY_less,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertLeqMenu, "activate", AccelGroup, GDK_KEY_comma,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertCoilMenu, "activate", AccelGroup, GDK_KEY_L,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertResMenu, "activate", AccelGroup, GDK_KEY_E,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertMovMenu, "activate", AccelGroup, GDK_KEY_M,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertAddMenu, "activate", AccelGroup, GDK_KEY_plus,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertSubMenu, "activate", AccelGroup, GDK_KEY_minus,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertMulMenu, "activate", AccelGroup, GDK_KEY_multiply,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertDivMenu, "activate", AccelGroup, GDK_KEY_D,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (InsertReadAdcMenu, "activate", AccelGroup, GDK_KEY_P,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MakeNormalMenu, "activate", AccelGroup, GDK_KEY_A,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (NegateMenu, "activate", AccelGroup, GDK_KEY_N,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MakeSetOnlyMenu, "activate", AccelGroup, GDK_KEY_S,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (MakeResetOnlyMenu, "activate", AccelGroup, GDK_KEY_R,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    
    // Creating keyboard shortcuts for Simulation menu
    gtk_widget_add_accelerator (SimulationModeMenu, "activate", AccelGroup, GDK_KEY_M,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (StartSimulationMenu, "activate", AccelGroup, GDK_KEY_R,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (StopSimulationMenu, "activate", AccelGroup, GDK_KEY_H,
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (SingleCycleMenu, "activate", AccelGroup, GDK_KEY_space,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    
    // Creating keyboard shortcuts for Compile menu
    gtk_widget_add_accelerator (CompileMenu, "activate", AccelGroup, GDK_KEY_F5,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    
    // Creating keyboard shortcuts for Help menu
    gtk_widget_add_accelerator (ManualMenu, "activate", AccelGroup, GDK_KEY_F1,
                                GDK_RELEASE_MASK, GTK_ACCEL_VISIBLE);
    
    gtk_window_add_accel_group (GTK_WINDOW (MainWindow), AccelGroup);

}

//-----------------------------------------------------------------------------
// Create the top-level menu bar for the main window. Mostly static, but we
// create the "select processor" menu from the list in mcutable.h dynamically.
//-----------------------------------------------------------------------------
HMENU MakeMainWindowMenus(void)
{   
    HMENU MenuBox;                                                  // Box for packing and alignment
    // HMENU TopMenu;                                                  // Menu Bar
    HWID  FileLabel;                                                // File menu label
    HWID  EditLabel;                                                // Edit menu label
    HWID  InstructionLabel;                                         // Instruction menu label
    HWID  SettingsLabel;                                            // Settings menu label
    HWID  CompileLabel;                                             // Compile menu label
    HWID  HelpLabel;                                                // Help menu label
    HWID  SimulateLabel;                                            // Simulate menu label
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
    Settings = gtk_menu_new();
    ProcessorMenu = gtk_menu_new();
    InstructionMenu = gtk_menu_new();
    SimulateMenu = gtk_menu_new();
    Compile = gtk_menu_new();
    Help = gtk_menu_new();

    // Creating labels for each menu
    FileLabel = gtk_menu_item_new_with_mnemonic("_File");
    EditLabel = gtk_menu_item_new_with_mnemonic("_Edit");
    SettingsLabel = gtk_menu_item_new_with_mnemonic("_Settings");  
    InstructionLabel = gtk_menu_item_new_with_mnemonic("_Instructions");
    SimulateLabel = gtk_menu_item_new_with_mnemonic("_Simulate");
    CompileLabel = gtk_menu_item_new_with_mnemonic("_Compile");
    HelpLabel = gtk_menu_item_new_with_mnemonic("_Help");

    // Creating labels for File Menu
    NewMenu = gtk_menu_item_new_with_mnemonic("_New");
    OpenMenu = gtk_menu_item_new_with_mnemonic("_Open");
    SaveMenu = gtk_menu_item_new_with_mnemonic("_Save");
    SaveAsMenu_AS = gtk_menu_item_new_with_mnemonic("_Save As");
    ExportMenu = gtk_menu_item_new_with_mnemonic("_Export As Text");
    ExitMenu = gtk_menu_item_new_with_mnemonic("_Exit");

    // Appending menu items (labels) to File menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), NewMenu);       // Appending menu items
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), OpenMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), SaveMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), SaveAsMenu_AS);
    FileMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), FileMenuSeparator);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), ExportMenu);
    FileMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), FileMenuSeparator);
    gtk_menu_shell_append(GTK_MENU_SHELL (FileMenu), ExitMenu);

    // Creating labels for Edit Menu
    UndoMenu = gtk_menu_item_new_with_mnemonic("_Undo");
    RedoMenu = gtk_menu_item_new_with_mnemonic("_Redo");
    InsertRungBeforeMenu = gtk_menu_item_new_with_mnemonic("_Insert rung Before");
    InsertRungAfterMenu = gtk_menu_item_new_with_mnemonic("_Insert Rung After");
    PushRungUpMenu = gtk_menu_item_new_with_mnemonic("_Move Selected Rung Up");
    PushRungDownMenu = gtk_menu_item_new_with_mnemonic("_Move Selected Rung Down");
    DeleteElementMenu = gtk_menu_item_new_with_mnemonic("_Delete Selected Element");
    DeleteRungMenu = gtk_menu_item_new_with_mnemonic("_Delete Rung");

    // Appending menu items to Edit menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), UndoMenu);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), RedoMenu);
    EditMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(EditMenu), EditMenuSeparator);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), InsertRungBeforeMenu);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), InsertRungAfterMenu);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), PushRungUpMenu);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), PushRungDownMenu);
    EditMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(EditMenu), EditMenuSeparator);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), DeleteElementMenu);    
    gtk_menu_shell_append(GTK_MENU_SHELL (EditMenu), DeleteRungMenu);

    // Creating labels for Settings Menu
    McuSettingsMenu = gtk_menu_item_new_with_mnemonic ("_MCU Parameters...");
    MicroControllerMenu = gtk_menu_item_new_with_mnemonic ("_Microcontroller");

    // Appending menu items to Settings menu
    gtk_menu_shell_append (GTK_MENU_SHELL (Settings), McuSettingsMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (Settings), MicroControllerMenu);

    // Appending the microcontroller names to "Microcontroller" item
    for (i = 0; i < NUM_SUPPORTED_MCUS; i++){
    ProcessorMenuItems[i] = gtk_check_menu_item_new_with_label (SupportedMcus[i].mcuName);
    gtk_menu_shell_append (GTK_MENU_SHELL (ProcessorMenu), ProcessorMenuItems[i]);
    }

    ProcessorMenuItems[NUM_SUPPORTED_MCUS] = gtk_check_menu_item_new_with_label ("(no microcontroller)");
    gtk_menu_shell_append (GTK_MENU_SHELL (ProcessorMenu), ProcessorMenuItems[NUM_SUPPORTED_MCUS]);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(MicroControllerMenu), ProcessorMenu);

    // Creating labels for Instruction Menu and adding separators
    InsertCommentMenu = gtk_menu_item_new_with_mnemonic("_Insert Comment");
    InsertContactsMenu = gtk_menu_item_new_with_mnemonic("_Insert Contacts");
    InsertOsrMenu = gtk_menu_item_new_with_mnemonic("_Insert OSR (One Shot Rising)");
    InsertOsfMenu = gtk_menu_item_new_with_mnemonic("_Insert OSF (One Shot Falling)");
    InsertTonMenu = gtk_menu_item_new_with_mnemonic("_Insert TON (Delayed Turn On)");
    InsertTofMenu = gtk_menu_item_new_with_mnemonic("_Insert TOF (Delayed Turn Off)");
    InsertRtoMenu = gtk_menu_item_new_with_mnemonic("_Insert RTO (Retentive Delayed Turn On)");
    InsertCtuMenu = gtk_menu_item_new_with_mnemonic("_Insert CTU (Count Up)");
    InsertCtdMenu = gtk_menu_item_new_with_mnemonic("_Insert CTD (Count Down)");
    InsertCtcMenu = gtk_menu_item_new_with_mnemonic("_Insert CTC (Count Circular)");
    InsertEquMenu = gtk_menu_item_new_with_mnemonic("_Insert EQU (Compare for Equals)");
    InsertNeqMenu = gtk_menu_item_new_with_mnemonic("_Insert NEQ (Compare for Not Equals)");
    InsertGrtMenu = gtk_menu_item_new_with_mnemonic("_Insert GRT (Compare for Greater Than)");
    InsertGeqMenu = gtk_menu_item_new_with_mnemonic("_Insert GEQ (Compare for Greater Than or Equal)");
    InsertLesMenu = gtk_menu_item_new_with_mnemonic("_Insert LES (Compare for Less Than)");
    InsertLeqMenu = gtk_menu_item_new_with_mnemonic("_Insert LEQ (Compare for Less Than or Equal)");
    InsertOpenMenu = gtk_menu_item_new_with_mnemonic("_Insert Open Circuit");
    InsertShortMenu = gtk_menu_item_new_with_mnemonic("_Insert Short Circuit");
    InsertMasterRlyMenu = gtk_menu_item_new_with_mnemonic("_Insert Master Control Relay");
    InsertCoilMenu = gtk_menu_item_new_with_mnemonic("_Insert Coil");
    InsertResMenu = gtk_menu_item_new_with_mnemonic("_Insert RES (Counter/RTO Reset)");
    InsertMovMenu = gtk_menu_item_new_with_mnemonic("_Insert MOV (Move)");
    InsertAddMenu = gtk_menu_item_new_with_mnemonic("_Insert ADD (16-bit Integer Ad)");
    InsertSubMenu = gtk_menu_item_new_with_mnemonic("_Insert SUB (16-bit Integer Subtract)");
    InsertMulMenu = gtk_menu_item_new_with_mnemonic("_Insert MUL (16-bit Integer Multiply)");
    InsertDivMenu = gtk_menu_item_new_with_mnemonic("_Insert DIV (16-bit Integer Division)");
    InsertShiftRegMenu = gtk_menu_item_new_with_mnemonic("_Insert Shift Register");
    InsertLutMenu = gtk_menu_item_new_with_mnemonic("_Insert Look-Up Table");
    InsertPwlMenu = gtk_menu_item_new_with_mnemonic("_Insert Piecewise Linear");
    InsertFmtdStrMenu = gtk_menu_item_new_with_mnemonic("_Insert Formatted String Over UART");
    InsertUartSendMenu = gtk_menu_item_new_with_mnemonic("_Insert UART Send");
    InsertUartRecvMenu = gtk_menu_item_new_with_mnemonic("_Insert UART Receive");
    InsertSetPwmMenu = gtk_menu_item_new_with_mnemonic("_Insert Set PWM Output");
    InsertReadAdcMenu = gtk_menu_item_new_with_mnemonic("_Insert A/D Converter Read");
    InsertPersistMenu = gtk_menu_item_new_with_mnemonic("_Insert Make Persistent");
    MakeNormalMenu = gtk_menu_item_new_with_mnemonic("_Make Normal");
    NegateMenu = gtk_menu_item_new_with_mnemonic("_Make Negated");
    MakeSetOnlyMenu = gtk_menu_item_new_with_mnemonic("_Make Set-Only");
    MakeResetOnlyMenu = gtk_menu_item_new_with_mnemonic("_Make Reset-Only");

    // Appending menu items to Instruction menu and adding separators
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertCommentMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertContactsMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertOsrMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertOsfMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertTonMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertTofMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertRtoMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertCtuMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertCtdMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertCtcMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertEquMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertNeqMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertGrtMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertGeqMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertLesMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertLeqMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertOpenMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertShortMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertMasterRlyMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertCoilMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertResMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertMovMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertAddMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertSubMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertMulMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertDivMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL(InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertShiftRegMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertLutMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertPwlMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertFmtdStrMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertUartSendMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertUartRecvMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertSetPwmMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertReadAdcMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InsertPersistMenu);
    InstructionMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), InstructionMenuSeparator);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MakeNormalMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), NegateMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MakeSetOnlyMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (InstructionMenu), MakeResetOnlyMenu);

    // Creating labels for Compile Menu
    CompileMenu = gtk_menu_item_new_with_mnemonic("_Compile");
    CompileAsMenu = gtk_menu_item_new_with_mnemonic("_Compile As...");

    // Appending menu items to Compile menu
    gtk_menu_shell_append(GTK_MENU_SHELL (Compile), CompileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (Compile), CompileAsMenu);

    // Creating labels for Help Menu
    ManualMenu = gtk_menu_item_new_with_mnemonic("_Manual...");
    AboutMenu = gtk_menu_item_new_with_mnemonic("_About...");

    // Appending menu items to Help menu
    gtk_menu_shell_append(GTK_MENU_SHELL (Help), ManualMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (Help), AboutMenu);

    // Creating labels for Simulation Menu
    SimulationModeMenu = gtk_check_menu_item_new_with_mnemonic("_Simulation Mode");
    StartSimulationMenu = gtk_menu_item_new_with_mnemonic("_Start Real-Time Simulation");
    StopSimulationMenu = gtk_menu_item_new_with_mnemonic("_Halt Simulation");
    SingleCycleMenu = gtk_menu_item_new_with_mnemonic("_Single Cycle");

    // Appending menu items to Simulate menu and adding separators
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), SimulationModeMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), StartSimulationMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), StopSimulationMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL (SimulateMenu), SingleCycleMenu);
    SimulateMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(SimulateMenu), SimulateMenuSeparator);

    // Creating submenus for each menu  
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(FileLabel), FileMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(EditLabel), EditMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(SettingsLabel), Settings);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(InstructionLabel), InstructionMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(SimulateLabel), SimulateMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(CompileLabel), Compile);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(HelpLabel), Help);

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
    AddMenuAccelerators ();
    
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
    /// Pane to separate Scrolled Window and other widgets
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

    /// Creating a list
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

    /// Appending Menus to grid
    gtk_grid_attach (GTK_GRID (grid), MakeMainWindowMenus(), 0, 0, 1, 1);

    /// Creating Scrolled Window
    ScrollWindow = gtk_scrolled_window_new (NULL, NULL);
    HWID viewport = gtk_viewport_new (NULL,NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (ScrollWindow),
				                          GTK_POLICY_AUTOMATIC, 
				                          GTK_POLICY_ALWAYS);
    gtk_widget_set_hexpand(GTK_WIDGET(ScrollWindow), TRUE);  
    gtk_widget_set_vexpand(GTK_WIDGET(ScrollWindow), TRUE);

    /// Adding DrawWindow to pane
    gtk_container_add (GTK_CONTAINER(viewport), DrawWindow);
    gtk_container_add (GTK_CONTAINER(ScrollWindow), viewport);
    gtk_paned_pack1 (GTK_PANED (pane), ScrollWindow, TRUE, TRUE);
    gtk_paned_set_position (GTK_PANED (pane), 0);    

    /// Appending tree view to pane and pane to grid
    gtk_paned_pack2 (GTK_PANED(pane), view, FALSE, FALSE);
    gtk_paned_set_position (GTK_PANED (pane), 400);
    gtk_grid_attach (GTK_GRID (grid), pane, 0, 0, 1, 1); 

    /// Creating Status Bar and attaching to grid
    StatusBar = gtk_statusbar_new();
    gtk_statusbar_push (GTK_STATUSBAR (StatusBar),
                        gtk_statusbar_get_context_id (GTK_STATUSBAR (StatusBar),
                        "Introduction"), "LDMicro Started");

    /// Appneding Status Bar to box which is then added to Main Window
    gtk_box_pack_start(GTK_BOX(PackBox), grid, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(PackBox), StatusBar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(MainWindow), PackBox);
}

//-----------------------------------------------------------------------------
// Adjust the size and visibility of the scrollbars as necessary, either due
// to a change in the size of the program or a change in the size of the
// window.
//-----------------------------------------------------------------------------
void RefreshScrollbars(void)
{
    SCROLLINFO vert, horiz;
    SetUpScrollbars(&NeedHoriz, &horiz, &vert);

    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(ScrollWindow));
    // g_print("adj = %f\n", gtk_adjustment_get_value(adjustment));
    // g_print("upper = %f\n", gtk_adjustment_get_upper(adjustment) - gtk_widget_get_allocated_height (ScrollWindow));
    // g_print("lower = %f\n", gtk_adjustment_get_lower(adjustment));
    // g_print("inc = %f\n", gtk_adjustment_get_step_increment(adjustment));
    // g_print("w width = %i\n", gtk_widget_get_allocated_width (DrawWindow));
    // g_print("w height = %i\n", gtk_widget_get_allocated_height (ScrollWindow));

    // SetScrollInfo(HorizScrollBar, SB_CTL, &horiz, TRUE);
    // SetScrollInfo(VertScrollBar, SB_CTL, &vert, TRUE);

    // RECT main;
    // GetClientRect(MainWindow, &main);

    // if(NeedHoriz) {
    //     MoveWindow(HorizScrollBar, 0, IoListTop - ScrollHeight - 2,
    //         main.right - ScrollWidth - 2, ScrollHeight, TRUE);
    //     ShowWindow(HorizScrollBar, SW_SHOW);
    //     EnableWindow(HorizScrollBar, TRUE);
    // } else {
    //     ShowWindow(HorizScrollBar, SW_HIDE);
    // }
    // MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, 1, ScrollWidth,
    //     NeedHoriz ? (IoListTop - ScrollHeight - 4) : (IoListTop - 3), TRUE);

    // MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, 1, ScrollWidth,
    //     NeedHoriz ? (IoListTop - ScrollHeight - 4) : (IoListTop - 3), TRUE);

    InvalidateRect(DrawWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Respond to a WM_VSCROLL sent to the main window, presumably by the one and
// only vertical scrollbar that it has as a child.
//-----------------------------------------------------------------------------
void VscrollProc(int wParam)
{
    int prevY = ScrollYOffset;
    switch(wParam) {
        case SB_LINEUP:
        case SB_PAGEUP:
            if(ScrollYOffset > 0) {
                ScrollYOffset--;
            }
            break;

        case SB_LINEDOWN:
        case SB_PAGEDOWN:
            if(ScrollYOffset < ScrollYOffsetMax) {
                ScrollYOffset++;
            }
            break;

        case SB_TOP:
            ScrollYOffset = 0;
            break;

        case SB_BOTTOM:
            ScrollYOffset = ScrollYOffsetMax;
            break;

        // case SB_THUMBTRACK:
        // case SB_THUMBPOSITION:
            // ScrollYOffset = HIWORD(wParam);
            // break;
    }
    // if(prevY != ScrollYOffset) {
    //     SCROLLINFO si;
    //     si.cbSize = sizeof(si);
    //     si.fMask = SIF_POS;
    //     si.nPos = ScrollYOffset;
    //     SetScrollInfo(VertScrollBar, SB_CTL, &si, TRUE);

    //     InvalidateRect(MainWindow, NULL, FALSE);
    // }
}

//-----------------------------------------------------------------------------
// Respond to a WM_HSCROLL sent to the main window, presumably by the one and
// only horizontal scrollbar that it has as a child.
//-----------------------------------------------------------------------------
void HscrollProc(int wParam)
{
    int prevX = ScrollXOffset;
    switch(wParam) {
        case SB_LINEUP:
            ScrollXOffset -= FONT_WIDTH;
            break;

        case SB_PAGEUP:
            ScrollXOffset -= POS_WIDTH*FONT_WIDTH;
            break;

        case SB_LINEDOWN:
            ScrollXOffset += FONT_WIDTH;
            break;

        case SB_PAGEDOWN:
            ScrollXOffset += POS_WIDTH*FONT_WIDTH;
            break;

        case SB_TOP:
            ScrollXOffset = 0;
            break;

        case SB_BOTTOM:
            ScrollXOffset = ScrollXOffsetMax;
            break;

        // case SB_THUMBTRACK:
        // case SB_THUMBPOSITION:
            // ScrollXOffset = HIWORD(wParam);
            // break;
    }

    if(ScrollXOffset > ScrollXOffsetMax) ScrollXOffset = ScrollXOffsetMax;
    if(ScrollXOffset < 0) ScrollXOffset = 0;

    // if(prevX != ScrollXOffset) {
    //     SCROLLINFO si;
    //     si.cbSize = sizeof(si);
    //     si.fMask = SIF_POS;
    //     si.nPos = ScrollXOffset;
    //     SetScrollInfo(HorizScrollBar, SB_CTL, &si, TRUE);

    //     InvalidateRect(MainWindow, NULL, FALSE);
    // }
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
    EnableMenuItem(EditMenu, PushRungUpMenu,
        canPushUp ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, PushRungDownMenu,
        canPushDown ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, DeleteRungMenu,
        (Prog.numRungs > 1) ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, NegateMenu,
        canNegate ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MakeNormalMenu,
        canNormal ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MakeResetOnlyMenu,
        canResetOnly ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MakeSetOnlyMenu,
        canSetOnly ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, InsertCommentMenu,
        canInsertComment ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(EditMenu, DeleteElementMenu,
        canDelete ? MF_ENABLED : MF_GRAYED);

    int t;
    t = canInsertEnd ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, InsertCoilMenu, t);
    EnableMenuItem(InstructionMenu, InsertResMenu, t);
    EnableMenuItem(InstructionMenu, InsertMovMenu, t);
    EnableMenuItem(InstructionMenu, InsertAddMenu, t);
    EnableMenuItem(InstructionMenu, InsertSubMenu, t);
    EnableMenuItem(InstructionMenu, InsertMulMenu, t);
    EnableMenuItem(InstructionMenu, InsertDivMenu, t);
    EnableMenuItem(InstructionMenu, InsertCtcMenu, t);
    EnableMenuItem(InstructionMenu, InsertPersistMenu, t);
    EnableMenuItem(InstructionMenu, InsertReadAdcMenu, t);
    EnableMenuItem(InstructionMenu, InsertSetPwmMenu, t);
    EnableMenuItem(InstructionMenu, InsertMasterRlyMenu, t);
    EnableMenuItem(InstructionMenu, InsertShiftRegMenu, t);
    EnableMenuItem(InstructionMenu, InsertLutMenu, t);
    EnableMenuItem(InstructionMenu, InsertPwlMenu, t);

    t = canInsertOther ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, InsertTonMenu, t);
    EnableMenuItem(InstructionMenu, InsertTofMenu, t);
    EnableMenuItem(InstructionMenu, InsertOsrMenu, t);
    EnableMenuItem(InstructionMenu, InsertOsfMenu, t);
    EnableMenuItem(InstructionMenu, InsertRtoMenu, t);
    EnableMenuItem(InstructionMenu, InsertContactsMenu, t);
    EnableMenuItem(InstructionMenu, InsertCtuMenu, t);
    EnableMenuItem(InstructionMenu, InsertCtdMenu, t);
    EnableMenuItem(InstructionMenu, InsertEquMenu, t);
    EnableMenuItem(InstructionMenu, InsertNeqMenu, t);
    EnableMenuItem(InstructionMenu, InsertGrtMenu, t);
    EnableMenuItem(InstructionMenu, InsertGeqMenu, t);
    EnableMenuItem(InstructionMenu, InsertLesMenu, t);
    EnableMenuItem(InstructionMenu, InsertLeqMenu, t);
    EnableMenuItem(InstructionMenu, InsertShortMenu, t);
    EnableMenuItem(InstructionMenu, InsertOpenMenu, t);
    EnableMenuItem(InstructionMenu, InsertUartSendMenu, t);
    EnableMenuItem(InstructionMenu, InsertUartRecvMenu, t);
    EnableMenuItem(InstructionMenu, InsertFmtdStrMenu, t);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the undo/redo menus.
//-----------------------------------------------------------------------------
void SetUndoEnabled(BOOL undoEnabled, BOOL redoEnabled)
{
    EnableMenuItem(EditMenu, UndoMenu, undoEnabled ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, RedoMenu, redoEnabled ? MF_ENABLED : MF_GRAYED);
}

//-----------------------------------------------------------------------------
// Toggle whether we are in simulation mode. A lot of options are only
// available in one mode or the other.
//-----------------------------------------------------------------------------
void ToggleSimulationMode(void)
{
    InSimulationMode = !InSimulationMode;
    if(InSimulationMode) {
        EnableMenuItem(SimulateMenu, StartSimulationMenu, MF_ENABLED);
        EnableMenuItem(SimulateMenu, SingleCycleMenu, MF_ENABLED);

        EnableMenuItem(FileMenu, OpenMenu, MF_GRAYED);
        EnableMenuItem(FileMenu, SaveMenu, MF_GRAYED);
        EnableMenuItem(FileMenu, SaveAsMenu_AS, MF_GRAYED);
        EnableMenuItem(FileMenu, NewMenu, MF_GRAYED);
        EnableMenuItem(FileMenu, ExportMenu, MF_GRAYED);

        EnableMenuItem(TopMenu, EditMenu, MF_GRAYED);
        EnableMenuItem(TopMenu, Settings, MF_GRAYED);
        EnableMenuItem(TopMenu, InstructionMenu, MF_GRAYED);
        EnableMenuItem(TopMenu, Compile, MF_GRAYED);
    
        CheckMenuItem(SimulateMenu, SimulationModeMenu, MF_CHECKED);

        // ClearSimulationData(); // simulation.cpp, ldmicro.h
        // Recheck InSimulationMode, because there could have been a Compile
        // error, which would have kicked us out of simulation mode.
        // if(UartFunctionUsed() && InSimulationMode) {
        //     ShowUartSimulationWindow(); // simulate.cpp
        // }
    }
    else {
        RealTimeSimulationRunning = FALSE;
        // KillTimer(MainWindow, TIMER_SIMULATE);

        EnableMenuItem(SimulateMenu, StartSimulationMenu, MF_GRAYED);
        EnableMenuItem(SimulateMenu, StopSimulationMenu, MF_GRAYED);
        EnableMenuItem(SimulateMenu, SingleCycleMenu, MF_GRAYED);

        EnableMenuItem(FileMenu, OpenMenu, MF_ENABLED);
        EnableMenuItem(FileMenu, SaveMenu, MF_ENABLED);
        EnableMenuItem(FileMenu, SaveAsMenu_AS, MF_ENABLED);
        EnableMenuItem(FileMenu, NewMenu, MF_ENABLED);
        EnableMenuItem(FileMenu, ExportMenu, MF_ENABLED);

        EnableMenuItem(TopMenu, EditMenu, MF_ENABLED);
        EnableMenuItem(TopMenu, Settings, MF_ENABLED);
        EnableMenuItem(TopMenu, InstructionMenu, MF_ENABLED);
        EnableMenuItem(TopMenu, Compile, MF_ENABLED);

        CheckMenuItem(SimulateMenu, SimulationModeMenu, MF_UNCHECKED);

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
            CheckMenuItem(ProcessorMenu, ProcessorMenuItems[i], MF_CHECKED);
        }
        else {
            CheckMenuItem(ProcessorMenu, ProcessorMenuItems[i], MF_UNCHECKED);
        }
    }
    // `(no microcontroller)' setting
    if (!Prog.mcu){
        CheckMenuItem(ProcessorMenu, ProcessorMenuItems[NUM_SUPPORTED_MCUS], MF_CHECKED);
    }
    else {
        CheckMenuItem(ProcessorMenu, ProcessorMenuItems[NUM_SUPPORTED_MCUS], MF_UNCHECKED);
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
    RECT main;
    GetClientRect(DrawWindow, &main);

    RECT status;
    GetWindowRect(StatusBar, &status);
    int statusHeight = status.bottom - status.top;

    // MoveWindow(StatusBar, 0, main.bottom - statusHeight, main.right,
        // statusHeight, TRUE);

    // Make sure that the I/O list can't disappear entirely.
    if(IoListHeight < 30) {
        IoListHeight = 30;
    }
    IoListTop = main.bottom ;//- IoListHeight - statusHeight;

    // Make sure that we can't drag the top of the I/O list above the
    // bottom of the menu bar, because it then becomes inaccessible.
    if(IoListTop < 5) {
        IoListHeight = main.bottom - statusHeight - 5;
        IoListTop = main.bottom - IoListHeight - statusHeight;
    }
    // MoveWindow(IoList, 0, IoListTop, main.right, IoListHeight, TRUE);

    RefreshScrollbars();
    
    InvalidateRect(DrawWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Start real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StartSimulation(void)
{
    RealTimeSimulationRunning = TRUE;
    EnableMenuItem(SimulateMenu, StartSimulationMenu, MF_GRAYED);
    EnableMenuItem(SimulateMenu, StopSimulationMenu, MF_ENABLED);
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

    EnableMenuItem(SimulateMenu, StartSimulationMenu, MF_ENABLED);
    EnableMenuItem(SimulateMenu, StopSimulationMenu, MF_GRAYED);
    // KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}