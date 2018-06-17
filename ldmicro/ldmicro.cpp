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
// A ladder logic compiler for 8 bit micros: user draws a ladder diagram,
// with an appropriately constrained `schematic editor,' and then we can
// simulated it under Windows or generate PIC/AVR code that performs the
// requested operations. This files contains the program entry point, plus
// most of the UI logic relating to the main window.
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include "linuxUI.h"

#include <stdio.h>
#include <stdlib.h>
#include "ldmicro.h"
#include "freezeLD.h"
#include "mcutable.h"


HINSTANCE   Instance;
HWID        MainWindow;
HWID        DrawWindow;
HCRDC         Hdc;

// parameters used to capture the mouse when implementing our totally non-
// general splitter control
//static HHOOK       MouseHookHandle;
static int         MouseY;

// For the open/save dialog boxes
#define LDMICRO_PATTERN "LDmicro Ladder Logic Programs (*.ld)\0*.ld\0" \
                     "All files\0*\0\0"
char CurrentSaveFile[MAX_PATH];
static BOOL ProgramChangedNotSaved = FALSE;

#define HEX_PATTERN  "Intel Hex Files (*.hex)\0*.hex\0All files\0*\0\0"
#define C_PATTERN "C Source Files (*.c)\0*.c\0All Files\0*\0\0"
#define INTERPRETED_PATTERN \
    "Interpretable Byte Code Files (*.int)\0*.int\0All Files\0*\0\0"
char CurrentCompileFile[MAX_PATH];

#define TXT_PATTERN  "Text Files (*.txt)\0*.txt\0All files\0*\0\0"

// Everything relating to the PLC's program, I/O configuration, processor
// choice, and so on--basically everything that would be saved in the
// project file.
PlcProgram Prog;

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then save the program to that
// file and then set our default filename to that.
//-----------------------------------------------------------------------------
// static BOOL SaveAsDialog(void)
// {
//     OPENFILENAME ofn;

//     memset(&ofn, 0, sizeof(ofn));
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hInstance = Instance;
//     ofn.lpstrFilter = LDMICRO_PATTERN;
//     ofn.lpstrDefExt = "ld";
//     ofn.lpstrFile = CurrentSaveFile;
//     ofn.nMaxFile = sizeof(CurrentSaveFile);
//     ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

//     if(!GetSaveFileName(&ofn))
//         return FALSE;

//     if(!SaveProjectToFile(CurrentSaveFile)) {
//         Error(_("Couldn't write to '%s'."), CurrentSaveFile);
//         return FALSE;
//     } else {
//         ProgramChangedNotSaved = FALSE;
//         return TRUE;
//     }
// }

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then export the program as
// an ASCII art drawing.
//-----------------------------------------------------------------------------
// static void ExportDialog(void)
// {
//     char exportFile[MAX_PATH];
//     OPENFILENAME ofn;

//     exportFile[0] = '\0';

//     memset(&ofn, 0, sizeof(ofn));
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hInstance = Instance;
//     ofn.lpstrFilter = TXT_PATTERN;
//     ofn.lpstrFile = exportFile;
//     ofn.lpstrTitle = _("Export As Text");
//     ofn.nMaxFile = sizeof(exportFile);
//     ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

//     if(!GetSaveFileName(&ofn))
//         return;

//     ExportDrawingAsText(exportFile);
// }

//-----------------------------------------------------------------------------
// If we already have a filename, save the program to that. Otherwise same
// as Save As. Returns TRUE if it worked, else returns FALSE.
//-----------------------------------------------------------------------------
// static BOOL SaveProgram(void)
// {
//     if(strlen(CurrentSaveFile)) {
//         if(!SaveProjectToFile(CurrentSaveFile)) {
//             Error(_("Couldn't write to '%s'."), CurrentSaveFile);
//             return FALSE;
//         } else {
//             ProgramChangedNotSaved = FALSE;
//             return TRUE;
//         }
//     } else {
//         return SaveAsDialog();
//     }
// }

//-----------------------------------------------------------------------------
// Compile the program to a hex file for the target micro. Get the output
// file name if necessary, then call the micro-specific compile routines.
//-----------------------------------------------------------------------------
static void CompileProgram(BOOL compileAs)
{
    if(compileAs || strlen(CurrentCompileFile)==0) {
        OPENFILENAME ofn;

        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.parentWindow = NULL;
        ofn.lpstrTitle = _("Compile To");
        if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
            ofn.lpstrFilter = C_PATTERN;
            ofn.lpstrDefExt = "c";
        } else if(Prog.mcu && Prog.mcu->whichIsa == ISA_INTERPRETED) {
            ofn.lpstrFilter = INTERPRETED_PATTERN;
            ofn.lpstrDefExt = "int";
        } else {
            ofn.lpstrFilter = HEX_PATTERN;
            ofn.lpstrDefExt = "hex";
        }
        ofn.lpstrFile = CurrentCompileFile;
        ofn.nMaxFile = sizeof(CurrentCompileFile);
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

        if(!GetSaveFileName(&ofn))
            return;

        // hex output filename is stored in the .ld file
        ProgramChangedNotSaved = TRUE;
    }

    if(!GenerateIntermediateCode()) return;

    if(Prog.mcu == NULL) {
        Error(_("Must choose a target microcontroller before compiling."));
        return;
    } 

    if(UartFunctionUsed() && Prog.mcu->uartNeeds.rxPin == 0) {
        Error(_("UART function used but not supported for this micro."));
        return;
    }
    
    if(PwmFunctionUsed() && Prog.mcu->pwmNeedsPin == 0) {
        Error(_("PWM function used but not supported for this micro."));
        return;
    }
  
    switch(Prog.mcu->whichIsa) {
        case ISA_AVR:           CompileAvr(CurrentCompileFile); break;
        case ISA_PIC16:         CompilePic16(CurrentCompileFile); break;
        case ISA_ANSIC:         CompileAnsiC(CurrentCompileFile); break;
        case ISA_INTERPRETED:   CompileInterpreted(CurrentCompileFile); break;
        case ISA_ARDUINO:   CompileArduino(CurrentCompileFile); break;

        default: oops();
    }
   IntDumpListing("t.pl");
}

