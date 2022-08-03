#define GETTEXT_PACKAGE "gtk3"
#include <glib/gi18n-lib.h>
#include <locale.h>

#include "inhul_sq_container.h"
#include "inhul_item_data_json.h"

static gchar* configFileName;

static GOptionEntry entries[] = 
{
	{ "config", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_FILENAME, &configFileName, "Configuration file with stored groups to show.", "config"},
	{ NULL }
};

static void on_application_activated(GtkApplication* app, gpointer user_data)
{
	GError* err = NULL;

    GtkWidget* window = gtk_application_window_new (app);

    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget* titleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget* title = gtk_label_new("Start menu");

	gtk_box_pack_start(GTK_BOX(titleBox), GTK_WIDGET(title), TRUE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(titleBox), FALSE, FALSE, 5);

	GPtrArray* itemData = inhul_item_data_load_from_json(configFileName, &err);
	InhulSqContainer* sqContainer = inhul_sq_container_new(itemData);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(sqContainer), TRUE, TRUE, 5);

	gtk_container_set_border_width(GTK_CONTAINER(box), 25);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));
    gtk_widget_show_all (window);
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_PACKAGE, DATADIR "/locale");
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	GError* err = NULL;
	GOptionContext* context = g_option_context_new("Start menu options");
	g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);
	if(!g_option_context_parse(context, &argc, &argv, &err))
	{
		g_print("Failed to parse entries: %s\n.", err->message);

		return 1;
	}

    GtkApplication* app = gtk_application_new("org.inhul.start", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_application_activated), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}
