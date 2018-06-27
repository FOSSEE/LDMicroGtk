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
// Dialog for entering the elements of a look-up table. I allow two formats:
// as a simple list of integer values, or like a string. The lookup table
// can either be a straight LUT, or one with piecewise linear interpolation
// in between the points.
// Jonathan Westhues, Dec 2005
//-----------------------------------------------------------------------------
#include "linuxUI.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <commctrl.h>

#include "ldmicro.h"

using namespace std;

static HWID LutDialog;

static HWID AsStringCheckbox;
static HWID CountTextbox;
static HWID DestTextbox;
static HWID IndexTextbox;
static HWID Labels[3];

static HWID StringTextbox;
static bool checkString;

static BOOL WasAsString;
static int WasCount;

static HWID ValuesTextbox[MAX_LOOK_UP_TABLE_LEN];
static LONG_PTR PrevValuesProc[MAX_LOOK_UP_TABLE_LEN];
static HWID ValuesLabel[MAX_LOOK_UP_TABLE_LEN];

static SWORD ValuesCache[MAX_LOOK_UP_TABLE_LEN];

static LONG_PTR PrevDestProc;
static LONG_PTR PrevIndexProc;
static LONG_PTR PrevCountProc;
static HWID OkButton;
static HWID CancelButton;

HWID LutGrid;
HWID LutPackingBox;

struct ShowLookUpTableDialogBuffer{
    int tmpcount;
    bool tmpasString;
    char PrevTableAsString[1024] = "";
} temp;

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 and minus in the values.
//-----------------------------------------------------------------------------

void LutDialogMyNumberProc (GtkEditable *editable, gchar *NewText, gint length, 
    gint *position, gpointer data){
    // gtk_widget_set_sensitive (MainWindow, TRUE);
    for (int i = 0; i < length; i++){
        if (!(isdigit (NewText[i]) || NewText[i] == '\b' || NewText[i] == '-')){
            g_signal_stop_emission_by_name (G_OBJECT (editable), "insert-text");
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 in the count.
//-----------------------------------------------------------------------------

void LutDialogMyDigitsProc (GtkEditable *editable, gchar *NewText, gint length, 
    gint *position, gpointer data){
    // gtk_widget_set_sensitive (MainWindow, TRUE);
    for (int i = 0; i < length; i++){
        if (!(isdigit (NewText[i]) || NewText[i] == '\b' )){
            g_signal_stop_emission_by_name (G_OBJECT (editable), "insert-text");
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than A-Za-z0-9_ in the name.
//-----------------------------------------------------------------------------

void LutDialogMyNameProc (GtkEditable *editable, gchar *NewText, gint length, 
    gint *position, gpointer data){
    // gtk_widget_set_sensitive (MainWindow, TRUE);
    for (int i = 0; i < length; i++){
        if (!(isalpha (NewText[i]) || NewText[i] == '_' || isdigit (NewText[i])
                                     || NewText[i] == '\b' )){
            g_signal_stop_emission_by_name (G_OBJECT (editable), "insert-text");
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Make the controls that are guaranteed not to move around as the count/
// as string settings change. This is different for the piecewise linear,
// because in that case we should not provide a checkbox to change whether
// the table is edited as a string or table.
//-----------------------------------------------------------------------------
static void MakeFixedControls(BOOL forPwl)
{
    // cout << "Inside MakeFixedControls ()" << "\n";
    Labels[0] = gtk_label_new ("Destination");
    Labels[1] = gtk_label_new ("Index:");
    Labels[2] = forPwl ? gtk_label_new ("Points:") : gtk_label_new ("Count:");

    DestTextbox = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (DestTextbox), 0);
    IndexTextbox = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (IndexTextbox), 0);
    CountTextbox = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (CountTextbox), 0);

    // cout << "Created Labels, Dest, Index and Count text boxes" << "\n";

    if(!forPwl) {
        AsStringCheckbox = gtk_check_button_new_with_label
                ("Edit table of ASCII values like a string");
        // cout << "Created CheckButton" << "\n";
    }


    OkButton = gtk_button_new_with_label ("OK");
    CancelButton = gtk_button_new_with_label ("Cancel");

    gtk_grid_attach (GTK_GRID (LutGrid), Labels[0], 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), DestTextbox, 1, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), OkButton, 3, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), Labels[1], 0, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), IndexTextbox, 1, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), CancelButton, 3, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), Labels[2], 0, 6, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), CountTextbox, 1, 6, 1, 1);
    gtk_grid_attach (GTK_GRID (LutGrid), AsStringCheckbox, 0, 8, 1, 1);

    g_signal_connect (G_OBJECT(DestTextbox), "insert-text",
		     G_CALLBACK(LutDialogMyNameProc), NULL);
    g_signal_connect (G_OBJECT(IndexTextbox), "insert-text",
		     G_CALLBACK(LutDialogMyNameProc), NULL);
    g_signal_connect (G_OBJECT(CountTextbox), "insert-text",
		     G_CALLBACK(LutDialogMyDigitsProc), NULL);
}

