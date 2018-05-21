#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include <linux/limits.h>
#include<X11/Xlib.h>
#include "ldmicro.h"

// We should display messages to the user differently if we are running
// interactively vs. in batch (command-line) mode.
//char CurrentSaveFile[PATH_MAX];
//char CurrentCompileFile[PATH_MAX];
bool RunningInBatchMode;

void Error(char *str, ...)
{
    va_list f;
    char buf[1024];
    va_start(f, str);
    vsprintf(buf, str, f);
    if(RunningInBatchMode) {
        /*AttachConsoleDynamic(ATTACH_PARENT_PROCESS);
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;*/

        // Indicate that it's an error, plus the output filename
        char str[PATH_MAX+100];
        sprintf(str, "compile error ('%s'): ", CurrentCompileFile);
        std::cout<<str<<std::endl;
        //WriteFile(h, str, strlen(str), &written, NULL);
        // The error message itself
        std::cout<<buf<<std::endl;
        //WriteFile(h, buf, strlen(buf), &written, NULL);
        // And an extra newline to be safe.
        std::cout<<str<<std::endl;
        //strcpy(str, "\n");
        //WriteFile(h, str, strlen(str), &written, NULL);
    } else {
        //HWND h = GetForegroundWindow();
        //MessageBox(h, buf, _("LDmicro Error"), MB_OK | MB_ICONERROR);
    }
}
