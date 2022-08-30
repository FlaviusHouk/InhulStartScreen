#ifndef _SQ_CONTAINER_GROUP_ITEM_H_
#define _SQ_CONTAINER_GROUP_ITEM_H_

#include "inhul_item_data.h"
#include "gvm_container.h"

#include <gtk/gtk.h>

#define WIDE_ITEM_HEIGHT 120
#define WIDE_ITEM_WIDTH  250

#define MARGIN 5

#define INHUL_TYPE_SQ_CONTAINER_GROUP_ITEM inhul_sq_container_group_item_get_type()

G_DECLARE_FINAL_TYPE(InhulSqContainerGroupItem, inhul_sq_container_group_item, INHUL, SQ_CONTAINER_GROUP_ITEM, GvmContainer);

GtkWidget*
inhul_sq_container_group_item_create_widget_for_desktop_item(const InhulDesktopItemData* data, gboolean smallItem);

InhulSqContainerGroupItem*
inhul_sq_container_group_item_new(InhulItemData* item);

#endif //_SQ_CONTAINER_GROUP_ITEM_H_
