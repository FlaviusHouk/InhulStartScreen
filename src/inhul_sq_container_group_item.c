#include "inhul_sq_container_group_item.h"

static void
inhul_sq_container_group_item_size_allocate(GtkWidget* this, GtkAllocation* allocation);

static void
inhul_sq_container_group_item_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data);

static GtkWidget*
inhul_sq_container_group_item_generate_item(gpointer item);

struct _InhulSqContainerGroupItem
{
	GvmContainer base;

	InhulItemData* item;
};

G_DEFINE_TYPE(InhulSqContainerGroupItem, inhul_sq_container_group_item, GVM_TYPE_CONTAINER); 

static void
inhul_sq_container_group_item_remove(GtkContainer* container, GtkWidget* child)
{
	gint i = 0;
	i++;
}

static void
inhul_sq_container_group_item_class_init(InhulSqContainerGroupItemClass* klass)
{
	GtkWidgetClass* widgetClass = GTK_WIDGET_CLASS(klass);
	widgetClass->size_allocate = inhul_sq_container_group_item_size_allocate;
	
	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);
	containerClass->forall = inhul_sq_container_group_item_forall;
	containerClass->remove = inhul_sq_container_group_item_remove;

	GvmContainerClass* gvmContainerClass = GVM_CONTAINER_CLASS(klass);
	gvmContainerClass->generateItem = inhul_sq_container_group_item_generate_item;
}

static void
inhul_sq_container_group_item_init(InhulSqContainerGroupItem* this)
{
	gtk_widget_set_has_window(GTK_WIDGET(this), FALSE);
	this->item = NULL;
}

InhulSqContainerGroupItem*
inhul_sq_container_group_item_new(InhulItemData* item)
{
	InhulSqContainerGroupItem* this = g_object_new(INHUL_TYPE_SQ_CONTAINER_GROUP_ITEM, NULL);

	this->item = item;

	g_assert(item->type == INHUL_ITEM_CONTAINER);

	gvm_container_set_items(GVM_CONTAINER(this), item->children);

	return this;
}

static void
inhul_sq_container_group_item_size_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
	InhulSqContainerGroupItem* this = INHUL_SQ_CONTAINER_GROUP_ITEM(widget);

	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(widget));

	if(g_list_length(visualChildren) == 0)
	{
		g_list_free(visualChildren);
		return;
	}

	if(this->item->type == INHUL_ITEM_DESKTOP_FILE)
	{
		GtkWidget* widget = visualChildren->data;

		gtk_widget_size_allocate(widget, allocation);

		return;
	}

	g_assert(this->item->type == INHUL_ITEM_CONTAINER);

	InhulItemLevel level = this->item->level;

	GList* iter = visualChildren;
	switch (level)
	{
		case ITEM_TALL:
			GtkAllocation tallChildAlloc =
			{
				.x = allocation->x + MARGIN / 2,
				.y = allocation->y,
				.width = allocation->width,
				.height = WIDE_ITEM_HEIGHT
			};

			for(; iter; iter = iter->next)
			{
				GtkWidget* child = GTK_WIDGET(iter->data);

				gtk_widget_size_allocate(child, &tallChildAlloc);

				tallChildAlloc.y+=WIDE_ITEM_HEIGHT + MARGIN;
			}

			break;
		
		case ITEM_WIDE:
			GtkAllocation wideChildAlloc =
			{
				.x = allocation->x,
				.y = allocation->y,
				.width = WIDE_ITEM_WIDTH / 2 - MARGIN,
				.height = WIDE_ITEM_HEIGHT
			};

			for(; iter; iter = iter->next)
			{
				GtkWidget* child = GTK_WIDGET(iter->data);

				const gchar* typeName = G_OBJECT_TYPE_NAME(child);

				gtk_widget_size_allocate(child, &wideChildAlloc);

				wideChildAlloc.x+=WIDE_ITEM_WIDTH / 2 + MARGIN / 2;
			}

			break;

		case ITEM_COMMON:
			GtkAllocation childAlloc =
			{
				.x = allocation->x,
				.y = allocation->y,
				.width = WIDE_ITEM_WIDTH / 4 - MARGIN / 2,
				.height = WIDE_ITEM_HEIGHT / 2 - MARGIN / 2
			};

			for(gint i = 0; iter; iter = iter->next)
			{
				GtkWidget* child = GTK_WIDGET(iter->data);

				gtk_widget_size_allocate(child, &childAlloc);

				++i;

				childAlloc.x = (i % 2) == 0 ? 
					allocation->x :
					allocation->x + (WIDE_ITEM_WIDTH / 4 + MARGIN);

				childAlloc.y = i <= 1 ?
					allocation->y :
					allocation->y + (WIDE_ITEM_HEIGHT / 2 + MARGIN);
			}

			break;

		default:
			g_error("Level value(%d) is out of range.", level);
			g_assert(0);
			break;
	}

	g_list_free(visualChildren);
}

static void
inhul_sq_container_group_item_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data)
{
	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(container));

	GList* iter = visualChildren;
	for(; iter; iter = iter->next)
	{
		GtkWidget* child = GTK_WIDGET(iter->data);

		callback(child, cb_data);
	}

	g_list_free(visualChildren);
}

static GtkWidget*
inhul_sq_container_group_item_generate_item(gpointer item)
{
	InhulItemData* childItem = (InhulItemData*)item;

	if(childItem->type == INHUL_ITEM_DESKTOP_FILE)
	{
		return inhul_sq_container_group_item_create_widget_for_desktop_item(childItem->desktopItemData, childItem->level == ITEM_SMALL);
	}

	return GTK_WIDGET(inhul_sq_container_group_item_new(childItem));
}

static void
inhul_sq_container_group_item_activated(GtkButton* button, gpointer data)
{
	InhulCommand* command = (InhulCommand*)data;

	inhul_command_execute(command);
}

GtkWidget*
inhul_sq_container_group_item_create_widget_for_desktop_item(const InhulDesktopItemData* data, gboolean smallItem)
{
	GError* err = NULL;

	GtkWidget* button = gtk_button_new();
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);	

	if(!smallItem)
	{
		GtkWidget* titleLabel = gtk_label_new(data->title);
		gtk_label_set_line_wrap(GTK_LABEL(titleLabel), TRUE);
		gtk_box_pack_end(GTK_BOX(box), titleLabel, FALSE, FALSE, 2);
	}

	GtkIconTheme* iconTheme = gtk_icon_theme_get_default();
	GdkPixbuf* icon = gtk_icon_theme_load_icon(iconTheme, data->icon_name, 32, 0, &err);

	if(icon == NULL)
	{
		if(err != NULL)
		{
			g_warning("%s\n", err->message);
		}
	}
	else
	{
		GtkWidget* image = gtk_image_new_from_pixbuf(icon);
		gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 2);
	}

	gtk_widget_set_tooltip_text(button, data->title);

	g_signal_connect(button, "clicked", G_CALLBACK(inhul_sq_container_group_item_activated), data->command);

	gtk_container_add(GTK_CONTAINER(button), GTK_WIDGET(box));
	return button;
}
