#ifndef __LDMICRO_H
#define __LDMICRO_H

// Size of the font that we will use to draw the ladder diagrams, in pixels
#define FONT_WIDTH   "7"
#define FONT_HEIGHT "13"

#include <linux/limits.h>

extern Display *disp;
extern Window MainWindow;
extern XEvent EventHandle;
extern bool RunningInBatchMode;
extern bool life;


extern char CurrentSaveFile[PATH_MAX];
extern char CurrentCompileFile[PATH_MAX];

extern unsigned long ColorSize;

// draw_outputdev.cpp
/*extern void (*DrawChars)(int, int, char *);
void CALLBACK BlinkCursor(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void PaintWindow(void);
void ExportDrawingAsText(char *file);*/
void InitForDrawing(void);/*
void SetUpScrollbars(BOOL *horizShown, SCROLLINFO *horiz, SCROLLINFO *vert);
int ScreenRowsAvailable(void);
int ScreenColsAvailable(void);
extern HFONT FixedWidthFont;
extern HFONT FixedWidthFontBold;
extern int SelectedGxAfterNextPaint;
extern int SelectedGyAfterNextPaint;
extern BOOL ScrollSelectedIntoViewAfterNextPaint;
extern int ScrollXOffset;
extern int ScrollYOffset;
extern int ScrollXOffsetMax;
extern int ScrollYOffsetMax;*/

//-----------------------------------------------
// The syntax highlighting style colours; a structure for the palette.


typedef struct SyntaxHighlightingColoursTag {
    XColor    bg;             // background
    XColor    def;            // default foreground
    XColor    selected;       // selected element
    XColor    op;             // `op code' (like OSR, OSF, ADD, ...)
    XColor    punct;          // punctuation, like square or curly braces
    XColor    lit;            // a literal number
    XColor    name;           // the name of an item
    XColor    rungNum;        // rung numbers
    XColor    comment;        // user-written comment text

    XColor    bus;            // the `bus' at the right and left of screen

    XColor    simBg;          // background, simulation mode
    XColor    simRungNum;     // rung number, simulation mode
    XColor    simOff;         // de-energized element, simulation mode
    XColor    simOn;          // energzied element, simulation mode
    XColor    simBusLeft;     // the `bus,' can be different colours for
    XColor    simBusRight;    // right and left of the screen
} SyntaxHighlightingColours;

extern SyntaxHighlightingColours HighlightColours;




void Error(char *str, ...);

#endif
