#include "inhul_item_data.h"

#include "inhul_sq_container.h"
#include "inhul_sq_container_group.h"


static void
inhul_sq_container_allocate(GtkWidget* widget, GtkAllocation* allocation);

static void
inhul_sq_container_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural);

static void
inhul_sq_container_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data);

struct _InhulSqContainer
{
	GtkContainer base;

	GPtrArray* groups;
}; 

G_DEFINE_TYPE(InhulSqContainer, inhul_sq_container, GTK_TYPE_CONTAINER);

static void
inhul_sq_container_class_init(InhulSqContainerClass* klass)
{
	GtkWidgetClass* widgetClass = GTK_WIDGET_CLASS(klass);

	widgetClass->size_allocate = inhul_sq_container_allocate;
	widgetClass->get_preferred_width = inhul_sq_container_get_preferred_width;

	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);

	containerClass->forall = inhul_sq_container_forall;
}

static void
inhul_sq_container_init(InhulSqContainer* this)
{
	gtk_widget_set_has_window(GTK_WIDGET(this), FALSE);
	this->groups = g_ptr_array_new();
}

static void
inhul_sq_container_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural)
{
	InhulSqContainer* this = INHUL_SQ_CONTAINER(widget);
	gint min_value = 0;
	gint nat_value = 0;
	
	for(int i = 0; i<this->groups->len; i++)
	{
		gint child_min = 0;
		gint child_nat = 0;
		InhulSqContainerGroup* group = (InhulSqContainerGroup*)this->groups->pdata[i];

		gtk_widget_get_preferred_width(GTK_WIDGET(group), &child_min, &child_nat);

		min_value+=child_min;
		nat_value+=child_nat;
	}

	if(minimum != NULL)
		*minimum = min_value;

	if(natural != NULL)
		*natural = nat_value;
}


static void
inhul_sq_container_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
	InhulSqContainer* this = INHUL_SQ_CONTAINER(widget);

	GtkAllocation child_alloc =
	{
		.x = allocation->x,
		.y = allocation->y,
		.width = allocation->width,
		.height = allocation->height,
	};

	for(int i = 0; i < this->groups->len; i++)
	{
		gint child_width = 0;

		InhulSqContainerGroup* group = (InhulSqContainerGroup*)this->groups->pdata[i];

		gtk_widget_size_allocate(GTK_WIDGET(group), &child_alloc);

		gtk_widget_get_preferred_width(GTK_WIDGET(group), NULL, &child_width);

		child_alloc.x+=child_width;
	}
}

static void
inhul_sq_container_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data)
{
	InhulSqContainer* this = INHUL_SQ_CONTAINER(container);
	
	for(int i = 0; i < this->groups->len; i++)
	{
		InhulSqContainerGroup* group = INHUL_SQ_CONTAINER_GROUP(this->groups->pdata[i]);

		callback(GTK_WIDGET(group), cb_data);
	}
}

InhulSqContainer*
inhul_sq_container_new(/*InhulItemGroup*/GPtrArray* groups)
{
	InhulSqContainer* this = INHUL_SQ_CONTAINER(g_object_new(INHUL_TYPE_SQ_CONTAINER, NULL));

	for(guint i = 0; i<groups->len; i++)
	{
		InhulItemGroup* groupData = (InhulItemGroup*)groups->pdata[i];

		InhulSqContainerGroup* group = inhul_sq_container_group_new(groupData);

		gtk_widget_set_parent(GTK_WIDGET(group), GTK_WIDGET(this));

		g_ptr_array_add(this->groups, group);
	}

	return this;
}
