#include "inhul_view_model_main.h"

#include "inhul_item_data_json.h"
#include "inhul_view_model_group.h"

#include <math.h>

enum
{
	PROP_GROUPS = 1,
	PROP_SELECTED_ITEM,
	N_PROPS
} InhulViewModelMainProps;

static GParamSpec* props[N_PROPS] = { NULL, };

struct _InhulViewModelMain
{
	GObject baseObject;

	GvmObservableCollection* groups;
	InhulViewModelItem* selectedItem;
};

G_DEFINE_TYPE(InhulViewModelMain, inhul_view_model_main, G_TYPE_OBJECT);

static void
inhul_view_model_main_get_value(GObject* obj, guint propId, GValue* value, GParamSpec* pspec)
{
	InhulViewModelMain* this = INHUL_VIEW_MODEL_MAIN(obj);

	switch(propId)
	{
		case PROP_GROUPS:
			g_value_set_object(value, G_OBJECT(this->groups));
			break;

		case PROP_SELECTED_ITEM:
			g_value_set_object(value, G_OBJECT(this->selectedItem));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_main_set_value(GObject* obj, guint propId, const GValue* value, GParamSpec* pspec)
{
	InhulViewModelMain* this = INHUL_VIEW_MODEL_MAIN(obj);

	switch(propId)
	{
		case PROP_SELECTED_ITEM:
			InhulViewModelItem* newSelectedItem = g_value_get_object(value);

			//TODO: check item is present in hierarchy?
			if(newSelectedItem == NULL && this->selectedItem != NULL)
			{
				inhul_view_model_item_set_is_selected(this->selectedItem, FALSE);
				return;
			}

			inhul_view_model_item_set_is_selected(newSelectedItem, TRUE);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propId, pspec);
			break;
	}
}

static void
inhul_view_model_main_class_init(InhulViewModelMainClass* klass)
{
	GObjectClass* gObjectClass = G_OBJECT_CLASS(klass);
	gObjectClass->get_property = inhul_view_model_main_get_value;
	gObjectClass->set_property = inhul_view_model_main_set_value;

	props[PROP_GROUPS] = g_param_spec_object(INHUL_VIEW_MODEL_MAIN_PROP_GROUPS,"Groups", "A collection of groups to display", GVM_TYPE_OBSERVABLE_COLLECTION, G_PARAM_READABLE);

	props[PROP_SELECTED_ITEM] = g_param_spec_object(INHUL_VIEW_MODEL_MAIN_PROP_SELECTED_ITEM, "Selected item", "Currently selected item", INHUL_VIEW_MODEL_TYPE_ITEM, G_PARAM_READWRITE);

	g_object_class_install_properties(gObjectClass, N_PROPS, props);
}

static void
inhul_view_model_main_init(InhulViewModelMain* this)
{
	this->groups = gvm_observable_collection_new();
	this->selectedItem = NULL;
}

InhulViewModelMain*
inhul_view_model_main_new()
{
	InhulViewModelMain* this = INHUL_VIEW_MODEL_MAIN(g_object_new(INHUL_VIEW_MODEL_TYPE_MAIN, NULL));

	return this;
}

/*InhulViewModelGroup*/ GvmObservableCollection*
inhul_view_model_main_get_groups (InhulViewModelMain* this)
{
	GvmObservableCollection* groups = NULL;

	g_object_get(G_OBJECT(this), INHUL_VIEW_MODEL_MAIN_PROP_GROUPS, &groups, NULL);

	return groups;
}

InhulViewModelItem*
inhul_view_model_main_get_selected_item(InhulViewModelMain* this)
{
	InhulViewModelItem* selectedItem = NULL;

	g_object_get(G_OBJECT(this), INHUL_VIEW_MODEL_MAIN_PROP_SELECTED_ITEM, &selectedItem, NULL);

	return selectedItem;
}

static void
inhul_view_model_main_on_child_selection (GObject* obj, GParamSpec* pspec, gpointer data)
{
	InhulViewModelItem* item = INHUL_VIEW_MODEL_ITEM(obj);
	InhulViewModelMain* this = INHUL_VIEW_MODEL_MAIN(data);

	if(inhul_view_model_item_get_is_selected(item))
	{
		if(this->selectedItem)
			inhul_view_model_item_set_is_selected(this->selectedItem, FALSE);

		g_object_set(G_OBJECT(this), INHUL_VIEW_MODEL_MAIN_PROP_SELECTED_ITEM, item, NULL);
	}
	else if(item == this->selectedItem)
	{
		g_object_set(G_OBJECT(this), INHUL_VIEW_MODEL_MAIN_PROP_SELECTED_ITEM, NULL, NULL);
	}
}

static void
inhul_view_model_main_subscibe_to_selection(InhulViewModelMain* this, InhulViewModelItem* item)
{
	InhulItemDataType itemType = inhul_view_model_item_get_item_type(item);

	if(itemType == INHUL_ITEM_DESKTOP_FILE)
	{
		g_signal_connect(G_OBJECT(item), "notify::is-selected", G_CALLBACK(inhul_view_model_main_on_child_selection), this);

		return;
	}

	/*InhulViewModelItem, Borrow*/GvmObservableCollection* children = inhul_view_model_item_get_children(item);
	GvmIterator* iter = gvm_observable_collection_get_iterator(children);

	while(gvm_iterator_move_next(iter))
	{
		InhulViewModelItem* childItem = INHUL_VIEW_MODEL_ITEM(gvm_iterator_get_current(iter));

		inhul_view_model_main_subscibe_to_selection(this, childItem);
	}

	gvm_iterator_free(iter);
}

void
inhul_view_model_main_read_items(InhulViewModelMain* this, const gchar* configFile, GError** err)
{
	GError* innerErr = NULL;
	g_assert(configFile);

	/*InhulItemGroup*/ GPtrArray* groups = inhul_item_data_load_from_json(configFile, &innerErr);

	if(innerErr != NULL)
	{
		g_propagate_error(err, innerErr);
		return;
	}

	for(gint i = 0; i < groups->len; i++)
	{
		InhulItemGroup* group = (InhulItemGroup*)groups->pdata[i];

		InhulViewModelGroup* groupVm = inhul_view_model_group_new(group);

		gvm_observable_collection_add(this->groups, groupVm);

		/*InhulViewModelItem*/GvmObservableCollection* groupChildren = inhul_view_model_group_get_children(groupVm);
		GvmIterator* iter = gvm_observable_collection_get_iterator(groupChildren);

		while(gvm_iterator_move_next(iter))
		{
			InhulViewModelItem* item = INHUL_VIEW_MODEL_ITEM(gvm_iterator_get_current(iter));

			inhul_view_model_main_subscibe_to_selection(this, item);
		}

		gvm_iterator_free(iter);
	}

	g_ptr_array_unref(groups);
}

void
inhul_view_model_main_generate_defaults(InhulViewModelMain* this, gint numberOfElemsInColumn)
{
	GPtrArray* items = inhul_item_data_generate_default();
    gint groupElemCount = numberOfElemsInColumn * 2 * 3;
	gint groupCount = (gint)ceil(items->len * 1.0 / groupElemCount);

	for(gint i = 0; i < groupCount; i++)
	{
		InhulItemGroup* group = g_new0(InhulItemGroup, 1);
		group->children = g_ptr_array_new();

		for(gint j = 0; j < 3; j++)
		{
			InhulItemData* tallItem = g_new(InhulItemData, 1);
			tallItem->type = INHUL_ITEM_CONTAINER;
			tallItem->level = ITEM_TALL;
			tallItem->children = g_ptr_array_new();

			for(gint k = 0; k < numberOfElemsInColumn; k++)
			{	
				gint index = i * groupElemCount + j * 2 * numberOfElemsInColumn + k * 2;

				if(index >= items->len)
				{
					g_ptr_array_add(group->children, tallItem);

					goto cycle_end;
				}

				InhulItemData* wideItem = g_new(InhulItemData, 1);
				wideItem->type = INHUL_ITEM_CONTAINER;
				wideItem->level = ITEM_WIDE;
				wideItem->children = g_ptr_array_new();
				
				InhulItemData* item = g_new(InhulItemData, 1);
				item->type = INHUL_ITEM_DESKTOP_FILE;
				item->level = ITEM_COMMON;
				item->desktopItemData = (InhulDesktopItemData*)items->pdata[index];

				g_ptr_array_add(wideItem->children, item);
				g_ptr_array_add(tallItem->children, wideItem);
				index++;

				if(index >= items->len)
				{
					g_ptr_array_add(group->children, tallItem);

					goto cycle_end;
				}

				item = g_new(InhulItemData, 1);
				item->type = INHUL_ITEM_DESKTOP_FILE;
				item->level = ITEM_COMMON;
				item->desktopItemData = (InhulDesktopItemData*)items->pdata[index];

				g_ptr_array_add(wideItem->children, item);
			}

			g_ptr_array_add(group->children, tallItem);
		}	

		cycle_end:
		InhulViewModelGroup* groupVm = inhul_view_model_group_new(group);

		/*InhulViewModelItem*/GvmObservableCollection* groupChildren = inhul_view_model_group_get_children(groupVm);
		GvmIterator* iter = gvm_observable_collection_get_iterator(groupChildren);

		while(gvm_iterator_move_next(iter))
		{
			InhulViewModelItem* childVm = INHUL_VIEW_MODEL_ITEM(gvm_iterator_get_current(iter));

			inhul_view_model_main_subscibe_to_selection(this, childVm);
		}

		gvm_iterator_free(iter);


		gvm_observable_collection_add(this->groups, groupVm);
	}

	g_ptr_array_unref(items);
}

void
inhul_view_model_main_save_config(InhulViewModelMain* this, const gchar* configFile, GError** error)
{
	g_assert(this);
	g_assert(configFile);

	GError* innerErr = NULL;
	GPtrArray* itemsToWrite = g_ptr_array_new();
	GvmIterator* iter = gvm_observable_collection_get_iterator(this->groups);

	while(gvm_iterator_move_next(iter))
	{
		InhulViewModelGroup* groupVm = INHUL_VIEW_MODEL_GROUP(gvm_iterator_get_current(iter));

		InhulItemGroup* group = inhul_view_model_group_get_group(groupVm);

		g_ptr_array_add(itemsToWrite, group);
	}

	inhul_item_data_save_data(configFile, itemsToWrite, &innerErr);

	if(innerErr != NULL)
	{
		g_propagate_error(error, innerErr);
	}

	g_ptr_array_unref(itemsToWrite);
}
