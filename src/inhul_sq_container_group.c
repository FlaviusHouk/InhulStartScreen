#include "inhul_sq_container_group.h"
#include "inhul_sq_container_group_item.h"

#define COLUMN_COUNT 3

#define MARGIN      5

static void
inhul_sq_container_group_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data);


struct _InhulSqContainerGroup
{
	GtkContainer base;
	GtkLabel* title;

	InhulSqContainerGroupItem** columns;	
};

typedef struct _StackItem StackItem;
struct _StackItem
{
	InhulSqContainerGroupItem* item;
	gint idx;
};

typedef void (*ItemCallback)(GtkWidget*, const StackItem*, gint, gpointer);
typedef struct _CallbackWithData CallbackWithData;
struct _CallbackWithData
{
	GtkCallback callback;
	gpointer data;
};

typedef struct _FillContext FillContext;
struct _FillContext
{
	InhulSqContainerGroup* this;
	StackItem* stack;
};

static void
inhul_sq_container_group_fill_item(InhulItemData* this, InhulItemDataStackItem*stack, gint depth, gpointer data);

static GtkWidget*
inhul_sq_container_group_create_widget_for_desktop_item(const InhulDesktopItemData* data, gboolean smallItem);

static void
inhul_sq_container_group_size_allocate(GtkWidget* widget, GtkAllocation* allocation);

static void
inhul_sq_container_group_traverse_children(const InhulSqContainerGroup* this, ItemCallback cb, gpointer data);

G_DEFINE_TYPE(InhulSqContainerGroup, inhul_sq_container_group, GTK_TYPE_CONTAINER);

static void
inhul_sq_container_group_class_init(InhulSqContainerGroupClass* klass)
{
	GtkWidgetClass* widgetClass = GTK_WIDGET_CLASS(klass);

	widgetClass->size_allocate = inhul_sq_container_group_size_allocate;

	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);

	containerClass->forall = inhul_sq_container_group_forall;
}

static void
inhul_sq_container_group_init(InhulSqContainerGroup* this)
{
	gtk_widget_set_has_window(GTK_WIDGET(this), FALSE);
	this->title = NULL;

	this->columns = g_new0(InhulSqContainerGroupItem*, COLUMN_COUNT);
}

static void
inhul_sq_container_allocate_child_size(GtkWidget* child, const StackItem* stack,  gint depth,  gpointer data)
{
	GtkAllocation* fullSize = (GtkAllocation*)data;

	g_assert(depth <= ITEM_SMALL && depth >= 0);

	GtkAllocation childAllocation;
	if (depth == ITEM_TALL)
	{
		gint x = fullSize->x + (MARGIN + WIDE_ITEM_WIDTH) * stack[0].idx;
		
		childAllocation.x = x;
		childAllocation.y = fullSize->y + MARGIN;
		childAllocation.width = WIDE_ITEM_WIDTH - 2 * MARGIN;
		childAllocation.height = fullSize->height - 2 * MARGIN;
	}
	else if(depth == ITEM_WIDE)
	{
		gint x = fullSize->x + (MARGIN + WIDE_ITEM_WIDTH) * stack[0].idx;
		gint y = fullSize->y + (MARGIN + WIDE_ITEM_HEIGHT) * stack[1].idx;

		childAllocation.x = x;
		childAllocation.y = y;
		childAllocation.width = WIDE_ITEM_WIDTH - 2 * MARGIN;
		childAllocation.height = WIDE_ITEM_HEIGHT - 2 * MARGIN;
	}
	else if(depth == ITEM_COMMON)
	{
		gint x = fullSize->x + (MARGIN + WIDE_ITEM_WIDTH) * stack[0].idx;
		gint y = fullSize->y + (MARGIN + WIDE_ITEM_HEIGHT) * stack[1].idx;

		if(stack[2].idx == 1)
		{
			x += WIDE_ITEM_WIDTH / 2 + MARGIN * 2;
		}

		childAllocation.x = x;
		childAllocation.y = y;
		childAllocation.width = (WIDE_ITEM_WIDTH / 2) - 2 * MARGIN;
		childAllocation.height = WIDE_ITEM_HEIGHT - 2 * MARGIN;
	}
	else
	{
		gint x = fullSize->x + (MARGIN + WIDE_ITEM_WIDTH) * stack[0].idx;
		gint y = fullSize->y + (MARGIN + WIDE_ITEM_HEIGHT) * stack[1].idx;

		if(stack[2].idx == 1)
		{
			x += WIDE_ITEM_WIDTH / 2 + MARGIN;
		}

		x += ((WIDE_ITEM_WIDTH / 4)) * (stack[3].idx & 1);
		y += ((WIDE_ITEM_HEIGHT / 4)) * (stack[3].idx & 2);

		childAllocation.x = x;
		childAllocation.y = y;
		childAllocation.width = (WIDE_ITEM_WIDTH / 4) - MARGIN;
		childAllocation.height = (WIDE_ITEM_HEIGHT / 2) - MARGIN;
	}

	gtk_widget_size_allocate(child, &childAllocation);
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

	inhul_sq_container_group_traverse_children(this, inhul_sq_container_allocate_child_size, allocation);
}