//-----------------------------------------------------------------------------
// Destroy all of the controls so that we can start anew. This is necessary
// because if the size of the LUT changes, or if the user switches from 
// table entry to string entry, we must completely reconfigure the dialog.
//-----------------------------------------------------------------------------
static void DestroyLutControls(void)
{
    cout << "Inside DestroyLutControls ()" << "\n";
    if(WasAsString) {
        // Nothing to do; we constantly update the cache from the user-
        // specified string, because we might as well do that when we
        // calculate the length.
    }
    else {
        cout << "In DestroyLut WasCount : " << WasCount << "\n";
        int i;
        for(i = 0; i < WasCount; i++) {
            char buf[20];
            // SendMessage(ValuesTextbox[i], WM_GETTEXT, (WPARAM)16, (LPARAM)buf);
            strcpy (buf, gtk_entry_get_text (GTK_ENTRY (ValuesTextbox[i])));
            ValuesCache[i] = atoi(buf);
        }
        cout << "Exiting else of WasAsString" << "\n";
    }
    if (checkString){
        DestroyWindow(StringTextbox);
        cout << "Destroyed StringTextbox" << "\n";
    }
    int i;
    // *** Changed from MAX_LOOK_UP_TABLE_LEN to count ***** //
    for(i = 0; i < temp.tmpcount; i++) {
        cout << "Destroy ValueTextbox i = " << i << "\n";
        DestroyWindow(ValuesTextbox[i]);
        DestroyWindow(ValuesLabel[i]);
        cout << "Called DestroyWindow() for ValuesTextbox and ValuesLabels" << "\n";
    }
}

//-----------------------------------------------------------------------------
// Make the controls that hold the LUT. The exact configuration of the dialog
// will depend on (a) whether the user chose table-type or string-type entry,
// and for table-type entry, on (b) the number of entries, and on (c)
// whether we are editing a PWL table (list of points) or a straight LUT.
//-----------------------------------------------------------------------------
static void MakeLutControls(BOOL asString, int count, BOOL forPwl)
{
    // Remember these, so that we know from where to cache stuff if we have
    // to destroy these textboxes and make something new later.
    WasAsString = asString;
    cout << "asString in MakeLutControls : " << asString <<"\n";
    WasCount = count;
    cout << "Count in MakeLutControls : " << count << "\n";
    if(forPwl && asString) oops();

    if(asString) {
        char str[3*MAX_LOOK_UP_TABLE_LEN+1];
        cout << "Inside if (asString) in MakeLutControls" << "\n";
        int i, j;
        j = 0;
        for(i = 0; i < count; i++) {
            int c = ValuesCache[i];
            if(c >= 32 && c <= 127 && c != '\\') {
                str[j++] = c;
            } else if(c == '\\') {
                str[j++] = '\\';
                str[j++] = '\\';
            } else if(c == '\r') {
                str[j++] = '\\';
                str[j++] = 'r';
            } else if(c == '\b') {
                str[j++] = '\\';
                str[j++] = 'b';
            } else if(c == '\f') {
                str[j++] = '\\';
                str[j++] = 'f';
            } else if(c == '\n') {
                str[j++] = '\\';
                str[j++] = 'n';
            } else {
                str[j++] = 'X';
            }
        }
        str[j++] = '\0';
            StringTextbox = gtk_entry_new ();
            gtk_grid_attach (GTK_GRID (LutGrid), StringTextbox, 1, 9, 1, 1);
            checkString = TRUE;
            gtk_widget_show_all (LutGrid);
            gtk_editable_set_editable (GTK_EDITABLE (CountTextbox), FALSE);
            // MoveWindow(StringTextbox, 100, 30, 320, 185, TRUE); // Changed LutDialog to StringTextbox
    }
    else {
        int i;
        int base;
        if(forPwl) {
            base = 100;
            }
        else {
            base = 140;
            }
        for(i = 0; i < count; i++) {
            int x, y;

            if(i < 16) {
                x = 10;
                y = base+30*i;
                }
            else {
                x = 160;
                y = base+30*(i-16);
                }

            char buf[20];
            sprintf(buf, "%d", ValuesCache[i]);
            ValuesTextbox[i] = gtk_entry_new ();
            gtk_entry_set_max_length (GTK_ENTRY (ValuesTextbox[i]), 0);
            // cout << "Made ValuesTextbox" << "\n";

            if(forPwl) {
                sprintf(buf, "%c%d:", (i & 1) ? 'y' : 'x', i/2);
                }
            else {
                sprintf(buf, "%2d:", i);
                }

            ValuesLabel[i] = gtk_label_new (buf);
            if (i<10){
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesLabel[i], 0, i+12, 1, 1);
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesTextbox[i], 1, i+12, 1, 1);
            }
            else if ((i>=10) && (i<20)){
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesLabel[i], 2, i+2, 1, 1);
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesTextbox[i], 3, i+2, 1, 1);
            }
            else if ((i>=20) && (i<30)){
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesLabel[i], 4, i-8, 1, 1);
                gtk_grid_attach (GTK_GRID (LutGrid), ValuesTextbox[i], 5, i-8, 1, 1);   
            }
            gtk_widget_show_all (LutDialog);
            g_signal_connect (G_OBJECT(ValuesTextbox[i]), "insert-text",
		     G_CALLBACK(LutDialogMyNumberProc), NULL);
        }
        if(count > 16) count = 16;
        gtk_editable_set_editable (GTK_EDITABLE (CountTextbox), TRUE);

        // MoveWindow(LutDialog, 100, 30, 320, base + 30 + count*30, TRUE);
    }
}

