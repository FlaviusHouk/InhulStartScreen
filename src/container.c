#include "container.h"

#define ITEM_HEIGHT 130
#define ITEM_WIDTH  270

#define GROUP_WIDTH 12

static InhulItemGroup*
get_item_group(InhulSqContainer* this, int i);

static void
inhul_sq_container_allocate(GtkWidget* widget, GtkAllocation* allocation);

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

	GtkContainerClass* containerClass = GTK_CONTAINER_CLASS(klass);

	containerClass->forall = inhul_sq_container_forall;
}

static void
inhul_sq_container_init(InhulSqContainer* this)
{
	this->groups = g_ptr_array_new();
}

static void
inhul_sq_container_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
	
}

static void
inhul_sq_container_forall(GtkContainer* container, gboolean include_internals, GtkCallback callback, gpointer cb_data)
{
	InhulSqContainer* this = INHUL_SQ_CONTAINER(container);
	
	for(int i = 0; i < this->groups->len; i++)
	{
		InhulItemGroup* grp = get_item_group(this, i);

		
	}
}


/*Groups*/
static InhulItemGroup*
get_item_group(InhulSqContainer* this, int i)
{
	/*TODO: check bounds*/
	return (InhulItemGroup*)this->groups->pdata[i];
}
