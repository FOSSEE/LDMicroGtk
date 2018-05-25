#include "GTK3Test.h"

/// GTK Widgets
GtkWidget *window;
GtkWidget *windowGrid;
GtkWidget *radioButton1, *radioButton2;
GtkWidget *textLabel;
GtkWidget *textBox;
GtkWidget *buttonSubmit;
GtkWidget *buttonCancel;
GtkWidget *textViewWindow;
GtkWidget *scrolledwindow;

/// GDK properties
GdkRGBA color;

void textViewPrint(const char* format, ...)
{
  va_list arglist;
  /// initialize valist for format arguments
  va_start(arglist, format);
  char* str;
  // g_print("--in-print--\n");
  // vprintf(format, arglist);
  vsprintf(str, format, arglist);
  // g_print("text: ");
  // g_print(str);
  // g_print("\n");
  va_end(arglist);
  // g_print("--text-gen-done--\n");

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textViewWindow));
  // g_print("--buffer-created--\n");
  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer), str, strlen(str));
  // g_print("--text-buffered--\n");
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textViewWindow), buffer);
  // g_print("--text-printed--\n");
}

 void setRGB(GdkRGBA* color, int r, int g, int b)
{
  color->red = r;
  color->green = g;
  color->blue = b;
  color->alpha = 1;
}

static void button1(GtkWidget *widget, gpointer data)
{
  textViewPrint ("Button1\n");
}

static void submitClicked(GtkWidget *widget, gpointer data)
{
  g_print("Submitted\n");
  textViewPrint("Submitted\n");

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioButton1)))
  {
    g_print("Radio 1 selected\n");
    textViewPrint("Radio 1 selected\n");
  }
  else
  {
    g_print("Radio 2 selected\n");
    textViewPrint("Radio 2 selected\n");
  }
  
  GtkEntryBuffer *entryBuffer;
  entryBuffer = gtk_entry_get_buffer(GTK_ENTRY(textBox));

  g_print("Text in textbox: %s\n", gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(entryBuffer)));
  textViewPrint("Text in textbox: %s\n", gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(entryBuffer)));

  int w,h;
  gtk_window_get_size(GTK_WINDOW(window), &w, &h);
  
  g_print("Width: %d, Height: %d\n", w, h);
  textViewPrint("Width: %i, Height: %i\n", w, h);
}

static void activate(GtkApplication* app, gpointer user_data)
{
  /// Make window
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  /// grid to hold multiple witgets
  windowGrid = gtk_grid_new();
  /// Attach grid to window
  gtk_container_add(GTK_CONTAINER(window), windowGrid);
  
  /// Make button
  GtkWidget *button;
  button = gtk_button_new_with_label ("Button1");
  g_signal_connect (button, "clicked", G_CALLBACK (button1), NULL);
 
  /** Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
  */
  gtk_grid_attach(GTK_GRID(windowGrid), button, 0, 0, 1, 1);

  /// Make box for 2 radio buttons
  // GtkWidget *radioBox;
  // radioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2); // (orientation, number of rows)
  //gtk_box_set_homogeneous (GTK_BOX(radioBox), TRUE);

  /// Make radio buttons
  radioButton1 = gtk_radio_button_new_with_label(NULL, "radio1");
  radioButton2 = gtk_radio_button_new_with_label(NULL, "radio2");
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radioButton2), gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioButton1)));
  gtk_grid_attach(GTK_GRID(windowGrid), radioButton1, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(windowGrid), radioButton2, 1, 1, 1, 1);

  /// Pack radio buttons in radio box
  // gtk_box_pack_start(GTK_BOX (radioBox), radioButton1, FALSE, FALSE, 0);
  // gtk_box_pack_start(GTK_BOX (radioBox), radioButton2, FALSE, FALSE, 0);
  // gtk_box_pack_start (GTK_BOX(windowGrid), radioBox, FALSE, FALSE, 0);

  /// Make lable for text box
  textLabel = gtk_label_new ("Enter something: ");
  gtk_grid_attach(GTK_GRID(windowGrid), textLabel, 0, 2, 1, 1);
  /// Spacing for widget
  gtk_widget_set_margin_top(GTK_WIDGET(textLabel), 10);
  gtk_widget_set_margin_bottom(GTK_WIDGET(textLabel), 10);
  gtk_widget_set_margin_right(GTK_WIDGET(textLabel), 10);
  gtk_widget_set_margin_left(GTK_WIDGET(textLabel), 10);
  
  /// Make text box
  textBox = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(textBox), 0);
  gtk_grid_attach(GTK_GRID(windowGrid), textBox, 1, 2, 1, 1);

  /// Make submit button
  buttonSubmit = gtk_button_new_with_label ("Submit");
  g_signal_connect (buttonSubmit, "clicked", G_CALLBACK (submitClicked), NULL);
  gtk_grid_attach(GTK_GRID(windowGrid), buttonSubmit, 0, 5, 1, 1);
  setRGB(&color, 0, 1, 0);
  gtk_widget_override_color(GTK_WIDGET(buttonSubmit), GTK_STATE_FLAG_NORMAL, &color);

  /// Make cancel button
  buttonCancel = gtk_button_new_with_label ("Cancel");
  g_signal_connect_swapped (buttonCancel, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_grid_attach(GTK_GRID(windowGrid), buttonCancel, 1, 5, 1, 1);
  setRGB(&color, 1, 0, 0);
  gtk_widget_override_color(GTK_WIDGET(buttonCancel), GTK_STATE_FLAG_NORMAL, &color);

  /// Make text view widget
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  textViewWindow = gtk_text_view_new();
  gtk_container_add(GTK_CONTAINER(scrolledwindow), textViewWindow);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textViewWindow), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textViewWindow), TRUE);
  gtk_text_view_set_overwrite(GTK_TEXT_VIEW(textViewWindow), FALSE);
  //gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textViewWindow), GTK_WRAP_WORD);
  gtk_widget_set_size_request(GTK_WIDGET(scrolledwindow), 2, 80);
  gtk_grid_attach(GTK_GRID(windowGrid), scrolledwindow, 0, 6, 5, 5);

  gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
  /// Make app
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.git.GTK", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
