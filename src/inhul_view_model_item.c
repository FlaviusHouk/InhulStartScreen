#include "inhul_view_model_item.h"

struct _InhulViewModelItem
{
	GObject baseObject;

	InhulItemData* item;
	gboolean isSelected;

	GvmObservableCollection* children;

	InhulCommand* makeBiggerCommand;
	InhulCommand* makeSmallerCommand;
};

typedef enum
{
	PROP_LEVEL = 1,
	PROP_IS_SELECTED,
	N_PROPS
} InhulViewModelItemProps;

static GParamSpec* props[N_PROPS] = { NULL, };

G_DEFINE_FINAL_TYPE(InhulViewModelItem, inhul_view_model_item, G_TYPE_OBJECT);

static void
inhul_view_model_item_get_property(GObject* obj, guint propId, GValue* value, GParamSpec* pspec)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(obj);

	switch((InhulViewModelItemProps)propId)
	{
		case PROP_LEVEL:
			g_value_set_int(value, this->item->level);
			break;

		case PROP_IS_SELECTED:
			g_value_set_boolean(value, this->isSelected);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_item_set_property(GObject* obj, guint propId, const GValue* value, GParamSpec* pspec)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(obj);

	switch((InhulViewModelItemProps)propId)
	{
		case PROP_IS_SELECTED:
			this->isSelected = g_value_get_boolean(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_item_dispose(GObject* obj)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(obj);


}

static void
inhul_view_model_item_class_init(InhulViewModelItemClass* klass)
{
	GObjectClass* objectClass = G_OBJECT_CLASS(klass);
	objectClass->get_property = inhul_view_model_item_get_property;
	objectClass->set_property = inhul_view_model_item_set_property;
	objectClass->dispose      = inhul_view_model_item_dispose;

	props[PROP_LEVEL] = g_param_spec_int(INHUL_VIEW_MODEL_ITEM_PROP_LEVEL, "Level", "Level on which item is placed", ITEM_TALL, ITEM_SMALL + 1, ITEM_TALL, G_PARAM_READABLE);

	props[PROP_IS_SELECTED] = g_param_spec_boolean(INHUL_VIEW_MODEL_ITEM_PROP_IS_SELECTED, "Is selected", "Flag that identifies that item is selected", FALSE, G_PARAM_READWRITE);

	g_object_class_install_properties(objectClass, N_PROPS, props);
}

static void
inhul_view_model_item_init(InhulViewModelItem* this)
{
	this->item = NULL;
	this->children = NULL;
	this->makeBiggerCommand = NULL;
	this->makeSmallerCommand = NULL;
	this->isSelected = FALSE;
}

InhulViewModelItem*
inhul_view_model_item_new(InhulItemData* item)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(g_object_new(INHUL_VIEW_MODEL_TYPE_ITEM, NULL));

	g_assert(item);

	this->item = item;

	return this;
}

static void
inhul_view_model_item_children_changed (GObject* obj, GvmCollectionChangedArgs* args, gpointer data)
{
	GvmObservableCollection* children = GVM_OBSERVABLE_COLLECTION(obj);
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(data);

	if(args->operation == COLLECTION_ADD)
	{
		InhulViewModelItem* child = INHUL_VIEW_MODEL_ITEM(args->newItem);

		g_ptr_array_add(this->item->children, child->item);
	}
	else if(args->operation == COLLECTION_CHANGE_POSITION)
	{
		InhulViewModelItem* oldChild = INHUL_VIEW_MODEL_ITEM(args->oldItem);

		guint index;
		g_assert(g_ptr_array_find(this->item->children, oldChild, &index));

		//TODO: decide when it is possible.
	}
	else if(args->operation == COLLECTION_REMOVE)
	{
		InhulViewModelItem* oldItem = INHUL_VIEW_MODEL_ITEM(args->oldItem);

		//TODO: remove index?
		g_ptr_array_remove(this->item->children, oldItem->item);
	}
}

GvmObservableCollection*
inhul_view_model_item_get_children(InhulViewModelItem* this)
{
	g_assert(this);

	if(this->children)
		return this->children;

	g_assert(this->item->type == INHUL_ITEM_CONTAINER);

	GPtrArray* childrenArr = g_ptr_array_new();
	GPtrArray* itemChildren = this->item->children;
	
	for(gint i = 0; i < itemChildren->len; i++)
	{
		InhulItemData* child = (InhulItemData*)itemChildren->pdata[i];

		InhulViewModelItem* childVm = inhul_view_model_item_new(child);

		g_ptr_array_add(childrenArr, childVm);
	}

	this->children = gvm_observable_collection_new_with_data(childrenArr);

	g_signal_connect(G_OBJECT(this->children), GVM_OBSERVABLE_COLLECTION_CHANGED_SIGNAL, G_CALLBACK(inhul_view_model_item_children_changed), this);

	return this->children;
}

InhulItemLevel
inhul_view_model_item_get_level(InhulViewModelItem* this)
{
	g_assert(this);

	return this->item->level;
}

InhulItemDataType
inhul_view_model_item_get_item_type(InhulViewModelItem* this)
{
	g_assert(this);

	return this->item->type;
}

InhulDesktopItemData*
inhul_view_model_item_get_desktop_item_data(InhulViewModelItem* this)
{
	g_assert(this);
	g_assert(this->item->type == INHUL_ITEM_DESKTOP_FILE);

	return this->item->desktopItemData;
}

gboolean
inhul_view_model_item_get_is_selected(InhulViewModelItem* this)
{
	g_assert(this);

	gboolean value = FALSE;

	g_object_get(G_OBJECT(this), INHUL_VIEW_MODEL_ITEM_PROP_IS_SELECTED, &value, NULL);

	return value;
}

void
inhul_view_model_item_set_is_selected(InhulViewModelItem* this, gboolean value)
{
	g_assert(this);

	g_object_set(G_OBJECT(this), INHUL_VIEW_MODEL_ITEM_PROP_IS_SELECTED, value, NULL);
}

/*Make bigger command*/
static void
inhul_view_model_item_make_bigger_execute(gpointer data)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(data);

	g_assert(this->item->level > ITEM_TALL);
	this->item->level--;

	g_object_notify(G_OBJECT(this), INHUL_VIEW_MODEL_ITEM_PROP_LEVEL);
}