//-----------------------------------------------------------------------------
// If the program has been modified then give the user the option to save it
// or to cancel the operation they are performing. Return TRUE if they want
// to cancel.
//-----------------------------------------------------------------------------
// BOOL CheckSaveUserCancels(void)
// {
//     if(!ProgramChangedNotSaved) {
//         // no problem
//         return FALSE;
//     }

//     int r = MessageBox(MainWindow, 
//         _("The program has changed since it was last saved.\r\n\r\n"
//         "Do you want to save the changes?"), "LDmicro",
//         MB_YESNOCANCEL | MB_ICONWARNING);
//     switch(r) {
//         case IDYES:
//             if(SaveProgram())
//                 return FALSE;
//             else
//                 return TRUE;

//         case IDNO:
//             return FALSE;

//         case IDCANCEL:
//             return TRUE;

//         default:
//             oops();
//     }
// }

//-----------------------------------------------------------------------------
// Load a new program from a file. If it succeeds then set our default filename
// to that, else we end up with an empty file then.
//-----------------------------------------------------------------------------
// static void OpenDialog(void)
// {
//     OPENFILENAME ofn;

//     char tempSaveFile[MAX_PATH] = "";

//     memset(&ofn, 0, sizeof(ofn));
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hInstance = Instance;
//     ofn.lpstrFilter = LDMICRO_PATTERN;
//     ofn.lpstrDefExt = "ld";
//     ofn.lpstrFile = tempSaveFile;
//     ofn.nMaxFile = sizeof(tempSaveFile);
//     ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

//     if(!GetOpenFileName(&ofn))
//         return;

//     if(!LoadProjectFromFile(tempSaveFile)) {
//         Error(_("Couldn't open '%s'."), tempSaveFile);
//         CurrentSaveFile[0] = '\0';
//     } else {
//         ProgramChangedNotSaved = FALSE;
//         strcpy(CurrentSaveFile, tempSaveFile);
//         UndoFlush();
//     }

//     GenerateIoListDontLoseSelection();
//     RefreshScrollbars();
//     UpdateMainWindowTitleBar();
// }

//-----------------------------------------------------------------------------
// Housekeeping required when the program changes: mark the program as
// changed so that we ask if user wants to save before exiting, and update
// the I/O list.
//-----------------------------------------------------------------------------
// void ProgramChanged(void)
// {
//     ProgramChangedNotSaved = TRUE;
//     GenerateIoListDontLoseSelection();
//     RefreshScrollbars();
// }
// #define CHANGING_PROGRAM(x) { \
//         UndoRemember(); \
//         x; \
//         ProgramChanged(); \
//     }

//-----------------------------------------------------------------------------
// Hook that we install when the user starts dragging the `splitter,' in case
// they drag it out of the narrow area of the drawn splitter bar. Resize
// the listview in response to mouse move, and unhook ourselves when they
// release the mouse button.
//-----------------------------------------------------------------------------
// static LRESULT CALLBACK MouseHook(int code, WPARAM wParam, LPARAM lParam)
// {
//     switch(code) {
//         case HC_ACTION: {
//             MSLLHOOKSTRUCT *mhs = (MSLLHOOKSTRUCT *)lParam;

//             switch(wParam) {
//                 case WM_MOUSEMOVE: {
//                     int dy = MouseY - mhs->pt.y;
                   
//                     IoListHeight += dy;
//                     if(IoListHeight < 50) IoListHeight = 50;
//                     MouseY = mhs->pt.y;
//                     MainWindowResized();

//                     break;
//                 }

//                 case WM_LBUTTONUP:
//                     UnhookWindowsHookEx(MouseHookHandle);
//                     break;
//             }
//             break;
//         }
//     }
//     return CallNextHookEx(MouseHookHandle, code, wParam, lParam);
// }

