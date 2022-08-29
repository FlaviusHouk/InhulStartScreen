#ifndef _GVM_CONTAINER_H_
#define _GVM_CONTAINER_H_

#include "gvm_observable_collection.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GVM_TYPE_CONTAINER gvm_container_get_type()

#define GVM_CONTAINER_ITEMS_PROPERTY "items"

G_DECLARE_DERIVABLE_TYPE(GvmContainer, gvm_container, GVM, CONTAINER, GtkContainer);

struct _GvmContainerClass
{
	GtkContainerClass parentClass;

	GtkWidget* (*generateItem)(gpointer);
	GList* (*getCreatedItems)();

	gpointer pad[12];
};

GvmObservableCollection*
gvm_container_get_items(GvmContainer* this);

void
gvm_container_set_items(GvmContainer* this, GvmObservableCollection* items);

GList*
gvm_container_get_visual_items(GvmContainer* this);

G_END_DECLS

#endif //_GVM_CONTAINER_H_
