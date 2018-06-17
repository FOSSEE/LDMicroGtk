#include "linuxUI.h"

/// Variable to current text color
COLORREF HdcCurrentTextColor;

/// EnableMenuItem Variables
const UINT MF_ENABLED = 0;
const UINT MF_GRAYED = 1;
const UINT MF_CHECKED = 2;
const UINT MF_UNCHECKED = 3;

/// ListStore
HWID view;
HTVC column;
 
/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWID window)
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
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new (ofn->lpstrTitle,
                                        GTK_WINDOW(ofn->parentWindow),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    //g_print("filter created\n");
    
    if (ofn->Flags & OFN_OVERWRITEPROMPT == OFN_OVERWRITEPROMPT)
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
                //g_print("filter pat: %s\n", strFilter);
                //g_print("filter reset\n");
                filterResetFlag = FALSE;
            }
            else
            {
                gtk_file_filter_set_name (GTK_FILE_FILTER(filter), strFilter);
                //g_print("filter name: %s\n", strFilter);
                strFilterLen = 0;
                filterResetFlag = TRUE;
            }
    }
    //g_print("filter rules added\n");
    
    sprintf(strFilter, "*.%s", ofn->lpstrDefExt);
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), strFilter);
    //gtk_file_filter_set_name (filter, "int files");
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(dialog), filter);
    
    delete strFilter;

    //g_print("default filter set\n");

    BOOL exitStatus = gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (exitStatus)
    {
        char* str;
        str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
        strcpy(ofn->lpstrFile, str);
        g_free(str);
    }
    //g_print("file path saved: %s\n", ofn->lpstrFile);
    
    gtk_widget_destroy (dialog);

    //g_print("exit\n");

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
            return new COLORREF(0, 0, 0);
            break;
        case WHITE_BRUSH:
            return new COLORREF(255, 255, 255);
            break;
        case GREY_BRUSH:
            return new COLORREF(128, 128, 128);
            break;
        default:
            return new COLORREF(255, 255, 255);
    }
}

void SelectObject(HCRDC hcr, HFONT hfont)
{
    cairo_select_font_face(hcr, hfont->lpszFace,
        hfont->fdwItalic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
        hfont->fnWeight == FW_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);

    cairo_rotate(hcr, hfont->nOrientation);

    cairo_text_extents_t extents;
    cairo_text_extents (hcr, "H", &extents);
 
    cairo_matrix_t matrix;
    cairo_matrix_init_scale (&matrix,
                    (double)hfont->nWidth * 1.6,
                    (double)hfont->nHeight * 1.4);
 
    cairo_set_font_matrix (hcr, &matrix);
    // g_print("wR = %f\nhR = %f\n", (double)hfont->nWidth / extents.width, (double)hfont->nHeight / extents.height);
    // g_print("tW = %f\ntH = %f\n", extents.width, extents.width);
    // cairo_set_font_size(hcr, 15);
}

HBRUSH CreateBrushIndirect(PLOGBRUSH plb)
{
    COLORREF brush(plb->lbColor);
    brush.alpha = (plb->lbStyle == BS_SOLID) ? 1 : 0.2;

    return &brush;
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
    gtk_widget_override_background_color(GTK_WIDGET(widget), 
                        GTK_STATE_FLAG_NORMAL, &bkCol);

    GtkStyleContext *context;
    COLORREF col;

    context = gtk_widget_get_style_context (widget);
    gint width = gtk_widget_get_allocated_width (widget);
    gint height = gtk_widget_get_allocated_height (widget);

    gtk_style_context_get_color (context,
                            gtk_style_context_get_state (context),
                            &col);
    
    gdk_cairo_set_source_rgba (hcr, &col);

    gtk_render_background (context, hcr, 0, 0, width, height);
}

void SetTextColor(HCRDC hcr, COLORREF color)
{
    HdcCurrentTextColor = color;
    gdk_cairo_set_source_rgba (hcr, &color);
}

void TextOut(HCRDC hcr, int nXStart, int nYStart, LPCTSTR lpString, int cchString)
{
    cairo_move_to(hcr, nXStart, nYStart);
    cairo_show_text(hcr, lpString);  

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
    // gtk_widget_queue_draw(hWid);
    gdk_window_invalidate_rect (gtk_widget_get_window (hWid), lpRect, bErase);
}