//-----------------------------------------------------------------------------
// Handle a selection from the menu bar of the main window.
//-----------------------------------------------------------------------------
// static void ProcessMenu(int code)
// {
//     if(code >= MNU_PROCESSOR_0 && code < MNU_PROCESSOR_0+NUM_SUPPORTED_MCUS) {
//         strcpy(CurrentCompileFile, "");
//         Prog.mcu = &SupportedMcus[code - MNU_PROCESSOR_0];
//         RefreshControlsToSettings();
//         return;
//     }
//     if(code == MNU_PROCESSOR_0+NUM_SUPPORTED_MCUS) {
//         Prog.mcu = NULL;
//         strcpy(CurrentCompileFile, "");
//         RefreshControlsToSettings();
//         return;
//     }

//     switch(code) {
//         case MNU_NEW:
//             if(CheckSaveUserCancels()) break;
//             NewProgram();
//             strcpy(CurrentSaveFile, "");
//             strcpy(CurrentCompileFile, "");
//             GenerateIoListDontLoseSelection();
//             RefreshScrollbars();
//             UpdateMainWindowTitleBar();
//             break;

//         case MNU_OPEN:
//             if(CheckSaveUserCancels()) break;
//             OpenDialog();
//             break;

//         case MNU_SAVE:
//             SaveProgram();
//             UpdateMainWindowTitleBar();
//             break;

//         case MNU_SAVE_AS:
//             SaveAsDialog();
//             UpdateMainWindowTitleBar();
//             break;

//         case MNU_EXPORT:
//             ExportDialog();
//             break;

//         case MNU_EXIT:
//             if(CheckSaveUserCancels()) break;
//             PostQuitMessage(0);
//             break;

//         case MNU_INSERT_COMMENT:
//             CHANGING_PROGRAM(AddComment(_("--add comment here--")));
//             break;

//         case MNU_INSERT_CONTACTS:
//             CHANGING_PROGRAM(AddContact());
//             break;

//         case MNU_INSERT_COIL:
//             CHANGING_PROGRAM(AddCoil());
//             break;

//         case MNU_INSERT_TON:
//             CHANGING_PROGRAM(AddTimer(ELEM_TON));
//             break;

//         case MNU_INSERT_TOF:
//             CHANGING_PROGRAM(AddTimer(ELEM_TOF));
//             break;

//         case MNU_INSERT_RTO:
//             CHANGING_PROGRAM(AddTimer(ELEM_RTO));
//             break;

//         case MNU_INSERT_CTU:
//             CHANGING_PROGRAM(AddCounter(ELEM_CTU));
//             break;

//         case MNU_INSERT_CTD:
//             CHANGING_PROGRAM(AddCounter(ELEM_CTD));
//             break;

//         case MNU_INSERT_CTC:
//             CHANGING_PROGRAM(AddCounter(ELEM_CTC));
//             break;

//         case MNU_INSERT_RES:
//             CHANGING_PROGRAM(AddReset());
//             break;

//         case MNU_INSERT_OPEN:
//             CHANGING_PROGRAM(AddEmpty(ELEM_OPEN));
//             break;

//         case MNU_INSERT_SHORT:
//             CHANGING_PROGRAM(AddEmpty(ELEM_SHORT));
//             break;

//         case MNU_INSERT_MASTER_RLY:
//             CHANGING_PROGRAM(AddMasterRelay());
//             break;

//         case MNU_INSERT_SHIFT_REG:
//             CHANGING_PROGRAM(AddShiftRegister());
//             break;

//         case MNU_INSERT_LUT:
//             CHANGING_PROGRAM(AddLookUpTable());
//             break;
        
//         case MNU_INSERT_PWL:
//             CHANGING_PROGRAM(AddPiecewiseLinear());
//             break;
        
//         case MNU_INSERT_FMTD_STR:
//             CHANGING_PROGRAM(AddFormattedString());
//             break;

//         case MNU_INSERT_OSR:
//             CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_RISING));
//             break;

//         case MNU_INSERT_OSF:
//             CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_FALLING));
//             break;

//         case MNU_INSERT_MOV:
//             CHANGING_PROGRAM(AddMove());
//             break;

//         case MNU_INSERT_SET_PWM:
//             CHANGING_PROGRAM(AddSetPwm());
//             break;

//         case MNU_INSERT_READ_ADC:
//             CHANGING_PROGRAM(AddReadAdc());
//             break;

//         case MNU_INSERT_UART_SEND:
//             CHANGING_PROGRAM(AddUart(ELEM_UART_SEND));
//             break;

//         case MNU_INSERT_UART_RECV:
//             CHANGING_PROGRAM(AddUart(ELEM_UART_RECV));
//             break;

//         case MNU_INSERT_PERSIST:
//             CHANGING_PROGRAM(AddPersist());
//             break;

//         {
//             int elem;
//             case MNU_INSERT_ADD: elem = ELEM_ADD; goto math;
//             case MNU_INSERT_SUB: elem = ELEM_SUB; goto math;
//             case MNU_INSERT_MUL: elem = ELEM_MUL; goto math;
//             case MNU_INSERT_DIV: elem = ELEM_DIV; goto math;
// math:
//                 CHANGING_PROGRAM(AddMath(elem));
//                 break;
//         }