//-----------------------------------------------------------------------------
// Decode a string into a look-up table; store the values in ValuesCache[],
// and update the count checkbox (which in string mode is read-only) to
// reflect the new length. Returns FALSE if the new string is too long, else
// TRUE.
//-----------------------------------------------------------------------------
BOOL StringToValuesCache(char *str, int *c)
{   
    // cout << "Inside StringToValuesCache" << "\n";
    int count = 0;
    while(*str) {
        if(*str == '\\') {
            str++;
            switch(*str) {
                case 'r': ValuesCache[count++] = '\r'; break;
                case 'n': ValuesCache[count++] = '\n'; break;
                case 'f': ValuesCache[count++] = '\f'; break;
                case 'b': ValuesCache[count++] = '\b'; break;
                default:  ValuesCache[count++] = *str; break;
            }
        } else {
            ValuesCache[count++] = *str;
        }
        if(*str) {
            str++;
        }
        if(count >= 32) {
            return FALSE;
        }
    }

    char buf[10];
    sprintf(buf, "%d", count);
    gtk_entry_set_text (GTK_ENTRY (CountTextbox), buf);
    *c = count;
    return TRUE;
}

void LookUpTableGetData (HWID widget, gpointer data){
    ElemLeaf *l = (ElemLeaf *) data;
    ElemLookUpTable *t = &(l->d.lookUpTable);
    strcpy (t->dest, gtk_entry_get_text (GTK_ENTRY (DestTextbox)));
    // t->index = const_cast <char*> (gtk_entry_get_text (GTK_ENTRY (IndexTextbox)));
    strcpy (t->index, gtk_entry_get_text (GTK_ENTRY (IndexTextbox)));
    // DestroyLutControls();

    // The call to DestroyLutControls updated ValuesCache, so just read
    // them out of there (whichever mode we were in before).
    int i;
    for(i = 0; i < temp.tmpcount; i++) {
        t->vals[i] = ValuesCache[i];
        }
    t->count = temp.tmpcount;
    t->editAsString = temp.tmpasString;
    gtk_widget_set_sensitive (MainWindow, TRUE);
}

