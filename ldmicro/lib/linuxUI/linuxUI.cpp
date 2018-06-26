#include "linuxUI.h"

/// Global variables to hole mouse click positions
int GLOBAL_mouse_last_clicked_x;
int GLOBAL_mouse_last_clicked_y;

/// Brushes
const COLORREF BLACK_BR(0, 0, 0);
const COLORREF WHITE_BR(255, 255, 255);
const COLORREF GRAY_BR(128, 128, 128);
const COLORREF LTGRAY_BR(211, 211, 211);
const COLORREF DKGRAY_BR(169, 169, 169);

/// Variable to current text color
COLORREF HdcCurrentTextColor;

/// Variable to hold timers
std::vector<TimerRecord> timerRecords;

/// EnableMenuItem Variables
const UINT MF_ENABLED = 0;
const UINT MF_GRAYED = 1;
const UINT MF_CHECKED = 2;
const UINT MF_UNCHECKED = 3;

/// Accelerators (keyboard shortcuts)
GtkAccelGroup* AccelGroup;
GClosure* closure;

/// ListStore
HWID view;
HTVC column;
 
/// Wraper function for gtk_window_has_toplevel_focus
BOOL GetFocus(HWID window)
{
    return (BOOL) gtk_window_has_toplevel_focus(GTK_WINDOW(window));
}

COLORREF RGB(int red, int green, int blue)
{
    COLORREF col(red, green, blue);
    // col.red = red/255.0;
    // col.green = green/255.0;
    // col.blue = blue/255.0;
    // col.alpha = 1.0;

    return col;
}

int MessageBox(HWID pWindow, char* message, char* title, UINT mFlags)
 {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkMessageType mType;
    
    if ((mFlags & MB_ICONERROR) == MB_ICONERROR)
        mType = GTK_MESSAGE_ERROR;
    else if ((mFlags & MB_ICONQUESTION) == MB_ICONQUESTION)
        mType = GTK_MESSAGE_QUESTION;
    else if ((mFlags & MB_ICONWARNING) == MB_ICONWARNING)
        mType = GTK_MESSAGE_WARNING;
    else if ((mFlags & MB_ICONINFORMATION) == MB_ICONINFORMATION)
        mType = GTK_MESSAGE_INFO;
    else
        mType = GTK_MESSAGE_OTHER;

    mType = GTK_MESSAGE_ERROR;
    HWID dialog = gtk_message_dialog_new (GTK_WINDOW(pWindow),
                                    flags,
                                    mType,
                                    GTK_BUTTONS_NONE,
                                    message);

    if ((mFlags & MB_OKCANCEL) == MB_OKCANCEL)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_OK", IDOK);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_CANCEL", IDCANCEL);
    }
    else if ((mFlags & MB_YESNO) == MB_YESNO)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_YES", IDYES);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_NO", IDNO);
    }
    else if ((mFlags & MB_YESNOCANCEL) == MB_YESNOCANCEL)
    {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_YES", IDYES);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_NO", IDNO);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "_CANCEL", IDCANCEL);
    }
    else
        gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", IDOK);
    
    gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG(dialog),
                               title);
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), message);
    int result = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    return result;
 }


