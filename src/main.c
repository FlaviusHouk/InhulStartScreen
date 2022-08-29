#define GETTEXT_PACKAGE "gtk3"
#include <glib/gi18n-lib.h>
#include <locale.h>

#include "inhul_sq_container.h"
#include "inhul_item_data_json.h"
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
	GvmObservableCollection* items;
};

static gboolean
set_items_on_idle(gpointer data)
{
	gboolean val = g_idle_remove_by_data(data);
	struct _SetItemsData* d = (struct _SetItemsData*)data;

	gvm_container_set_items(GVM_CONTAINER(d->container), d->items);
	gtk_widget_show_all(GTK_WIDGET(d->container));

	g_free(d);

	return G_SOURCE_REMOVE;
}

static void
on_size_allocated(GtkWidget* widget, GtkAllocation* allocation, gpointer data)
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(widget), on_size_allocated, data);

	InhulSqContainer* container = INHUL_SQ_CONTAINER(widget);
	GPtrArray* items = (GPtrArray*)data;
	GvmObservableCollection* structuredItems = gvm_observable_collection_new();

	gint height = allocation->height / (WIDE_ITEM_HEIGHT + 5);
	gint groupElemCount = height * 2 * 3;
	gint groupCount = items->len / groupElemCount;

	for(gint i = 0; i < groupCount; i++)
	{
		InhulItemGroup* group = g_new0(InhulItemGroup, 1);
		group->children = gvm_observable_collection_new();

		for(gint j = 0; j < 3; j++)
		{
			InhulItemData* tallItem = g_new(InhulItemData, 1);
			tallItem->type = INHUL_ITEM_CONTAINER;
			tallItem->level = ITEM_TALL;
			tallItem->children = gvm_observable_collection_new();

			for(gint k = 0; k < height; k++)
			{	
				gint index = i * groupElemCount + j * 2 * height + k * 2;

				if(index >= items->len)
				{
					gvm_observable_collection_add(group->children, tallItem);

					goto cycle_end;
				}

				InhulItemData* wideItem = g_new(InhulItemData, 1);
				wideItem->type = INHUL_ITEM_CONTAINER;
				wideItem->level = ITEM_WIDE;
				wideItem->children = gvm_observable_collection_new();
				
				InhulItemData* item = g_new(InhulItemData, 1);
				item->type = INHUL_ITEM_DESKTOP_FILE;
				item->level = ITEM_COMMON;
				item->desktopItemData = (InhulDesktopItemData*)items->pdata[index];

				gvm_observable_collection_add(wideItem->children, item);
				gvm_observable_collection_add(tallItem->children, wideItem);
				index++;

				if(index >= items->len)
				{
					gvm_observable_collection_add(group->children, tallItem);

					goto cycle_end;
				}

				item = g_new(InhulItemData, 1);
				item->type = INHUL_ITEM_DESKTOP_FILE;
				item->level = ITEM_COMMON;
				item->desktopItemData = (InhulDesktopItemData*)items->pdata[index];

				gvm_observable_collection_add(wideItem->children, item);
			}

			gvm_observable_collection_add(group->children, tallItem);
		}

		cycle_end:
		gvm_observable_collection_add(structuredItems, group);
	}

	struct _SetItemsData* d = g_new(struct _SetItemsData, 1);
	d->container = container;
	d->items = structuredItems;

	g_idle_add(set_items_on_idle, d);
}

static void on_application_activated(GtkApplication* app, gpointer user_data)
{
	GError* err = NULL;

    GtkWidget* window = gtk_application_window_new (app);

    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_widget_show(window);

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget* titleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget* title = gtk_label_new("Start menu");

	gtk_box_pack_start(GTK_BOX(titleBox), GTK_WIDGET(title), TRUE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(titleBox), FALSE, FALSE, 5);

	InhulSqContainer* sqContainer = inhul_sq_container_new();
	if(configFileName != NULL)
	{	
		/*InhulItemGroup*/ GPtrArray* groups = inhul_item_data_load_from_json(configFileName, &err);

		if(err != NULL)
		{
			g_print("%s\n", err->message);
			g_application_release(G_APPLICATION(app));
		}
 
		GvmObservableCollection* items = gvm_observable_collection_new_with_data(groups);

		gvm_container_set_items(GVM_CONTAINER(sqContainer), items);
	}

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(sqContainer), TRUE, TRUE, 5);

	gtk_container_set_border_width(GTK_CONTAINER(box), 25);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));
    gtk_widget_show_all (box);

	if(configFileName == NULL)
	{
		/*InhulDesktopItemData*/ GPtrArray* desktopItems = inhul_item_data_generate_default();

		g_signal_connect(G_OBJECT(sqContainer), "size-allocate", G_CALLBACK(on_size_allocated), desktopItems);
	}
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