//         {
//             int elem;
//             case MNU_INSERT_EQU: elem = ELEM_EQU; goto cmp;
//             case MNU_INSERT_NEQ: elem = ELEM_NEQ; goto cmp;
//             case MNU_INSERT_GRT: elem = ELEM_GRT; goto cmp;
//             case MNU_INSERT_GEQ: elem = ELEM_GEQ; goto cmp;
//             case MNU_INSERT_LES: elem = ELEM_LES; goto cmp;
//             case MNU_INSERT_LEQ: elem = ELEM_LEQ; goto cmp;
// cmp:    
//                 CHANGING_PROGRAM(AddCmp(elem));
//                 break;
//         } 

//         case MNU_MAKE_NORMAL:
//             CHANGING_PROGRAM(MakeNormalSelected());
//             break;

//         case MNU_NEGATE:
//             CHANGING_PROGRAM(NegateSelected());
//             break;

//         case MNU_MAKE_SET_ONLY:
//             CHANGING_PROGRAM(MakeSetOnlySelected());
//             break;

//         case MNU_MAKE_RESET_ONLY:
//             CHANGING_PROGRAM(MakeResetOnlySelected());
//             break;

//         case MNU_UNDO:
//             UndoUndo();
//             break;

//         case MNU_REDO:
//             UndoRedo();
//             break;

//         case MNU_INSERT_RUNG_BEFORE:
//             CHANGING_PROGRAM(InsertRung(FALSE));
//             break;

//         case MNU_INSERT_RUNG_AFTER:
//             CHANGING_PROGRAM(InsertRung(TRUE));
//             break;

//         case MNU_DELETE_RUNG:
//             CHANGING_PROGRAM(DeleteSelectedRung());
//             break;

//         case MNU_PUSH_RUNG_UP:
//             CHANGING_PROGRAM(PushRungUp());
//             break;

//         case MNU_PUSH_RUNG_DOWN:
//             CHANGING_PROGRAM(PushRungDown());
//             break;

//         case MNU_DELETE_ELEMENT:
//             CHANGING_PROGRAM(DeleteSelectedFromProgram());
//             break;

//         case MNU_MCU_SETTINGS:
//             CHANGING_PROGRAM(ShowConfDialog());
//             break;

//         case MNU_SIMULATION_MODE:
//             ToggleSimulationMode();
//             break;

//         case MNU_START_SIMULATION:
//             StartSimulation();
//             break;

//         case MNU_STOP_SIMULATION:
//             StopSimulation();
//             break;

//         case MNU_SINGLE_CYCLE:
//             SimulateOneCycle(TRUE);
//             break;

//         case MNU_COMPILE:
//             CompileProgram(FALSE);
//             break;

//         case MNU_COMPILE_AS:
//             CompileProgram(TRUE);
//             break;

//         case MNU_MANUAL:
//             ShowHelpDialog(FALSE);
//             break;

//         case MNU_ABOUT:
//             ShowHelpDialog(TRUE);
//             break;
//     }
// }

//-----------------------------------------------------------------------------
// WndProc for MainWindow.
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // switch (msg) {
    //     case WM_ERASEBKGND:
    //         break;

    //     case WM_SETFOCUS:

    //     case WM_PAINT: {
    //     }

    //     case WM_KEYDOWN: {
    //     }

    //     case WM_LBUTTONDBLCLK: {
    //     }

    //     case WM_LBUTTONDOWN: {
    //     }
    //     case WM_MOUSEMOVE: {
    //     }
    //     case WM_MOUSEWHEEL: {
    //     }

    //     case WM_SIZE:

    //     case WM_NOTIFY: {
    //         NMHDR *h = (NMHDR *)lParam;
    //         if(h->hwndFrom == IoList) {
    //             IoListProc(h);
    //         }
    //         return 0;
    //     }
    //     case WM_VSCROLL:

    //     case WM_HSCROLL:

    //     case WM_COMMAND:
    //         ProcessMenu(LOWORD(wParam));
    //         InvalidateRect(MainWindow, NULL, FALSE);
    //         break;

    //     case WM_CLOSE:
    //     case WM_DESTROY:

    //     default:
    //         return DefWindowProc(hwnd, msg, wParam, lParam);
    // }

    return 1;
}

void LD_WM_Close_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)//(HWND window)
{
    /* Handles:
    * WM_CLOSE
    */

    FreezeWindowPos(MainWindow);
    FreezeDWORD(IoListHeight);

    gtk_main_quit();
}

