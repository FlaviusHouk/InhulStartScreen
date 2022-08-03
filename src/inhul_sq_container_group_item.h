#ifndef _SQ_CONTAINER_GROUP_ITEM_H_
#define _SQ_CONTAINER_GROUP_ITEM_H_

#include <gtk/gtk.h>

typedef struct _InhulSqContainerGroupItem InhulSqContainerGroupItem;

InhulSqContainerGroupItem*
inhul_sq_container_group_item_new(InhulSqContainerGroupItem* parent);

GtkWidget*
inhul_sq_container_group_item_get_widget(InhulSqContainerGroupItem* item);

void
inhul_sq_container_group_item_set_widget(InhulSqContainerGroupItem* item, GtkWidget* widget);

InhulSqContainerGroupItem*
inhul_sq_container_group_item_get_parent(InhulSqContainerGroupItem* item);

void
inhul_sq_container_group_item_set_parent(InhulSqContainerGroupItem* item, InhulSqContainerGroupItem* parent);

guint
inhul_sq_container_group_item_get_children_count(InhulSqContainerGroupItem* item);

InhulSqContainerGroupItem*
inhul_sq_container_group_item_get_child(InhulSqContainerGroupItem* item, guint index);

void
inhul_sq_container_group_item_set_child(InhulSqContainerGroupItem* item, guint index, InhulSqContainerGroupItem* child);

void
inhul_sq_container_group_item_free(InhulSqContainerGroupItem* item);

#endif //_SQ_CONTAINER_GROUP_ITEM_H_