BOOL GetSaveFileName(OPENFILENAME *ofn)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

    dialog = gtk_file_chooser_dialog_new (ofn->lpstrTitle,
                                        GTK_WINDOW(ofn->parentWindow),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Save",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    char filename[15] = "Untitled";

    if (ofn->lpstrDefExt != NULL)
        sprintf(filename, "Untitled.%s", ofn->lpstrDefExt);
    
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(dialog), filename);

    if (ofn->Flags & OFN_OVERWRITEPROMPT)
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    
    GtkFileFilter *filter = gtk_file_filter_new ();
    char* strFilter = new char[strlen(ofn->lpstrFilter)];
    DWORD strFilterLen = 0;
    BOOL filterResetFlag = FALSE;
    
    for (int i = 0; !(ofn->lpstrFilter[i] == '\0' && ofn->lpstrFilter[i-1] == '\0'); ++i)   
    {
        memcpy (strFilter + strFilterLen, &ofn->lpstrFilter[i], 1 );
        ++strFilterLen;
        if (ofn->lpstrFilter[i] == '\0')
            if (filterResetFlag)
            {
                gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
                gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(dialog), filter);
                filter = gtk_file_filter_new ();
                strFilterLen = 0;
                filterResetFlag = FALSE;
            }
            else
            {
                gtk_file_filter_set_name (GTK_FILE_FILTER(filter), strFilter);
                strFilterLen = 0;
                filterResetFlag = TRUE;
            }
    }
    
    sprintf(strFilter, "*.%s", ofn->lpstrDefExt);
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(dialog), filter);
    
    delete strFilter;

    BOOL exitStatus = gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (exitStatus)
    {
        char* str;
        str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
        
        strcpy(ofn->lpstrFile, str);
        g_free(str);
    }
    
    gtk_widget_destroy (dialog);

    return exitStatus;
}

BOOL GetOpenFileName(OPENFILENAME *ofn)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new (ofn->lpstrTitle,
                                        GTK_WINDOW(ofn->parentWindow),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    
    GtkFileFilter *filter = gtk_file_filter_new ();
    char* strFilter = new char[strlen(ofn->lpstrFilter)];
    DWORD strFilterLen = 0;
    BOOL filterResetFlag = FALSE;
    
    for (int i = 0; !(ofn->lpstrFilter[i] == '\0' && ofn->lpstrFilter[i-1] == '\0'); ++i)   
    {
        memcpy (strFilter + strFilterLen, &ofn->lpstrFilter[i], 1 );
        ++strFilterLen;
        if (ofn->lpstrFilter[i] == '\0')
            if (filterResetFlag)
            {
                gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
                gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(dialog), filter);
                filter = gtk_file_filter_new ();
                strFilterLen = 0;
                filterResetFlag = FALSE;
            }
            else
            {
                gtk_file_filter_set_name (GTK_FILE_FILTER(filter), strFilter);
                strFilterLen = 0;
                filterResetFlag = TRUE;
            }
    }
    
    sprintf(strFilter, "*.%s", ofn->lpstrDefExt);
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
    //gtk_file_filter_set_name (filter, "int files");
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(dialog), filter);
    
    delete strFilter;

    BOOL exitStatus = gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (exitStatus)
    {
        char* str;
        str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
        strcpy(ofn->lpstrFile, str);
        g_free(str);
    }
    
    gtk_widget_destroy (dialog);

    return exitStatus;
}


void EnableMenuItem(HMENU MenuName, HMENU MenuItem, UINT CheckEnabledItem) 
{
    switch (CheckEnabledItem){
        case MF_ENABLED :
           gtk_widget_set_sensitive (MenuItem, true);
        break;
        case MF_GRAYED :
           gtk_widget_set_sensitive (MenuItem, false);
        break; 
    }
}

void CheckMenuItem(HMENU MenuName, HMENU MenuItem, UINT Check)
{
    switch (Check){
        case MF_CHECKED :
            gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(MenuItem), true);
        break;
        case MF_UNCHECKED :
            gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(MenuItem), false);
        break;
    }
}

HANDLE GetStockObject(int fnObject)
{
    switch(fnObject)
    {
        case BLACK_BRUSH:
            return (HANDLE)&BLACK_BR;
            break;
        case WHITE_BRUSH:
            return (HANDLE)&WHITE_BR;
            break;
        case GRAY_BRUSH:
            return (HANDLE)&GRAY_BR;
            break;
        case LTGRAY_BRUSH:
            return (HANDLE)&LTGRAY_BR;
            break;
        case DKGRAY_BRUSH:
            return (HANDLE)&DKGRAY_BR;
            break;
        default:
            return (HANDLE)&WHITE_BR;
    }
}