void LookUpTableCheckMode (void){
    int count = temp.tmpcount;
    bool asString = temp.tmpasString;

    // Are we in table mode? In that case watch the (user-editable) count
    // field, and use that to determine how many textboxes to show.
    char buf[20];
    // buf = const_cast <char*> (gtk_entry_get_text (GTK_ENTRY (CountTextbox)));
    strcpy (buf, gtk_entry_get_text (GTK_ENTRY (CountTextbox)));
    if(atoi(buf) != count && !asString) {
        count = atoi(buf);
        if(count < 0 || count > 32) {
            count = 0;
            gtk_entry_set_text (GTK_ENTRY (CountTextbox), "");
        }
        cout << "Count in LookUpTableCheckMode : " << count << "\n";
        DestroyLutControls();
        MakeLutControls(asString, count, FALSE);
    }

    // Are we in string mode? In that case watch the string textbox,
    // and use that to update the (read-only) count field.
    if(asString) {
        char scratch[1024];
        // scratch = const_cast <char*> (gtk_entry_get_text 
        //                         (GTK_ENTRY (StringTextbox)));
        strcpy (scratch, gtk_entry_get_text (GTK_ENTRY (StringTextbox)));
        if(strcmp(scratch, temp.PrevTableAsString)!=0) {
            if(StringToValuesCache(scratch, &count)) {
                strcpy(temp.PrevTableAsString, scratch);
            }
            else {
                // Too long; put back the old one
                gtk_entry_set_text (GTK_ENTRY (StringTextbox),
                                    temp.PrevTableAsString);
            }
        }
    }
    // Did we just change modes?
    BOOL x = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (AsStringCheckbox));
    if((x && !asString) || (!x && asString)) {
        cout << "x is " << x << "\n";    
        asString = x;
        if (x == 1){
            MakeLutControls(asString, count, FALSE);
        }
        else {
            DestroyLutControls();
        }
    }

    temp.tmpcount = count;
    temp.tmpasString = asString;
}

// Checks for the required key press
gboolean LookUpTableKeyPress (HWID widget, GdkEventKey* event, gpointer data){
    LookUpTableCheckMode ();
    if (event -> keyval == GDK_KEY_Return){
        LookUpTableGetData(NULL, (gpointer) data);
    }
    else if (event -> keyval == GDK_KEY_Escape){
        DestroyWindow (LutDialog);
        gtk_widget_set_sensitive (MainWindow, TRUE);
    }
    return FALSE;
}

// Mouse click callback
void LutDialogMouseClick (HWID widget, gpointer data){
    LookUpTableCheckMode ();
    LookUpTableGetData(NULL, (gpointer) data);
}

// Calls DestroyWindow
void LutCallDestroyWindow (HWID widget, gpointer data){
    DestroyWindow (LutDialog);
    gtk_widget_set_sensitive (MainWindow, TRUE);
}

//-----------------------------------------------------------------------------
// Show the look-up table dialog. This one is nasty, mostly because there are
// two ways to enter a look-up table: as a table, or as a string. Presumably
// I should convert between those two representations on the fly, as the user
// edit things, so I do.
//-----------------------------------------------------------------------------
void ShowLookUpTableDialog(ElemLeaf *l)
{
    ElemLookUpTable *t = &(l->d.lookUpTable);
    GdkEventKey* event;

    // First copy over all the stuff from the leaf structure; in particular,
    // we need our own local copy of the table entries, because it would be
    // bad to update those in the leaf before the user clicks okay (as he
    // might cancel).

    int count = t->count;
    cout << "Initially count is " << count << "\n";
    BOOL asString = t->editAsString;
    cout << "Initially asString is " << asString << "\n";
    memset(ValuesCache, 0, sizeof(ValuesCache));
    int i;
    for(i = 0; i < count; i++) {
            ValuesCache[i] = t->vals[i];
    }

    // Now create the dialog's fixed controls, plus the changing (depending
    // on show style/entry count) controls for the initial configuration.

    LutGrid = gtk_grid_new();
    // LutGrid[1] = gtk_grid_new();
    // gtk_grid_set_column_spacing (GTK_GRID (LutGrid), 1);
    LutPackingBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    LutDialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(LutDialog), "Look-Up Table");
    gtk_window_set_default_size(GTK_WINDOW(LutDialog), 100, 200);
    gtk_window_set_resizable (GTK_WINDOW (LutDialog), FALSE);
    gtk_box_pack_start(GTK_BOX(LutPackingBox), LutGrid, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(LutDialog), LutPackingBox);
    gtk_widget_add_events (LutDialog, GDK_KEY_PRESS_MASK);
    gtk_widget_add_events (LutDialog, GDK_BUTTON_PRESS_MASK);
    MakeFixedControls(FALSE);
    MakeLutControls(asString, count, FALSE);
  
    // Set up the controls to reflect the initial configuration.
    gtk_entry_set_text (GTK_ENTRY (DestTextbox), t->dest);
    gtk_entry_set_text (GTK_ENTRY (IndexTextbox), t->index);

    char buf[30];
    sprintf(buf, "%d", t->count);
    gtk_entry_set_text (GTK_ENTRY (CountTextbox), buf);
    if(asString) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (AsStringCheckbox), TRUE);
    }

    // And show the window
    gtk_widget_set_sensitive (MainWindow, FALSE);
    gtk_widget_show_all (LutDialog);
    gtk_widget_grab_focus(DestTextbox);
    gtk_widget_grab_focus(OkButton);

    temp.tmpcount = count;
    cout << "Count in ShowLookUp : " << count << "\n";
    temp.tmpasString = asString;
    cout << "asString in ShowLookUp : " << asString << "\n";

    g_signal_connect (G_OBJECT (LutDialog), "key-press-event",
                    G_CALLBACK(LookUpTableKeyPress), (gpointer)l);
    g_signal_connect (G_OBJECT (OkButton), "clicked",
                    G_CALLBACK(LutDialogMouseClick), (gpointer)l);
    g_signal_connect (G_OBJECT (CancelButton), "clicked",
                    G_CALLBACK(LutCallDestroyWindow), NULL);
}

