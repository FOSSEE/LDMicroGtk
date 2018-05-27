#include "linuxUI/linuxUI.h"

/// Wraper function for gtk_window_has_toplevel_focus
BOOL isFocus(HWND window)
{
    return (BOOL) gtk_window_has_toplevel_focus(GTK_WINDOW(gtk_widget_get_parent_window(GTK_WIDGET(window))));
}

COLORREF RGB(int red, int green, int blue)
{
    COLORREF col;
    col.red = red/255.0;
    col.green = green/255.0;
    col.blue = blue/255.0;
    col.alpha = 1.0;

    return col;
}

gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  guint width, height;
  COLORREF color = RGB (255, 0, 0);
	
  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);
  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             0, 2 * G_PI);

  gtk_style_context_get_color (gtk_widget_get_style_context (widget),
                              GTK_STATE_FLAG_NORMAL,
                              &color);

  gdk_cairo_set_source_rgba (cr, &color);

  cairo_fill (cr);

 return FALSE;
}

static void load (GtkApplication* app, gpointer user_data)
{
	/// Make window
	HWND window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  HWND drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 100, 100);
  g_signal_connect (G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_callback), NULL);

	gtk_container_add(GTK_CONTAINER(window), drawing_area);

	gtk_widget_show_all (window);
}

int main (int argc, char** argv)
{
	/// Make app
	HAPP app;
	int status;

	app = gtk_application_new ("org.git.ldmicro", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (load), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	
	return status;
}