gboolean LD_WM_KeyDown_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{   
    /* Handles:
    * WM_KEYDOWN
    */

    switch(event->key.state)
    {
        case GDK_SHIFT_MASK:
            g_print("SHIFT+");
            break;
        case GDK_CONTROL_MASK:
            g_print("CONTROL+");
            break;
    }
  
    g_print("%c\n", (char)gdk_keyval_to_unicode(event->key.keyval));

    // if(wParam == 'M') {
    //         if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //             ToggleSimulationMode();
    //             break;
    //         }
    //     } else if(wParam == VK_TAB) {
    //         SetFocus(IoList);
    //         BlinkCursor(0, 0, 0, 0);
    //         break;
    //     } else if(wParam == VK_F1) {
    //         ShowHelpDialog(FALSE);
    //         break;
    //     }

    //     if(InSimulationMode) {
    //         switch(wParam) {
    //             case ' ':
    //                 SimulateOneCycle(TRUE);
    //                 break;

    //             case 'R':
    //                 if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
    //                     StartSimulation();
    //                 break;

    //             case 'H':
    //                 if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
    //                     StopSimulation();
    //                 break;

    //             case VK_DOWN:
    //                 if(ScrollYOffset < ScrollYOffsetMax)
    //                     ScrollYOffset++;
    //                 RefreshScrollbars();
    //                 InvalidateRect(MainWindow, NULL, FALSE);
    //                 break;

    //             case VK_UP:
    //                 if(ScrollYOffset > 0)
    //                     ScrollYOffset--;
    //                 RefreshScrollbars();
    //                 InvalidateRect(MainWindow, NULL, FALSE);
    //                 break;

    //             case VK_LEFT:
    //                 ScrollXOffset -= FONT_WIDTH;
    //                 if(ScrollXOffset < 0) ScrollXOffset = 0;
    //                 RefreshScrollbars();
    //                 InvalidateRect(MainWindow, NULL, FALSE);
    //                 break;

    //             case VK_RIGHT:
    //                 ScrollXOffset += FONT_WIDTH;
    //                 if(ScrollXOffset >= ScrollXOffsetMax)
    //                     ScrollXOffset = ScrollXOffsetMax;
    //                 RefreshScrollbars();
    //                 InvalidateRect(MainWindow, NULL, FALSE);
    //                 break;

    //             case VK_RETURN:
    //             case VK_ESCAPE:
    //                 ToggleSimulationMode();
    //                 break;
    //         }
    //         break;
    //     }


    //     switch(wParam) {
    //         case VK_F5:
    //             CompileProgram(FALSE);
    //             break;

    //         case VK_UP:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(PushRungUp());
    //             } else {
    //                 MoveCursorKeyboard(wParam);
    //             }
    //             break;

    //         case VK_DOWN:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(PushRungDown());
    //             } else {
    //                 MoveCursorKeyboard(wParam);
    //             }
    //             break;

    //         case VK_RIGHT:
    //         case VK_LEFT:
    //             MoveCursorKeyboard(wParam);
    //             break;

    //         case VK_RETURN:
    //             CHANGING_PROGRAM(EditSelectedElement());
    //             break;

    //         case VK_DELETE:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(DeleteSelectedRung());
    //             } else {
    //                 CHANGING_PROGRAM(DeleteSelectedFromProgram());
    //             }
    //             break;

    //         case VK_OEM_1:
    //             CHANGING_PROGRAM(AddComment(_("--add comment here--")));
    //             break;

    //         case 'C':
    //             CHANGING_PROGRAM(AddContact());
    //             break;

    //         // TODO: rather country-specific here
    //         case VK_OEM_2:
    //             CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_RISING));
    //             break;

    //         case VK_OEM_5:
    //             CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_FALLING));
    //             break;

    //         case 'L':
    //             CHANGING_PROGRAM(AddCoil());
    //             break;

    //         case 'R':
    //             CHANGING_PROGRAM(MakeResetOnlySelected());
    //             break;

    //         case 'E':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 ExportDialog();
    //             } else {
    //                 CHANGING_PROGRAM(AddReset());
    //             }
    //             break;

    //         case 'S':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 SaveProgram();
    //                 UpdateMainWindowTitleBar();
    //             } else {
    //                 CHANGING_PROGRAM(MakeSetOnlySelected());
    //             }
    //             break;

    //         case 'N':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 if(CheckSaveUserCancels()) break;
    //                 if(!ProgramChangedNotSaved) {
    //                     int r = MessageBox(MainWindow, 
    //                         _("Start new program?"),
    //                         "LDmicro", MB_YESNO | MB_DEFBUTTON2 |
    //                         MB_ICONQUESTION);
    //                     if(r == IDNO) break;
    //                 }
    //                 NewProgram();
    //                 strcpy(CurrentSaveFile, "");
    //                 strcpy(CurrentCompileFile, "");
    //                 GenerateIoListDontLoseSelection();
    //                 RefreshScrollbars();
    //                 UpdateMainWindowTitleBar();
    //             } else {
    //                 CHANGING_PROGRAM(NegateSelected());
    //             }
    //             break;

    //         case 'A':
    //             CHANGING_PROGRAM(MakeNormalSelected());
    //             break;

    //         case 'T':
    //             CHANGING_PROGRAM(AddTimer(ELEM_RTO));
    //             break;

    //         case 'O':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 if(CheckSaveUserCancels()) break;
    //                 OpenDialog();
    //             } else {
    //                 CHANGING_PROGRAM(AddTimer(ELEM_TON));
    //             }
    //             break;

    //         case 'F':
    //             CHANGING_PROGRAM(AddTimer(ELEM_TOF));
    //             break;

    //         case 'U':
    //             CHANGING_PROGRAM(AddCounter(ELEM_CTU));
    //             break;

    //         case 'I':
    //             CHANGING_PROGRAM(AddCounter(ELEM_CTD));
    //             break;

    //         case 'J':
    //             CHANGING_PROGRAM(AddCounter(ELEM_CTC));
    //             break;

    //         case 'M':
    //             CHANGING_PROGRAM(AddMove());
    //             break;

    //         case 'P':
    //             CHANGING_PROGRAM(AddReadAdc());
    //             break;

    //         case VK_OEM_PLUS:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(AddMath(ELEM_ADD));
    //             } else {
    //                 CHANGING_PROGRAM(AddCmp(ELEM_EQU));
    //             }
    //             break;

    //         case VK_OEM_MINUS:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //             } else {
    //                 CHANGING_PROGRAM(AddMath(ELEM_SUB));
    //             }
    //             break;

    //         case '8':
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(AddMath(ELEM_MUL));
    //             }
    //             break;

    //         case 'D':
    //             CHANGING_PROGRAM(AddMath(ELEM_DIV));
    //             break;

    //         case VK_OEM_PERIOD:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(AddCmp(ELEM_GRT));
    //             } else {
    //                 CHANGING_PROGRAM(AddCmp(ELEM_GEQ));
    //             }
    //             break;

    //         case VK_OEM_COMMA:
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(AddCmp(ELEM_LES));
    //             } else {
    //                 CHANGING_PROGRAM(AddCmp(ELEM_LEQ));
    //             }
    //             break;

    //         case 'V':
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(InsertRung(TRUE));
    //             }
    //             break;

    //         case '6':
    //             if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    //                 CHANGING_PROGRAM(InsertRung(FALSE));
    //             }
    //             break;

    //         case 'Z':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 UndoUndo();
    //             }
    //             break;

    //         case 'Y':
    //             if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //                 UndoRedo();
    //             }
    //             break;

    //         default:
    //             break;
    //     }
    //     if(wParam != VK_SHIFT && wParam != VK_CONTROL) {
    //         InvalidateRect(MainWindow, NULL, FALSE);
    //     }
    //     break;
    return FALSE;
}

