#ifndef _INHUL_SQ_CONTAINER_GROUP_H_
#define _INHUL_SQ_CONTAINER_GROUP_H_

#include "inhul_item_data.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define WIDE_ITEM_HEIGHT 130
#define WIDE_ITEM_WIDTH  270

#define GROUP_WIDTH 12

#define INHUL_TYPE_SQ_CONTAINER_GROUP inhul_sq_container_group_get_type()

G_DECLARE_FINAL_TYPE(InhulSqContainerGroup, inhul_sq_container_group, INHUL, SQ_CONTAINER_GROUP, GtkContainer);

InhulSqContainerGroup*
inhul_sq_container_group_new(const InhulItemGroup* group);

G_END_DECLS

#endif //_INHUL_SQ_CONTAINER_GROUP_H_
