#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "item.h"

#include <gtk/gtk.h>

typedef struct _ItemGroup InhulItemGroup;

struct _ItemGroup
{
	GString* title;
	/*InhulItem*/GPtrArray* items;
};

#define GET_ITEM_FROM_GROUP(grp, idx) ((InhulItem*)(grp->items->data[idx]))

#define INHUL_TYPE_SQ_CONTAINER inhul_sq_container_get_type()

G_DECLARE_FINAL_TYPE(InhulSqContainer, inhul_sq_container, INHUL, SQ_CONTAINER, GtkContainer)

InhulSqContainer*
inhul_sq_container_new();

#endif //_CONTAINER_H_ 