gboolean LD_GTK_mouse_click_hook(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_LBUTTONDBLCLK, WM_LBUTTONDOWN
    */

    g_print("x = %f\n", event->button.x_root);
    g_print("y = %f\n", event->button.y_root);
    switch(event->button.type)
    {
        case GDK_BUTTON_PRESS:// To Do: run only for left click
            // int x = LOWORD(lParam);
            // int y = HIWORD(lParam);
            // if((y > (IoListTop - 9)) && (y < (IoListTop + 3))) {
            //     POINT pt;
            //     pt.x = x; pt.y = y;
            //     ClientToScreen(MainWindow, &pt);
            //     MouseY = pt.y;
            //     MouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL,
            //             (HOOKPROC)MouseHook, Instance, 0);
            // }
            // if(!InSimulationMode) MoveCursorMouseClick(x, y);

            // SetFocus(MainWindow);
            // InvalidateRect(MainWindow, NULL, FALSE);
            break;
        case GDK_2BUTTON_PRESS:
            // int x = LOWORD(lParam);
            // int y = HIWORD(lParam);
            // if(InSimulationMode) {
            //     EditElementMouseDoubleclick(x, y);
            // } else {
            //     CHANGING_PROGRAM(EditElementMouseDoubleclick(x, y));
            // }
            // InvalidateRect(MainWindow, NULL, FALSE);
            break;

    }
    return FALSE;
}

gboolean LD_GTK_mouse_scroll_hook(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_VSCROLL, WM_HSCROLL, WM_MOUSEWHEEL
    */

    switch(event->scroll.direction)
    {
        case GDK_SCROLL_UP:
        case GDK_SCROLL_DOWN:
            // VscrollProc(wParam);
            break;
        case GDK_SCROLL_LEFT:
        case GDK_SCROLL_RIGHT:
            // HscrollProc(wParam);
            break;
        case GDK_SCROLL_SMOOTH:
            // if((GET_WHEEL_DELTA_WPARAM(wParam)) > 0) {
            //     VscrollProc(SB_LINEUP);
            // } else {
            //     VscrollProc(SB_LINEDOWN);
            // }
            // gdk_event_get_scroll_deltas (const GdkEvent *event, gdouble *delta_x, gdouble *delta_y);
            break;

    }
    return FALSE;
}

gboolean LD_WM_MouseMove_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_MOUSEMOVE
    */

    g_print("x = %f\n", event->button.x_root);
    g_print("y = %f\n", event->button.y_root);
    // int x = LOWORD(lParam);
    // int y = HIWORD(lParam);

    // if((y > (IoListTop - 9)) && (y < (IoListTop + 3))) {
    //     SetCursor(LoadCursor(NULL, IDC_SIZENS));
    // } else {
    //     SetCursor(LoadCursor(NULL, IDC_ARROW));
    // }
    
    // break;
    return FALSE;
}

