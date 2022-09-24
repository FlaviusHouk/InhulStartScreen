#include "inhul_sq_container_group.h"
#include "inhul_sq_container_group_item.h"

#define COLUMN_COUNT 3

static void
inhul_sq_container_group_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural);

static void
inhul_sq_container_group_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data);

struct _InhulSqContainerGroup
{
	GvmContainer base;
	GtkLabel* title;

	InhulViewModelGroup* group;
};


static void
inhul_sq_container_group_fill_item(InhulItemData* this, InhulItemDataStackItem*stack, gint depth, gpointer data);

static void
inhul_sq_container_group_size_allocate(GtkWidget* widget, GtkAllocation* allocation);

static GtkWidget*
inhul_sq_container_group_generate_item(gpointer item);

G_DEFINE_TYPE(InhulSqContainerGroup, inhul_sq_container_group, GVM_TYPE_CONTAINER);

static void
inhul_sq_container_group_class_init(InhulSqContainerGroupClass* klass)
{
	GtkWidgetClass* widgetClass = GTK_WIDGET_CLASS(klass);
	widgetClass->size_allocate = inhul_sq_container_group_size_allocate;
	widgetClass->get_preferred_width = inhul_sq_container_group_get_preferred_width;

	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);
	containerClass->forall = inhul_sq_container_group_forall;

	GvmContainerClass* gvmContainerClass = GVM_CONTAINER_CLASS(klass);
	gvmContainerClass->generateItem = inhul_sq_container_group_generate_item;
}

static void
inhul_sq_container_group_init(InhulSqContainerGroup* this)
{
	gtk_widget_set_has_window(GTK_WIDGET(this), FALSE);
	this->title = NULL;
}

static void
inhul_sq_container_group_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural)
{
	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(widget));

	gint childrenNum = g_list_length(visualChildren);

	gint width = (WIDE_ITEM_WIDTH + MARGIN)* childrenNum + MARGIN * 2;

	if(minimum)
		*minimum = width;

	if(natural)
		*natural = width;

	g_list_free(visualChildren);
}

static void
inhul_sq_container_group_size_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
	InhulSqContainerGroup* this = INHUL_SQ_CONTAINER_GROUP(widget);

	if(this->title)
	{
		gint naturalSize = 0;
		gtk_widget_get_preferred_height(GTK_WIDGET(this->title), NULL, &naturalSize);

		GtkAllocation titleSize = 
		{
			.x = allocation->x,
			.y = allocation->y,
			.width = allocation->width,
			.height = naturalSize
		};

		gtk_widget_size_allocate(GTK_WIDGET(this->title), &titleSize);

		allocation->y+=naturalSize;
		allocation->height-=naturalSize;
	}

	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(this));

	GList* iter = visualChildren;

	GtkAllocation childAlloc =
	{
		.x = allocation->x,
		.y = allocation->y,
		.width = WIDE_ITEM_WIDTH,
		.height = allocation->height
	};

	gint num = g_list_length(visualChildren);
	g_assert(num <= COLUMN_COUNT);

	for(; iter; iter=iter->next)
	{
		GtkWidget* child = GTK_WIDGET(iter->data);

		gtk_widget_size_allocate(child, &childAlloc);

		childAlloc.x += WIDE_ITEM_WIDTH + MARGIN; 
	}

	g_list_free(iter);
}

InhulSqContainerGroup*
inhul_sq_container_group_new(InhulViewModelGroup* group)
{
	g_assert(group);

	InhulSqContainerGroup* this = g_object_new(INHUL_TYPE_SQ_CONTAINER_GROUP, NULL);

	const gchar* name = inhul_view_model_group_get_name(group);

	if(name)
	{
		this->title=GTK_LABEL(gtk_label_new(name));
		gtk_widget_set_parent(GTK_WIDGET(this->title), GTK_WIDGET(this));
	}

	GvmObservableCollection* children = inhul_view_model_group_get_children(group);

	g_assert(gvm_observable_collection_get_length(children) <= COLUMN_COUNT);

	this->group = group;

	gvm_container_set_items(GVM_CONTAINER(this), children);

	return this;
}

static void
inhul_sq_container_group_forall(GtkContainer* container, gboolean include_internals, GtkCallback cb, gpointer cb_data)
{
	InhulSqContainerGroup* this = INHUL_SQ_CONTAINER_GROUP(container);

	if(include_internals && this->title)
		cb(GTK_WIDGET(this->title), cb_data);

	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(container));

	GList* iter = visualChildren;
	for(; iter; iter = iter->next)
	{
		GtkWidget* child = GTK_WIDGET(iter->data);

		cb(child, cb_data);
	}

	g_list_free(visualChildren);
}

static GtkWidget*
inhul_sq_container_group_generate_item(gpointer item)
{
	InhulViewModelItem* childItem = INHUL_VIEW_MODEL_ITEM(item);
	InhulItemDataType itemType = inhul_view_model_item_get_item_type(childItem);

	if(itemType == INHUL_ITEM_DESKTOP_FILE)
	{
		return inhul_sq_container_group_item_create_widget(childItem);
	}

	return GTK_WIDGET(inhul_sq_container_group_item_new(childItem));
}