static void
inhul_sq_container_call_gtk_callback(GtkWidget* child, const StackItem* stack, gint depth, gpointer data)
{
	CallbackWithData* cb = (CallbackWithData*)data;

	cb->callback(child, cb->data);
}

static void
inhul_sq_container_group_traverse_children(const InhulSqContainerGroup* this, ItemCallback cb, gpointer data)
{
	StackItem stack[STACK_DEPTH + 1];

	gint depth;
	for(int i = 0; i<COLUMN_COUNT; i++)
	{
		depth = 1;
		InhulSqContainerGroupItem* item = this->columns[i];

		if(item == NULL)
			continue;

		stack[0].item = NULL;
		stack[0].idx = i;

		stack[depth].item = item;
		stack[depth].idx = 0;
		while(TRUE)
		{
			InhulSqContainerGroupItem* curr = stack[depth].item;
			guint childrenCount = inhul_sq_container_group_item_get_children_count(curr);
			if(childrenCount == 0)
			{
				GtkWidget* widget = inhul_sq_container_group_item_get_widget(curr);

				if(widget)
					cb(widget, stack, depth, data);

				depth--;
				stack[depth].idx++;

				continue;
			}

			gint index = stack[depth].idx;

			if(index >= childrenCount)
			{
				stack[depth].idx = 0;
				stack[depth--].item  = NULL;

				if(depth == 0)
					break;
					
				stack[depth].idx++;

				continue;
			}

			InhulSqContainerGroupItem* child = inhul_sq_container_group_item_get_child(curr, index);

			if(child != NULL)
			{
				stack[++depth].item = child;
				stack[depth].idx  = 0;
			}
		}
	}
}

static void
inhul_sq_container_group_forall(GtkContainer* container, gboolean include_internals, GtkCallback cb, gpointer cb_data)
{
	InhulSqContainerGroup* this = INHUL_SQ_CONTAINER_GROUP(container);

	if(include_internals && this->title);
		cb(GTK_WIDGET(this->title), cb_data);

	CallbackWithData cbWrapper =
	{
		.data = cb_data,
		.callback = cb
	};

	inhul_sq_container_group_traverse_children(this, inhul_sq_container_call_gtk_callback, &cbWrapper);
}

InhulSqContainerGroup*
inhul_sq_container_group_new(const InhulItemGroup* group)
{
	InhulSqContainerGroup* this = g_object_new(INHUL_TYPE_SQ_CONTAINER_GROUP, NULL);

	if(group->name)
	{
		const gchar* name = g_strdup(group->name);
		this->title=GTK_LABEL(gtk_label_new(name));
		gtk_widget_set_parent(GTK_WIDGET(this->title), GTK_WIDGET(this));
	}

	g_assert(group->children->len < COLUMN_COUNT);
	this->columns = g_new0(InhulSqContainerGroupItem*, COLUMN_COUNT);

	StackItem stack[STACK_DEPTH + 1] =
	{
		{ NULL, 0 },
		{ NULL, 0 },
		{ NULL, 0 },
		{ NULL, 0 },
		{ NULL, 0 }
	};

	FillContext ctx = 
	{
		.this = this,
		.stack = stack
	};

	inhul_item_data_traverse_group(group, inhul_sq_container_group_fill_item, &ctx);

	return this;
}


