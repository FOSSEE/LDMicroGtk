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
// Dialog for setting the properties of a relay coils: negated or not,
// plus the name, plus set-only or reset-only
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <linuxUI.h>
#include <stdio.h>
//#include <commctrl.h>

#include "ldmicro.h"

static HWID CoilDialog;

static HWID SourceInternalRelayRadio;
static HWID SourceMcuPinRadio;
static HWID NegatedRadio;
static HWID NormalRadio;
static HWID SetOnlyRadio;
static HWID ResetOnlyRadio;
static HWID NameTextbox;

static LONG_PTR PrevNameProc;

HWID CoilGrid;
HWID CoilPackingBox;

// //-----------------------------------------------------------------------------
// // Don't allow any characters other than A-Za-z0-9_ in the name.
// //-----------------------------------------------------------------------------
// // static LRESULT CALLBACK MyNameProc(HWND hwnd, UINT msg, WPARAM wParam,
// //     LPARAM lParam)
// // {
// //     if(msg == WM_CHAR) {
// //         if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' ||
// //             wParam == '\b'))
// //         {
// //             return 0;
// //         }
// //     }

// //     return CallWindowProc((WNDPROC)PrevNameProc, hwnd, msg, wParam, lParam);
// // }

// void CoilDialogMyNameProc (GtkEditable *editable, gchar *NewText, gint length, 
//     gint *position, gpointer data){
//     // gtk_widget_set_sensitive (MainWindow, TRUE);
//     for (int i = 0; i < length; i++){
//         if (!(isalpha (NewText[i]) || NewText[i] == '_' || isdigit (NewText[i])
//                                      || NewText[i] == '\b' )){
//             g_signal_stop_emission_by_name (G_OBJECT (editable), "insert-text");
//             return;
//         }
//     }
// }


// static void MakeControls(void)
// {
//     NormalRadio = CreateWindowEx(0, WC_BUTTON, _("( ) Normal"),
//         WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
//         16, 21, 100, 20, CoilDialog, NULL, Instance, NULL);
//     NiceFont(NormalRadio);

//     NormalRadio = gtk_radio_button_new_with_label (NULL, "( ) Normal");
//     NegatedRadio = gtk_radio_button_new_with_label_from_widget
//                         (GTK_RADIO_BUTTON (NormalRadio), "(/) Negated");
//     SetOnlyRadio = gtk_radio_button_new_with_label_from_widget
//                         (GTK_RADIO_BUTTON (NormalRadio), "(S) Set-Only");
//     ResetOnlyRadio = gtk_radio_button_new_with_label_from_widget
//                         (GTK_RADIO_BUTTON (NormalRadio), "(R) Reset-Only");
    
//     SourceInternalRelayRadio = gtk_radio_button_new_with_label (NULL, "Internal Relay");
//     SourceMcuPinRadio = gtk_radio_button_new_with_label_from_widget
//                         (GTK_RADIO_BUTTON (SourceInternalRelayRadio), "Pin on MCU");
    
//     HWID textLabel = gtk_label_new ("Name:");
    
//     NameTextbox = gtk_entry_new();
//     gtk_entry_set_max_length (GTK_ENTRY (NameTextbox), 0);

//     OkButton = gtk_button_new_with_label ("OK");
//     CancelButton = gtk_button_new_with_label ("Cancel");

//     gtk_grid_attach (GTK_GRID (CoilGrid), SourceInternalRelayRadio, 1, 2, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), SourceInputPinRadio, 1, 3, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), SourceOutputPinRadio, 1, 4, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), textLabel, 2, 2, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), NegatedCheckbox, 2, 3, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), NameTextbox, 3, 2, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), OkButton, 4, 2, 1, 1);
//     gtk_grid_attach (GTK_GRID (CoilGrid), CancelButton, 4, 3, 1, 1);

//     gtk_grid_set_column_spacing (GTK_GRID (CoilGrid), 1);
//     gtk_box_pack_start(GTK_BOX(CoilPackingBox), CoilGrid, TRUE, TRUE, 0);


//     // PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
//     //     (LONG_PTR)MyNameProc);
// }

// void ShowCoilDialog(BOOL *negated, BOOL *setOnly, BOOL *resetOnly, char *name)
// {
//     CoilGrid = gtk_grid_new();
//     CoilPackingBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

//     CoilDialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(CoilDialog), "Coil");
//     gtk_window_set_default_size(GTK_WINDOW(CoilDialog), 100, 50);
//     gtk_window_set_resizable (GTK_WINDOW (CoilDialog), FALSE);
//     gtk_container_add(GTK_CONTAINER(CoilDialog), CoilPackingBox);
//     gtk_widget_add_events (CoilDialog, GDK_KEY_PRESS_MASK);
//     gtk_widget_add_events (CoilDialog, GDK_BUTTON_PRESS_MASK);

//     MakeControls();
   
//     if(name[0] == 'R') {
//         SendMessage(SourceInternalRelayRadio, BM_SETCHECK, BST_CHECKED, 0);
//     } else {
//         SendMessage(SourceMcuPinRadio, BM_SETCHECK, BST_CHECKED, 0);
//     }
//     SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name + 1));
//     if(*negated) {
//         SendMessage(NegatedRadio, BM_SETCHECK, BST_CHECKED, 0);
//     } else if(*setOnly) {
//         SendMessage(SetOnlyRadio, BM_SETCHECK, BST_CHECKED, 0);
//     } else if(*resetOnly) {
//         SendMessage(ResetOnlyRadio, BM_SETCHECK, BST_CHECKED, 0);
//     } else {
//         SendMessage(NormalRadio, BM_SETCHECK, BST_CHECKED, 0);
//     }

//     EnableWindow(MainWindow, FALSE);
//     ShowWindow(CoilDialog, TRUE);
//     SetFocus(NameTextbox);
//     SendMessage(NameTextbox, EM_SETSEL, 0, -1);

//     MSG msg;
//     DWORD ret;
//     DialogDone = FALSE;
//     DialogCancel = FALSE;
//     while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
//         if(msg.message == WM_KEYDOWN) {
//             if(msg.wParam == VK_RETURN) {
//                 DialogDone = TRUE;
//                 break;
//             } else if(msg.wParam == VK_ESCAPE) {
//                 DialogDone = TRUE;
//                 DialogCancel = TRUE;
//                 break;
//             }
//         }

//         if(IsDialogMessage(CoilDialog, &msg)) continue;
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }

//     if(!DialogCancel) {
//         if(SendMessage(SourceInternalRelayRadio, BM_GETSTATE, 0, 0)
//             & BST_CHECKED)
//         {
//             name[0] = 'R';
//         } else {
//             name[0] = 'Y';
//         }
//         SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(name+1));

//         if(SendMessage(NormalRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
//             *negated = FALSE;
//             *setOnly = FALSE;
//             *resetOnly = FALSE;
//         } else if(SendMessage(NegatedRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
//             *negated = TRUE;
//             *setOnly = FALSE;
//             *resetOnly = FALSE;
//         } else if(SendMessage(SetOnlyRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
//             *negated = FALSE;
//             *setOnly = TRUE;
//             *resetOnly = FALSE;
//         } else if(SendMessage(ResetOnlyRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
//         {
//             *negated = FALSE;
//             *setOnly = FALSE;
//             *resetOnly = TRUE;
//         }
//     }

//     EnableWindow(MainWindow, TRUE);
//     DestroyWindow(CoilDialog);
//     return;
// }
