#include "inhul_item_data.h"

#include "inhul_sq_container.h"
#include "inhul_sq_container_group.h"
#include "inhul_sq_container_group_item.h"

static void
inhul_sq_container_allocate(GtkWidget* widget, GtkAllocation* allocation);

static void
inhul_sq_container_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural);

static void
inhul_sq_container_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data);

static void
inhul_sq_container_allocate_child_size(GtkWidget* child, gpointer data);

static GtkWidget*
inhul_sq_container_generate_item(gpointer item);

struct _InhulSqContainer
{
	GtkContainer base;
}; 

G_DEFINE_TYPE(InhulSqContainer, inhul_sq_container, GVM_TYPE_CONTAINER);

static void
inhul_sq_container_class_init(InhulSqContainerClass* klass)
{
	GtkWidgetClass* widgetClass = GTK_WIDGET_CLASS(klass);
	widgetClass->size_allocate = inhul_sq_container_allocate;
	widgetClass->get_preferred_width = inhul_sq_container_get_preferred_width;

	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);
	containerClass->forall = inhul_sq_container_forall;

	GvmContainerClass* gvmContainerClass = GVM_CONTAINER_CLASS(klass);
	gvmContainerClass->generateItem = inhul_sq_container_generate_item;
}

static void
inhul_sq_container_init(InhulSqContainer* this)
{
	gtk_widget_set_has_window(GTK_WIDGET(this), FALSE);
}

static void
inhul_sq_container_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural)
{
	gint min_value = 0;
	gint nat_value = 0;

	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(widget));

	GList* iter = visualChildren;
	for(; iter; iter = iter->next)
	{
		gint child_min = 0;
		gint child_nat = 0;

		GtkWidget* child = GTK_WIDGET(iter->data);

		gtk_widget_get_preferred_width(GTK_WIDGET(child), &child_min, &child_nat);

		min_value+=child_min;
		nat_value+=child_nat;
	}

	if(minimum != NULL)
		*minimum = min_value;

	if(natural != NULL)
		*natural = nat_value;

	g_list_free(visualChildren);
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

	gtk_widget_set_allocation(widget, allocation);

	inhul_sq_container_forall(GTK_CONTAINER(widget), FALSE, inhul_sq_container_allocate_child_size, &child_alloc);
}

static void
inhul_sq_container_allocate_child_size(GtkWidget* child, gpointer data)
{
	GtkAllocation* allocation = (GtkAllocation*)data;
	gint child_width = 0;

	gtk_widget_size_allocate(child, allocation);
	gtk_widget_get_preferred_width(child, NULL, &child_width);

	allocation->x+=child_width + 5 /*MARGIN*/;
}

static void
inhul_sq_container_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data)
{
	GList* visualChildren = gvm_container_get_visual_items(GVM_CONTAINER(container));
	GList* iter = visualChildren;

	for(; iter; iter = iter->next)
	{
		GtkWidget* child = GTK_WIDGET(iter->data);

		callback(GTK_WIDGET(child), cb_data);
	}

	//TODO: uncomment and investigate SIGSEGV.
	//g_free(visualChildren);
}

InhulSqContainer*
inhul_sq_container_new(InhulViewModelMain* vm)
{
	g_assert(vm);

	InhulSqContainer* this = INHUL_SQ_CONTAINER(g_object_new(INHUL_TYPE_SQ_CONTAINER, NULL));

	GvmObservableCollection* groups = inhul_view_model_main_get_groups(vm);
	gvm_container_set_items(GVM_CONTAINER(this), groups);

	return this;
}

static GtkWidget*
inhul_sq_container_generate_item(gpointer item)
{
	InhulViewModelGroup* group = (InhulViewModelGroup*)item;

	return GTK_WIDGET(inhul_sq_container_group_new(group));
}