static gboolean
inhul_view_model_item_make_bigger_can_execute(gpointer data)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(data);

	return this->item->level > ITEM_TALL;
}

InhulCommand*
inhul_view_model_item_get_make_bigger_command(InhulViewModelItem* this)
{
	if(this->makeBiggerCommand == NULL)
	{
		this->makeBiggerCommand = inhul_command_new_custom(inhul_view_model_item_make_bigger_execute, inhul_view_model_item_make_bigger_can_execute, this, NULL);
	}

	return this->makeBiggerCommand;
}

/*Make smaller command*/
static void
inhul_view_model_item_make_smaller_execute(gpointer data)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(data);

	g_assert(this->item->level < ITEM_SMALL);
	this->item->level--;

	g_object_notify(G_OBJECT(this), INHUL_VIEW_MODEL_ITEM_PROP_LEVEL);
}

static gboolean
inhul_view_model_make_smaller_can_execute(gpointer data)
{
	InhulViewModelItem* this = INHUL_VIEW_MODEL_ITEM(data);

	return this->item->level < ITEM_SMALL;
}

InhulCommand*
inhul_view_model_item_get_make_smaller_command(InhulViewModelItem* this)
{
	if(this->makeSmallerCommand == NULL)
	{
		this->makeSmallerCommand = inhul_command_new_custom(inhul_view_model_item_make_smaller_execute, inhul_view_model_make_smaller_can_execute, this, NULL);
	}

	return this->makeSmallerCommand;
}