gboolean LD_WM_Paint_call(HWID widget, HCRDC cr, gpointer data)
{
    /* Handles:
    * WM_PAINT
    */

    g_print("draw called\n");

    // guint width, height;
    // GdkRGBA color;
    // GtkStyleContext *context;

    // context = gtk_widget_get_style_context (widget);

    // width = gtk_widget_get_allocated_width (widget);
    // height = gtk_widget_get_allocated_height (widget);

    // gtk_render_background (context, cr, 0, 0, width, height);

    // cairo_arc (cr,
    //             width / 2.0, height / 2.0,
    //             MIN (width, height) / 3.0,
    //             0, 2 * G_PI);

    // gtk_style_context_get_color (context,
    //                             gtk_style_context_get_state (context),
    //                             &color);
    // gdk_cairo_set_source_rgba (cr, &color);

    // cairo_fill (cr);
    // static double Cairo_R = 0.0, Cairo_G = 0.0, Cairo_B = 0.0;
    // cairo_set_source_rgb(cr, Cairo_R, Cairo_G, Cairo_G); 
    // Cairo_R = (Cairo_R+0.2 > 0.4) ? 0 : Cairo_R+0.2;
    // Cairo_G = (Cairo_G+0.4 > 1.0) ? 0.4 : Cairo_G+0.4;
    // Cairo_B = (Cairo_B+0.1 > 0.5) ? 0 : Cairo_B+0.1;
    
    // cairo_select_font_face(cr, "Purisa",
    //     CAIRO_FONT_SLANT_NORMAL,
    //     CAIRO_FONT_WEIGHT_BOLD);

    // cairo_set_font_size(cr, 20);

    // cairo_move_to(cr, 20, height / 2.0);
    // cairo_show_text(cr, "-----------THIS IS A TEST DRAW----------");  

    // cairo_fill (cr);

    
    // PAINTSTRUCT ps;
    Hdc = cr;//BeginPaint(hwnd, &ps);

    /// This draws the schematic.
    PaintWindow();

    // RECT r;
    // // Fill around the scroll bars
    // if(NeedHoriz) {
    //     r.top = IoListTop - ScrollHeight - 2;
    //     r.bottom = IoListTop - 2;
    //     FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    // }
    // GetClientRect(MainWindow, &r);
    // r.left = r.right - ScrollWidth - 2;
    // FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

    // // Draw the splitter thing to grab to resize the I/O listview.
    // GetClientRect(MainWindow, &r);
    // r.top = IoListTop - 2;
    // r.bottom = IoListTop;
    // FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    // r.top = IoListTop - 2;
    // r.bottom = IoListTop - 1;
    // FillRect(Hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
    // r.top = IoListTop;
    // r.bottom = IoListTop + 1;
    // FillRect(Hdc, &r, (HBRUSH)GetStockObject(DKGRAY_BRUSH));

    // EndPaint(hwnd, &ps);

    return FALSE;
}

gboolean LD_WM_Destroy_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_DESTROY
    */

    // if(CheckSaveUserCancels()) break;

    // PostQuitMessage(0);
    // return 1;

    return FALSE;
}

gboolean LD_WM_Size_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_SIZE
    */

    // MainWindowResized();
    // break;

    return FALSE;
}

gboolean LD_WM_SetFocus_call(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    /* Handles:
    * WM_SETFOCUS
    */

    // InvalidateRect(MainWindow, NULL, FALSE);
    // break;

    return FALSE;
}

