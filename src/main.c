#define GETTEXT_PACKAGE "gtk3"
#include <glib/gi18n-lib.h>
#include <locale.h>
#include <math.h>

#include "inhul_sq_container.h"
#include "inhul_item_data_json.h"
#include "inhul_view_model_group.h"
#include "inhul_view_model_main.h"
#include "inhul_sq_container_group_item.h"

#define DEFAULT_LAYOUT_FILE_NAME "layout.json"

static gchar* configFileName;

static GOptionEntry entries[] = 
{
	{ "config", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_FILENAME, &configFileName, "Configuration file with stored groups to show.", "config"},
	{ NULL }
};

struct _SetItemsData
{
	InhulSqContainer* container;
	InhulViewModelMain* mainVm;
	gint height;
};

static gboolean
set_items_on_idle(gpointer data)
{
	struct _SetItemsData* d = (struct _SetItemsData*)data;

	inhul_view_model_main_generate_defaults(d->mainVm, d->height);
	gtk_widget_show_all(GTK_WIDGET(d->container));

	g_free(d);

	return G_SOURCE_REMOVE;
}

static void
on_size_allocated(GtkWidget* widget, GtkAllocation* allocation, gpointer data)
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(widget), on_size_allocated, data);

	InhulSqContainer* container = INHUL_SQ_CONTAINER(widget);
	InhulViewModelMain* mainVm = INHUL_VIEW_MODEL_MAIN(data);

	struct _SetItemsData* d = g_new(struct _SetItemsData, 1);	
	d->height = allocation->height / (WIDE_ITEM_HEIGHT + 5);
	d->container = container;
	d->mainVm = mainVm;

	g_idle_add(set_items_on_idle, d);
}

static void
on_shutdown(GApplication* app, gpointer data)
{
	GError* err = NULL;

	InhulViewModelMain* mainVm = INHUL_VIEW_MODEL_MAIN(data);
	inhul_view_model_main_save_config(mainVm, "layout.json", &err);

	if(err != NULL)
	{
		g_error("%s\n", err->message);
	}
}

static void
load_styles()
{
	GError* err = NULL;

	GdkScreen* screen = gdk_screen_get_default();
	GtkCssProvider* styleProvider = gtk_css_provider_new();

	GFile* cssFile = g_file_new_for_path("styles.css");
	gtk_css_provider_load_from_file(styleProvider, cssFile, &err);

	if(err)
	{
		g_error("%s\n", err->message);
		g_assert(FALSE);
	}

	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(styleProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_object_unref(G_OBJECT(cssFile));
	g_object_unref(G_OBJECT(styleProvider));
}

static void 
on_application_activated(GtkApplication* app, gpointer user_data)
{
	GError* err = NULL;

    GtkWidget* window = gtk_application_window_new (app);

    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_widget_show(window);

	load_styles();

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget* titleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget* title = gtk_label_new("Start menu");

	gtk_box_pack_start(GTK_BOX(titleBox), GTK_WIDGET(title), TRUE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(titleBox), FALSE, FALSE, 5);

	InhulViewModelMain* mainVm = inhul_view_model_main_new ();
	InhulSqContainer* sqContainer = inhul_sq_container_new(mainVm);

	if(configFileName != NULL)
	{	
		inhul_view_model_main_read_items(mainVm, configFileName, &err);

		if(err != NULL)
		{
			g_error("%s\n", err->message);
			g_application_release(G_APPLICATION(app));
			return;
		}
	}

	GtkWidget* scroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroller), GTK_WIDGET(sqContainer));

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(scroller), TRUE, TRUE, 5);

	gint margin = 50;

	g_object_set(G_OBJECT(sqContainer), "margin-top", margin, "margin-right", margin, "margin-left", margin, NULL);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));
    gtk_widget_show_all (box);

	if(configFileName == NULL)
	{
		g_signal_connect(G_OBJECT(sqContainer), "size-allocate", G_CALLBACK(on_size_allocated), mainVm);
	}

	g_signal_connect(G_OBJECT(app), "shutdown", G_CALLBACK(on_shutdown), mainVm);
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
