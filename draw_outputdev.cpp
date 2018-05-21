#include <X11/Xlib.h>
#include <string.h>
#include "ldmicro.h"
#include <iostream>

//unsigned long ColorSize;
SyntaxHighlightingColours HighlightColours;

Font FixedWidthFont;
Font FixedWidthFontBold;

XColor toRGB(unsigned long red, unsigned long green, unsigned long blue)
{
    XColor temp;
    temp.red=(short)(red*65535/255);
    temp.green=(short)(green*65535/255);
    temp.blue=(short)(blue*65535/255);
    temp.pixel=((red * ColorSize * ColorSize) | (green * ColorSize)
     | (blue));
    return temp;
}

static void SetSyntaxHighlightingColours(void)
{
    static const SyntaxHighlightingColours Schemes[] = {
        {
            toRGB(0, 0, 0),           // bg
            toRGB(255, 255, 225),     // def
            toRGB(255, 110, 90),      // selected
            toRGB(255, 150, 90),      // op
            toRGB(255, 255, 100),     // punct
            toRGB(255, 160, 160),     // lit
            toRGB(120, 255, 130),     // name
            toRGB(130, 130, 130),     // rungNum
            toRGB(130, 130, 245),     // comment

            toRGB(255, 255, 255),     // bus

            toRGB(0, 0, 0),           // simBg
            toRGB(130, 130, 130),     // simRungNum
            toRGB(100, 130, 130),     // simOff
            toRGB(255, 150, 150),     // simOn

            toRGB(255, 150, 150),     // simBusLeft
            toRGB(150, 150, 255),     // simBusRight
        },
    };

    memcpy(&HighlightColours, &Schemes[0], sizeof(Schemes[0]));
}

void InitForDrawing(void)
{
    SetSyntaxHighlightingColours();
/*   FixedWidthFont.font = 
    FixedWidthFont = CreateFont(
        FONT_HEIGHT, FONT_WIDTH,
        0, 0,
        FW_REGULAR,
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FF_DONTCARE,
        "Lucida Console");

    FixedWidthFontBold = CreateFont(
        FONT_HEIGHT, FONT_WIDTH,
        0, 0,
        FW_REGULAR, // the bold text renders funny under Vista
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FF_DONTCARE,
        "Lucida Console");

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = HighlightColours.simBusRight;
    BusRightBus = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.simBusLeft;
    BusLeftBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.bus;
    BusBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.bg;
    BgBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.simBg;
    SimBgBrush = CreateBrushIndirect(&lb);*/
}
