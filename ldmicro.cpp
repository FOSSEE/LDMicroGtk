#include<linux/limits.h>
#include<X11/Xlib.h>
#include<assert.h>
#include<unistd.h>
#include<iostream>
#include<stdbool.h>
#include<string.h>
#include<stdio.h>
#include "include/ldmicro.h"
#include<cmath>
#include<stdlib.h>

bool life = true;
unsigned long ColorSize;
char CurrentSaveFile[PATH_MAX];
char CurrentCompileFile[PATH_MAX];
Display *disp;
Window MainWindow;
XEvent EventHandle;

int main(int argc,char* argv[])
{
    disp = XOpenDisplay(NULL);
    assert(disp);
    unsigned long BlackColor = BlackPixel(disp,DefaultScreen(disp));
    unsigned long WhiteColor = WhitePixel(disp, DefaultScreen(disp));
    ColorSize = (cbrt(WhiteColor + 1));
    MainWindow = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0,
         800, 600, 0, BlackColor, BlackColor);                              //Needs to be modified later(border,background,depth)
    //IoListHeight = 100;
    //InitCommonControls();
    InitForDrawing();

    //MakeMainWindowControls();
    //MainWindowResized();

    //NewProgram();
    strcpy(CurrentSaveFile, "");
    // Check if we're running in non-interactive mode; in that case we should
    // load the file, compile, and exit.
    if((argc>1)){
        RunningInBatchMode = true;
        char *err = (char*)"Bad command line arguments: run 'ldmicro /c src.ld dest.hex'";
        if(argc!=4) { Error(err); exit(-1); }
        if(memcmp(argv[1], "/c", 2) == 0) {
            char *source = argv[2];
            char *dest = source;

        /*if(!LoadProjectFromFile(source)) {
            //Error("Couldn't open '%s', running non-interactively.",
            source);
            exit(-1);
        }*/
        strcpy(CurrentCompileFile, dest);
        //GenerateIoList(-1);
        //CompileProgram(false);
        exit(0);
    }
    else{ Error(err); exit(-1); }
    }

    // We are running interactively, or we would already have exited. We
    // can therefore show the window now, and otherwise set up the GUI.
    XSelectInput(disp, MainWindow, StructureNotifyMask);
    XMapWindow(disp, MainWindow);
    GC GraphicsContext = XCreateGC(disp, MainWindow, 0, NULL);
    XSetForeground(disp,GraphicsContext,WhiteColor);                        //Change the drawing color later.
    Atom wmDeleteMessage = XInternAtom(disp, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(disp, MainWindow, &wmDeleteMessage, 1);
    for(;;)
    {
        XNextEvent(disp,&EventHandle);
        if(EventHandle.type == MapNotify)
            break;
    }
    XFlush(disp);        //Show Display
    while(life){
        XNextEvent(disp,&EventHandle);
        switch(EventHandle.type){
            case ClientMessage:
                if(EventHandle.xclient.data.l[0] = wmDeleteMessage)
                    life=false;
            break;
        }
    }
}
