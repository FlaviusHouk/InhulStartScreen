#include "inhul_sq_container_group_item.h"

struct _InhulSqContainerGroupItem
{
	InhulSqContainerGroupItem* parent;
	GtkWidget* content;
	/*InhulSqContanierGroupItem*/GPtrArray* children;
};

InhulSqContainerGroupItem*
inhul_sq_container_group_item_new(InhulSqContainerGroupItem* parent)
{
	InhulSqContainerGroupItem* this = g_new(InhulSqContainerGroupItem, 1);

	this->parent   =  parent;
	this->content  =  NULL;
	this->children =  g_ptr_array_new();

	return this;
}

GtkWidget*
inhul_sq_container_group_item_get_widget(InhulSqContainerGroupItem* item)
{
	return item->content;
}

void
inhul_sq_container_group_item_set_widget(InhulSqContainerGroupItem* item, GtkWidget* widget)
{
	g_object_ref(G_OBJECT(widget));
	item->content = widget;
}

InhulSqContainerGroupItem*
inhul_sq_container_group_item_get_parent(InhulSqContainerGroupItem* item)
{
	return item->parent;
}

void
inhul_sq_container_group_item_set_parent(InhulSqContainerGroupItem* item, InhulSqContainerGroupItem* parent)
{
	item->parent = parent;
}

guint
inhul_sq_container_group_item_get_children_count(InhulSqContainerGroupItem* item)
{
	return item->children->len;
}

InhulSqContainerGroupItem*
inhul_sq_container_group_item_get_child(InhulSqContainerGroupItem* item, guint index)
{
	if(index >= item->children->len)
		g_assert(FALSE);

	return (InhulSqContainerGroupItem*)item->children->pdata[index];
}

void
inhul_sq_container_group_item_set_child(InhulSqContainerGroupItem* item, guint index, InhulSqContainerGroupItem* child)
{
	if(index >= item->children->len)
		g_ptr_array_add(item->children, child);

	item->children->pdata[index] = child;
}

void
inhul_sq_container_group_item_free(InhulSqContainerGroupItem* item)
{
	if(item->content)
		g_object_unref(G_OBJECT(item->content));

	g_free(item->children);
	g_free(item);
}
