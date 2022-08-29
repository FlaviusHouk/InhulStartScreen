#include "gvm_container.h"

#define GVM_CONTAINER_ITEMS_PROP_ID 1

static GParamSpec* itemsProperty;

typedef struct _GvmContainerPrivate GvmContainerPrivate;
struct _GvmContainerPrivate
{
	GvmObservableCollection* items;
	GHashTable* mapping;
};

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(GvmContainer, gvm_container, GTK_TYPE_CONTAINER);

static void
gvm_collection_on_items_changed(GvmObservableCollection* coll, GvmCollectionChangedArgs* args, gpointer userData)
{
	GvmContainer* this = GVM_CONTAINER(userData);
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);
	GHashTable* mapping = priv->mapping;

	if(args->operation == COLLECTION_ADD)
	{
		GtkWidget* child = GVM_CONTAINER_GET_CLASS(this)->generateItem(args->newItem);
	
		g_hash_table_insert(mapping, args->newItem, child);

		gtk_widget_set_parent(child, GTK_WIDGET(this));
	}
}

static void
gvm_container_create_visual_children(GvmContainer* this)
{
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);

	GvmIterator* iter = gvm_observable_collection_get_iterator(priv->items);

	for(gpointer item = gvm_iterator_get_current(iter); gvm_iterator_move_next(iter); item = gvm_iterator_get_current(iter))
	{
		GtkWidget* child = GVM_CONTAINER_GET_CLASS(this)->generateItem(item);

		g_hash_table_insert(priv->mapping, item, child);

		gtk_widget_set_parent(child, GTK_WIDGET(this));
	}

	g_free(iter);
}

static void
gvm_container_set_property(GObject* obj, guint propId, const GValue* value, GParamSpec* pspec)
{
	GvmContainer* this = GVM_CONTAINER(obj);
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);

	switch(propId)
	{
		case GVM_CONTAINER_ITEMS_PROP_ID:
			if(priv->items)
			{
				g_object_unref(priv->items);
			}

			priv->items = GVM_OBSERVABLE_COLLECTION(g_value_get_object(value));
			g_signal_connect(priv->items, GVM_OBSERVABLE_COLLECTION_CHANGED_SIGNAL, G_CALLBACK(gvm_collection_on_items_changed), this);
			priv->mapping = g_hash_table_new(NULL, NULL);

			gvm_container_create_visual_children(this);

			break;

		default:
			G_OBJECT_CLASS(gvm_container_parent_class)->set_property(obj, propId, value, pspec);
			break;
	}
}

static void
gvm_container_get_property(GObject* obj, guint propId, GValue* value, GParamSpec* pspec)
{
	GvmContainer* this = (GvmContainer*)obj;
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);

	switch(propId)
	{
		case GVM_CONTAINER_ITEMS_PROP_ID:
			g_value_set_object(value, G_OBJECT(priv->items));
			break;

		default:
			G_OBJECT_CLASS(gvm_container_parent_class)->get_property(obj, propId, value, pspec);
			break;
	}
}

static void
gvm_container_class_init(GvmContainerClass* klass)
{
	klass->generateItem = NULL;
	
	GObjectClass* gObjectClass = G_OBJECT_CLASS(klass);
	gObjectClass->set_property = gvm_container_set_property;
	gObjectClass->get_property = gvm_container_get_property;

	klass->generateItem = NULL;

	itemsProperty = g_param_spec_object(GVM_CONTAINER_ITEMS_PROPERTY, "items", "Collection of items to visualize", GVM_TYPE_OBSERVABLE_COLLECTION, G_PARAM_READWRITE);

	g_object_class_install_property(gObjectClass, GVM_CONTAINER_ITEMS_PROP_ID, itemsProperty);
}

static void
gvm_container_init(GvmContainer* this)
{
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);

	priv->items=NULL;
	priv->mapping=NULL;
}


void
gvm_container_set_items(GvmContainer* this, GvmObservableCollection* items)
{
	GValue value = G_VALUE_INIT;
	g_value_init(&value, GVM_TYPE_OBSERVABLE_COLLECTION);
	g_value_set_object(&value, G_OBJECT(items));

	g_object_set_property(G_OBJECT(this), GVM_CONTAINER_ITEMS_PROPERTY, &value);
}

GvmObservableCollection*
gvm_container_get_items(GvmContainer* this)
{
	GObject* items;

	g_object_get(G_OBJECT(this), GVM_CONTAINER_ITEMS_PROPERTY, &items, NULL);

	return GVM_OBSERVABLE_COLLECTION(items);
}

GList*
gvm_container_get_visual_items(GvmContainer* this)
{
	GvmContainerPrivate* priv = gvm_container_get_instance_private(this);

	GList* list = NULL;
	
	if(priv->items == NULL)
		return list;

	GvmIterator* iter = gvm_observable_collection_get_iterator(priv->items);

	for(gpointer item = gvm_iterator_get_current(iter); gvm_iterator_move_next(iter); item = gvm_iterator_get_current(iter))
	{
		gpointer visualChild = g_hash_table_lookup(priv->mapping, item);

		//Not the best way. TODO: refactor
		list = g_list_append(list, visualChild);
	}

	return list;
}