void SelectObject(HCRDC hcr, HFONT hfont)
{
    if (hcr ==NULL)
        return;
    
    cairo_select_font_face(hcr, hfont->lpszFace,
        hfont->fdwItalic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
        hfont->fnWeight == FW_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);

    cairo_rotate(hcr, hfont->nOrientation);
    // cairo_text_extents_t extents;
    // cairo_text_extents (hcr, "Z", &extents);
    // cairo_matrix_t matrix;
    // cairo_matrix_init_scale (&matrix,
    //                 (double)hfont->nWidth,
    //                 (double)hfont->nHeight);
 
    // cairo_set_font_matrix (hcr, &matrix);

    cairo_set_font_size(hcr, 10);
}

HBRUSH CreateBrushIndirect(PLOGBRUSH plb)
{
    COLORREF* brush = new COLORREF;
    brush->red = plb->lbColor.red;
    brush->green = plb->lbColor.green;
    brush->blue = plb->lbColor.blue;
    brush->alpha = (plb->lbStyle == BS_SOLID) ? 1 : 0.2;

    return brush;
}

HFONT CreateFont(int nHeight, int nWidth, int nOrientation, int fnWeight,
    DWORD fdwItalic, LPCTSTR lpszFace)
{
    HFONT font = new FONT;
    font->nHeight = nHeight;
    font->nWidth = nWidth;
    font->nOrientation = nOrientation;
    font->fnWeight = fnWeight;
    font->fdwItalic = fdwItalic;
    font->lpszFace = lpszFace;

    return font;
}

void SetBkColor(HWID widget, HCRDC hcr, COLORREF bkCol)
{
    if (hcr == NULL)
        return;
    
    gtk_widget_override_background_color(GTK_WIDGET(widget), 
                        GTK_STATE_FLAG_NORMAL, &bkCol);

    gint width = gtk_widget_get_allocated_width (widget);
    gint height = gtk_widget_get_allocated_height (widget);

    // COLORREF col;
    // GtkStyleContext *context;

    // context = gtk_widget_get_style_context (widget);

    // gtk_style_context_get_color (context,
    //                     gtk_style_context_get_state (context),
    //                     &col);
    gdk_cairo_set_source_rgba (hcr, &bkCol);
    // cairo_rectangle(hcr, 0, 0, width, height);
    // cairo_stroke_preserve(hcr);

    cairo_fill (hcr);
}

void SetTextColor(HCRDC hcr, COLORREF color)
{
    if (hcr == NULL)
        return;
    
    HdcCurrentTextColor = color;
    gdk_cairo_set_source_rgba (hcr, &color);
}

void TextOut(HWID hWid, HCRDC hcr, int nXStart, int nYStart, LPCTSTR lpString, int cchString)
{
    if (hcr == NULL)
        return;
    
    nYStart += 30;
    
    cairo_text_extents_t extents;
    cairo_text_extents (hcr, lpString, &extents);
    int width = gtk_widget_get_allocated_width (hWid);
    int height= gtk_widget_get_allocated_height (hWid);
    BOOL resize_flag = FALSE;

    if(nYStart+(extents.height/2.0) >= height)
    {
        height += extents.height + 50;
        resize_flag = TRUE;
    }
    
    if (nXStart+(extents.width/2.0) >= width)
    {
        width += extents.width;
        resize_flag = TRUE;
    }

    if (resize_flag)
        gtk_widget_set_size_request(hWid, width, height);
    
    char* text = (char*)malloc(cchString);
    strncpy(text, lpString, cchString);
    text[cchString] = '\0';

    cairo_move_to(hcr, nXStart, nYStart);
    cairo_show_text(hcr, text);

    cairo_fill (hcr);
}

COLORREF GetTextColor(HCRDC Hdc)
{
    // COLORREF col;
    // gtk_style_context_get_color (Hdc,
    //                             gtk_style_context_get_state (Hdc),
    //                             &col);
    
    return HdcCurrentTextColor;
}