//-----------------------------------------------------------------------------
// Show the piecewise linear table dialog. This one can only be edited in
// only a single format, which makes things easier than before.
// //-----------------------------------------------------------------------------
// void ShowPiecewiseLinearDialog(ElemLeaf *l)
// {
//     ElemPiecewiseLinear *t = &(l->d.piecewiseLinear);

//     // First copy over all the stuff from the leaf structure; in particular,
//     // we need our own local copy of the table entries, because it would be
//     // bad to update those in the leaf before the user clicks okay (as he
//     // might cancel).
//     int count = t->count;
//     memset(ValuesCache, 0, sizeof(ValuesCache));
//     int i;
//     for(i = 0; i < count*2; i++) {
//         ValuesCache[i] = t->vals[i];
//     }

//     // Now create the dialog's fixed controls, plus the changing (depending
//     // on show style/entry count) controls for the initial configuration.

//     LutDialog = gtk_dialog_new_with_buttons ("Piecewise Linear Table", GTK_WINDOW (MainWindow),
//                     GTK_DIALOG_MODAL, "Ok", GTK_RESPONSE_ACCEPT,
//                     "Cancel", GTK_RESPONSE_REJECT, NULL);
//     gtk_widget_add_events (LutDialog, GDK_KEY_PRESS_MASK);
//     gtk_widget_add_events (LutDialog, GDK_BUTTON_PRESS_MASK);
//     MakeFixedControls(TRUE);
//     MakeLutControls(FALSE, count*2, TRUE);
  
//     // Set up the controls to reflect the initial configuration.
//     gtk_entry_set_text (GTK_ENTRY (DestTextbox), t->dest);
//     gtk_entry_set_text (GTK_ENTRY (IndexTextbox), t->index);
//     char buf[30];
//     sprintf(buf, "%d", t->count);
//     gtk_entry_set_text (GTK_ENTRY (CountTextbox), buf);

//     // And show the window
//     gtk_widget_set_sensitive (MainWindow, FALSE);
//     gtk_widget_show_all (LutDialog);
//     gtk_widget_grab_focus (DestTextbox);
// //     SendMessage(DestTextbox, EM_SETSEL, 0, -1);

// //     MSG msg;
// //     DWORD ret;
// //     DialogDone = FALSE;
// //     DialogCancel = FALSE;
// //     while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
// //         if(msg.message == WM_KEYDOWN) {
// //             if(msg.wParam == VK_RETURN) {
// //                 DialogDone = TRUE;
// //                 break;
// //             } else if(msg.wParam == VK_ESCAPE) {
// //                 DialogDone = TRUE;
// //                 DialogCancel = TRUE;
// //                 break;
// //             }
// //         }

// //         if(!IsDialogMessage(LutDialog, &msg)) {
// //             TranslateMessage(&msg);
// //             DispatchMessage(&msg);
// //         }

//         // Watch the (user-editable) count field, and use that to
//         // determine how many textboxes to show.
//         char* buf;
//         buf = gtk_entry_get_text (GTK_ENTRY (CountTextbox));
// //         SendMessage(CountTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)buf);
//         if(atoi(buf) != count) {
//             count = atoi(buf);
//             if(count < 0 || count > 10) {
//                 count = 0;
//                 gtk_entry_set_text (GTK_ENTRY (CountTextbox), "");
//             }
//             DestroyLutControls();
//             MakeLutControls(FALSE, count*2, TRUE);
//         }
//     }

// //     if(!DialogCancel) {
// //         SendMessage(DestTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(t->dest));
// //         SendMessage(IndexTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(t->index));
// //         DestroyLutControls();
// //         // The call to DestroyLutControls updated ValuesCache, so just read
// //         // them out of there.
// //         int i;
// //         for(i = 0; i < count*2; i++) {
// //             t->vals[i] = ValuesCache[i];
// //         }
// //         t->count = count;
// //     }

// //     EnableWindow(MainWindow, TRUE);
// //     DestroyWindow(LutDialog);
// }