static void
inhul_sq_container_group_fill_item(InhulItemData* item, InhulItemDataStackItem*stack, gint depth, gpointer data)
{
	FillContext* ctx = (FillContext*)data;
	InhulSqContainerGroup* this = ctx->this;
	
	g_assert(depth <= ITEM_SMALL && depth >= 0);

	InhulSqContainerGroupItem* parentItem = NULL;
	gint childIndex = 0;

	if(depth > 0)
	{
		parentItem = ctx->stack[depth - 1].item;
		childIndex = ctx->stack[depth - 1].idx++;
	}

	if(ctx->stack[depth].idx > stack[depth].idx)
	{
		for(int i = depth; i<STACK_DEPTH; i++)
		{
			ctx->stack[i].idx = 0;
		}
	}	

	if(item == NULL)
	{
		inhul_sq_container_group_item_set_child(parentItem, childIndex, NULL);
	}
	else if(item->type == INHUL_ITEM_DESKTOP_FILE)
	{
		InhulSqContainerGroupItem* groupItem = inhul_sq_container_group_item_new(parentItem);

		if(depth == ITEM_TALL)
		{
			this->columns[childIndex]=groupItem;
		}

		const InhulDesktopItemData* desktopItemData = item->desktopItemData;

		GtkWidget* widget = inhul_sq_container_group_create_widget_for_desktop_item(desktopItemData, depth == ITEM_SMALL);

		inhul_sq_container_group_item_set_widget(groupItem, widget);

		gtk_widget_set_parent(widget, GTK_WIDGET(this));

		if(parentItem)
			inhul_sq_container_group_item_set_child(parentItem, childIndex, groupItem);

		ctx->stack[depth].item = groupItem;

		return;
	}
	else if(item->type == INHUL_ITEM_CONTAINER)
	{
		InhulSqContainerGroupItem* childItem = inhul_sq_container_group_item_new(parentItem);

		if(depth == ITEM_TALL)
		{
			this->columns[childIndex] = childItem;
		}

		ctx->stack[depth].item = childItem;

		if(parentItem)
			inhul_sq_container_group_item_set_child(parentItem, childIndex, childItem);

		return;
	}

	g_assert(FALSE);
}

static void
inhul_sq_container_group_item_activated(GtkButton* button, gpointer data)
{
	InhulCommand* command = (InhulCommand*)data;

	inhul_command_execute(command);
}

static GtkWidget*
inhul_sq_container_group_create_widget_for_desktop_item(const InhulDesktopItemData* data, gboolean smallItem)
{
	GError* err = NULL;

	GtkWidget* button = gtk_button_new();
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);	

	if(!smallItem)
	{
		GtkWidget* titleLabel = gtk_label_new(data->title);
		gtk_box_pack_end(GTK_BOX(box), titleLabel, FALSE, FALSE, 2);
	}

	GtkIconTheme* iconTheme = gtk_icon_theme_get_default();
	GdkPixbuf* icon = gtk_icon_theme_load_icon(iconTheme, data->icon_name, 32, 0, &err);

	if(icon == NULL)
	{
		g_print("%s\n", err->message);
		g_assert(0);
	}

	GtkWidget* image = gtk_image_new_from_pixbuf(icon);
	gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 2);

	gtk_widget_set_tooltip_text(image, data->title);

	g_signal_connect(button, "clicked", G_CALLBACK(inhul_sq_container_group_item_activated), data->command);

	gtk_container_add(GTK_CONTAINER(button), GTK_WIDGET(box));
	return button;
}