BOOL InvalidateRect(HWID hWid, const RECT *lpRect, BOOL bErase)
{
    if(!GDK_IS_WINDOW(hWid))
        return FALSE;

    if (lpRect == NULL)
    {
        gdk_window_invalidate_rect (gtk_widget_get_window (hWid), NULL, FALSE);
        return TRUE;
    }

    GDRECT Gdrect;
    RECT_to_GDRECT(lpRect, &Gdrect);
    // gtk_widget_queue_draw(hWid);
    gdk_window_invalidate_rect (gtk_widget_get_window (hWid), &Gdrect, FALSE);
    
    return TRUE;
}

int FillRect(HCRDC hDC, const RECT *lprc, HBRUSH hbr)
{
    if (hDC == NULL)
        return -1;
    
    GDRECT gdrc;
    RECT_to_GDRECT(lprc, &gdrc);

    cairo_set_source_rgb(hDC, hbr->red, hbr->green, hbr->blue);
    cairo_rectangle(hDC, gdrc.x, gdrc.y, gdrc.width, gdrc.height);
    cairo_stroke_preserve(hDC);
    cairo_fill(hDC);
    
    return 0;
}

BOOL PatBlt(HCRDC hdc, int nXLeft, int nYLeft, int nWidth, int nHeight, DWORD dwRop, HBRUSH hbr)
{
    if (hdc == NULL)
        return FALSE;
    
    cairo_set_source_rgb(hdc, hbr->red, hbr->green, hbr->blue);
    cairo_rectangle(hdc, nXLeft, nYLeft + 20, nWidth, nHeight);
    cairo_stroke_preserve(hdc);

    cairo_fill(hdc);

    return TRUE;
}

BOOL GetClientRect(HWID hWid, PRECT pRect)
{   
    GtkAllocation allocation;
    gtk_widget_get_allocation (hWid, &allocation);

    pRect->top = allocation.x;
    pRect->left = allocation.y;
    pRect->right = allocation.width;
    pRect->bottom = allocation.height;

    return TRUE;
}

BOOL MoveWindow(HWID hWid, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    gtk_window_move(GTK_WINDOW(hWid), X, Y);
    gtk_window_resize(GTK_WINDOW(hWid), nWidth, nHeight);

    if (bRepaint)
        gdk_window_invalidate_rect (gtk_widget_get_window (hWid), NULL, FALSE);
    
    return TRUE;
}


BOOL GetWindowRect(HWID hWid, PRECT pRect)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (hWid, &allocation);

    pRect->top = allocation.x;
    pRect->left = allocation.y;
    pRect->right = allocation.width;
    pRect->bottom = allocation.height;

    return TRUE;
}

UINT SetTimer(HWID hWid, UINT  nIDEvent, UINT uElapse, BOOL (*lpTimerFunc)(BOOL) )
{
    auto record_it = std::find_if(timerRecords.begin(), timerRecords.end(),  [&nIDEvent](TimerRecord &Record) { return Record.ufID == nIDEvent; });

    if (record_it != timerRecords.end())
        return 0;

    TimerRecord tr;
    tr.pfun = lpTimerFunc;
    tr.ufID = nIDEvent;
    tr.utID = g_timeout_add(uElapse, (GSourceFunc)lpTimerFunc, FALSE);

    timerRecords.push_back(tr);
    return tr.utID;
}

BOOL KillTimer(HWID hWid, UINT uIDEvent)
{
    auto record_it = std::find_if(timerRecords.begin(), timerRecords.end(),  [&uIDEvent](TimerRecord &Record) { return Record.ufID == uIDEvent; });

    if (record_it == timerRecords.end())
        return FALSE;
    
    record_it->pfun(TRUE);
    g_source_remove (record_it->utID);
    timerRecords.erase(record_it);

    return TRUE;
}

void DestroyWindow (HWID widget)
{
    gtk_widget_destroy (widget);
}