//-----------------------------------------------------------------------------
// Entry point into the program.
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    /// Check if we're running in non-interactive mode; in that case we should
    /// load the file, compile, and exit.

    if(argc >= 2 && !(argc < 4) ) {
        RunningInBatchMode = TRUE;

        char *err =
            "Bad command line arguments: run 'ldmicro /c src.ld dest.hex'";

        // if (argc < 4)
        // {
        //     Error(err); 
        //     exit(-1);
        // }

        char *source = (char*)malloc(strlen(argv[2]) + strlen(argv[3]) + 2);
        sprintf(source, "%s %s", argv[2], argv[3]);

        while(isspace(*source)) {
            source++;
        }
        if(*source == '\0') 
        { 
            Error(err); 
            free(source);
            exit(-1); 
        }

        char *dest = source;
        while(!isspace(*dest) && *dest) {
            dest++;
        }
        if(*dest == '\0') 
        { 
            Error(err); 
            free(source);
            exit(-1); 
        }
        *dest = '\0'; dest++;
        while(isspace(*dest)) {
            dest++;
        }

        if(*dest == '\0') 
        { 
            Error(err); 
            free(source);
            exit(-1); 
        }

        if(!LoadProjectFromFile(source)) {
            Error("Couldn't open '%s', running non-interactively.\n", source);
            free(source);
            exit(-1);
        }
        strcpy(CurrentCompileFile, dest);
        GenerateIoList(-1);
        CompileProgram(FALSE);
        exit(0);
    }
    
    gtk_init(&argc, &argv);
    Instance = NULL;
    /* TEST
    MainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(MainWindow), "LDMicro");
    g_signal_connect (MainWindow, "delete_event", G_CALLBACK (LDMicro_close), NULL);
    gtk_window_set_default_size (GTK_WINDOW (MainWindow), 600, 400);
    gtk_window_resize (GTK_WINDOW (MainWindow), 600, 400);
    
    ThawWindowPos(MainWindow);
    ThawDWORD(IoListHeight);
    
    
    // Title bar
    UpdateMainWindowTitleBar();

    // Splitting the window
    MakeMainWindowControls();

    // Calling the Simulation functions
    
    // StartSimulation(); // test
    // SetMenusEnabled(true, true, false,
    // true, false, false, false,
    // true, true, true);  // test
    // ToggleSimulationMode(); //test
    // GenerateIoListDontLoseSelection();
    StopSimulation(); //Test
    */
    
    MainHeap = HeapCreate(0, 1024*64, 0);

    // MakeDialogBoxClass();
    // MakeComponentListClass();
    // MakeSmplDialogClass();
    // MakeNamingListClass();
    HMENU top = MakeMainWindowMenus();

    /// Make main window
    MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(MainWindow),  "LDmicro");
    gtk_window_set_default_size (GTK_WINDOW(MainWindow), 800, 600);
    gtk_window_resize (GTK_WINDOW(MainWindow), 800, 600);
    gtk_window_move(GTK_WINDOW(MainWindow), 10, 10);
    gtk_widget_override_background_color(GTK_WIDGET(MainWindow), 
                            GTK_STATE_FLAG_NORMAL, ((HBRUSH)GetStockObject(GREY_BRUSH))->getThis());
    gtk_window_set_default_icon(LoadImage(Instance, LDMICRO_ICON,
                            IMAGE_ICON, 32, 32, 0));
    gtk_window_set_icon(GTK_WINDOW(MainWindow), LoadImage(Instance, LDMICRO_ICON,
                            IMAGE_ICON, 32, 32, 0));
    /// Make main window - end

    InitForDrawing();

    ThawWindowPos(MainWindow);
    IoListHeight = 100;
    ThawDWORD(IoListHeight);

    MakeMainWindowControls(); /// takes care of MakeMainWindowMenus()
    MainWindowResized();

    /// Keyboard and mouse hooks equivalent to MainWndProc
    g_signal_connect (MainWindow, "delete_event", G_CALLBACK (LD_WM_Close_call), NULL);
    g_signal_connect (MainWindow, "key_press_event", G_CALLBACK (LD_WM_KeyDown_call), NULL);
    g_signal_connect (MainWindow, "button_press_event", G_CALLBACK (LD_GTK_mouse_click_hook), NULL);
    g_signal_connect (MainWindow, "scroll_event", G_CALLBACK (LD_GTK_mouse_scroll_hook), NULL);
    g_signal_connect (MainWindow, "motion_notify_event", G_CALLBACK (LD_WM_MouseMove_call), NULL);
    g_signal_connect (DrawWindow, "draw", G_CALLBACK (LD_WM_Paint_call), NULL);
    g_signal_connect (MainWindow, "destroy_event", G_CALLBACK (LD_WM_Destroy_call), NULL);
    g_signal_connect (MainWindow, "configure_event", G_CALLBACK (LD_WM_Size_call), NULL);
    g_signal_connect (MainWindow, "focus_in_event", G_CALLBACK (LD_WM_SetFocus_call), NULL);
    /// Keyboard and mouse hooks equivalent to MainWndProc - end

    NewProgram();
    strcpy(CurrentSaveFile, "");

    // We are running interactively, or we would already have exited. We
    // can therefore show the window now, and otherwise set up the GUI.

    // Displaying the window
    gtk_widget_show_all(MainWindow);
    // SetTimer(MainWindow, TIMER_BLINK_CURSOR, 800, BlinkCursor);
    
    if(argc >= 2) {
        // g_print("load prog: %s\n", argv[1]);
        char line[MAX_PATH];
        if(*argv[1] == '"') { 
            strcpy(line, argv[1]+1);
        } else {
            strcpy(line, argv[1]);
        }
        if(strchr(line, '"')) *strchr(line, '"') = '\0';
        
        realpath(line, CurrentSaveFile);
        // g_print("resolved path: %s\n", CurrentSaveFile);
        if(!LoadProjectFromFile(CurrentSaveFile)) {
            NewProgram();
            Error(_("Couldn't open '%s'."), CurrentSaveFile);
            CurrentSaveFile[0] = '\0';
        }
        UndoFlush();
    }

    GenerateIoListDontLoseSelection(); //~
    // RefreshScrollbars();
    UpdateMainWindowTitleBar(); //~

    // MSG msg;
    // DWORD ret;
    // while(ret = GetMessage(&msg, NULL, 0, 0)) {
    //     if(msg.hwnd == IoList && msg.message == WM_KEYDOWN) {
    //         if(msg.wParam == VK_TAB) {
    //             SetFocus(MainWindow);
    //             continue;
    //         }
    //     }
    //     if(msg.message == WM_KEYDOWN && msg.wParam != VK_UP &&
    //         msg.wParam != VK_DOWN && msg.wParam != VK_RETURN && msg.wParam
    //         != VK_SHIFT)
    //     {
    //         if(msg.hwnd == IoList) {
    //             msg.hwnd = MainWindow;
    //             SetFocus(MainWindow);
    //         }
    //     }
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }
    
    gtk_main();
    return EXIT_SUCCESS;
}