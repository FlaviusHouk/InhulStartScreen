#include "inhul_view_model_group.h"
#include "inhul_view_model_item.h"

struct _InhulViewModelGroup
{
	GObject baseObject;

	GvmObservableCollection* children;
	InhulItemGroup* item;
};

typedef enum {
	PROP_NAME = 1,
	PROP_CHILDREN,
	N_PROPS
} InhulViewModelGroupProps;

static GParamSpec* obj_props[N_PROPS] = { NULL, };

G_DEFINE_FINAL_TYPE(InhulViewModelGroup, inhul_view_model_group, G_TYPE_OBJECT);

static void
inhul_view_model_group_set_property(GObject* obj, guint propId, const GValue* value, GParamSpec* pspec)
{
	InhulViewModelGroup* this = INHUL_VIEW_MODEL_GROUP(obj);

	switch((InhulViewModelGroupProps)propId)
	{
		case PROP_NAME:
			if(this->item->name)
				g_free(this->item->name);

			const gchar* newName = g_value_get_string(value);

			if(newName)
				this->item->name = g_strdup(newName);
			else
				this->item->name = NULL;

			break;

		case PROP_CHILDREN:
			GObject* newChildren = g_value_get_object(value);
			g_object_ref(newChildren);

			this->children = GVM_OBSERVABLE_COLLECTION(newChildren);

			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_group_on_child_level_change (GObject* obj, GParamSpec* spec, gpointer data)
{
	InhulViewModelItem* child = INHUL_VIEW_MODEL_ITEM(obj);
	InhulViewModelGroup* this = INHUL_VIEW_MODEL_GROUP(data);

	InhulItemLevel newLevel = inhul_view_model_item_get_level(child);
	g_assert(newLevel == ITEM_WIDE);

	guint index;
	g_assert(gvm_observable_collection_find(this->children, child, &index));

	InhulItemData* newTall = g_new0(InhulItemData, 1);
	newTall->level = ITEM_TALL;
	newTall->type = INHUL_ITEM_CONTAINER;
	newTall->children = g_ptr_array_new();

	InhulViewModelItem* newTallVm = inhul_view_model_item_new(newTall);

	GvmObservableCollection* newChildren = inhul_view_model_item_get_children(newTallVm);

	gvm_observable_collection_set(this->children, newTall, index);

	gvm_observable_collection_add(newChildren, child);
}

static GvmObservableCollection*
inhul_view_model_group_generate_children(InhulViewModelGroup* this)
{
	InhulItemGroup* group = this->item;
	GPtrArray* children = g_ptr_array_new();

	for(gint i = 0; i < group->children->len; i++)
	{
		InhulItemData* child = (InhulItemData*)group->children->pdata[i];
		InhulViewModelItem* childVm = inhul_view_model_item_new(child);

		g_ptr_array_add(children, childVm);

		g_signal_connect(childVm, "notify::level", G_CALLBACK(inhul_view_model_group_on_child_level_change), this);
	}

	return gvm_observable_collection_new_with_data(children);
}

static void
inhul_view_model_group_get_property(GObject* obj, guint propId, GValue* value, GParamSpec* pspec)
{
	InhulViewModelGroup* this = INHUL_VIEW_MODEL_GROUP(obj);

	switch((InhulViewModelGroupProps)propId)
	{
		case PROP_NAME:
			g_value_set_string(value, this->item->name);
			break;

		case PROP_CHILDREN:
			if(this->children == NULL)
				this->children = inhul_view_model_group_generate_children(this);

			g_value_set_object(value, G_OBJECT(this->children));

			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_group_class_init(InhulViewModelGroupClass* klass)
{
	GObjectClass* objectClass = G_OBJECT_CLASS(klass);

	objectClass->get_property = inhul_view_model_group_get_property;
	objectClass->set_property = inhul_view_model_group_set_property;

	obj_props[PROP_NAME] = g_param_spec_string(INHUL_VIEW_MODEL_GROUP_PROP_NAME, "Name", "Name of the group", NULL, G_PARAM_READWRITE);

	obj_props[PROP_CHILDREN] = g_param_spec_object(INHUL_VIEW_MODEL_GROUP_PROP_CHILDREN, "Children", "Children of this group", GVM_TYPE_OBSERVABLE_COLLECTION, G_PARAM_READABLE);

	g_object_class_install_properties(objectClass, N_PROPS, obj_props);
}

static void
inhul_view_model_group_init(InhulViewModelGroup* this)
{
	this->children = NULL;
	this->item = NULL;
}

InhulViewModelGroup*
inhul_view_model_group_new(InhulItemGroup* group)
{
	g_assert(group);

	InhulViewModelGroup* this = INHUL_VIEW_MODEL_GROUP(g_object_new(INHUL_VIEW_MODEL_TYPE_GROUP, NULL));

	this->item = group;

	return this;
}

const gchar*
inhul_view_model_group_get_name(InhulViewModelGroup* group)
{
	g_assert(group);

	return group->item->name;
}

void
inhul_view_model_group_set_name(InhulViewModelGroup* group, const gchar* name)
{
	g_assert(group);

	g_object_set(group, INHUL_VIEW_MODEL_GROUP_PROP_NAME, &name);
}

GvmObservableCollection*
inhul_view_model_group_get_children(InhulViewModelGroup* group)
{
	g_assert(group);

	GvmObservableCollection* children = NULL;

	g_object_get(group, INHUL_VIEW_MODEL_GROUP_PROP_CHILDREN, &children, NULL);

	

	return children;
}

InhulItemGroup*
inhul_view_model_group_get_group(InhulViewModelGroup* this)
{
	g_assert(this);

	return this->item;
}
