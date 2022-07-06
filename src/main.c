#include "gtk/gtk.h"

static void on_application_activated(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window = gtk_application_window_new (app);

    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    gtk_widget_show_all (window);
}

int main(int argc, char** argv)
{
    GtkApplication* app = gtk_application_new("org.inhul.start", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_application_activated), NULL);

    int status = g_application_run(app, argc, argv);

    g_object_unref(app);

    return status;
}
