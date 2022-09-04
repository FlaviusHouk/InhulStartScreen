#include "inhul_item_data_json.h"
#include "gvm_iterator.h"

#include <json-glib/json-glib.h>

InhulItemGroup*
inhul_item_data_build(JsonObject* jsonGroup);

InhulItemData*
inhul_item_data_build_item(JsonObject* jsonItem, InhulItemLevel level);

static InhulItemDataType
inhul_item_data_parse_type(const gchar* type);

GPtrArray*
inhul_item_data_load_from_json(const gchar* jsonFile, GError** err)
{
	GError* innerErr = NULL;

	JsonParser* parser = json_parser_new();
	if(!json_parser_load_from_file(parser, jsonFile, &innerErr))
	{
		g_propagate_prefixed_error(err, innerErr, "Cannot load config.");

		return NULL;
	}

	JsonNode* root = json_parser_get_root(parser);
	JsonObject* rootObj = json_node_get_object(root);
	JsonArray* jsonGroups = json_object_get_array_member(rootObj, "Groups");

	GPtrArray* groups = g_ptr_array_new();
	int groupCount = json_array_get_length(jsonGroups);
	for(guint i = 0; i < groupCount; i++)
	{
		JsonObject* group = json_array_get_object_element(jsonGroups, i);

		InhulItemGroup* groupItem = inhul_item_data_build(group);

		g_ptr_array_add(groups, groupItem);
	}

	g_object_unref(G_OBJECT(parser));

	return groups;
}

static void
inhul_item_data_save_item(InhulItemData* item, JsonBuilder* builder)
{
	json_builder_begin_object(builder);

	if(item->type == INHUL_ITEM_DESKTOP_FILE)
	{
		json_builder_set_member_name(builder, "Type");
		json_builder_add_string_value(builder, "Desktop");

		json_builder_set_member_name(builder, "FileName");
		json_builder_add_string_value(builder, item->desktopItemData->desktopFileName);
	}
	else
	{
		json_builder_set_member_name(builder, "Children");
		json_builder_begin_array(builder);

		GvmIterator* iter = g_ptr_array_get_iterator(item->children);

		while(gvm_iterator_move_next(iter))
		{
			gpointer child = gvm_iterator_get_current(iter);

			InhulItemData* childItem = (InhulItemData*)child;
			inhul_item_data_save_item(childItem, builder);
		}

		g_free(iter);

		json_builder_end_array(builder);
	}

	json_builder_end_object(builder);
}

static void
inhul_item_data_save_group(InhulItemGroup* group, JsonBuilder* builder)
{
	json_builder_begin_object(builder);

	json_builder_set_member_name(builder, "Type");
	json_builder_add_string_value(builder, "Group");

	json_builder_set_member_name(builder, "Name");
	json_builder_add_string_value(builder, group->name);

	json_builder_set_member_name(builder, "Items");
	json_builder_begin_array(builder);

	GvmIterator* iter = g_ptr_array_get_iterator(group->children);

	while(gvm_iterator_move_next(iter))
	{
		InhulItemData* childItem = (InhulItemData*)gvm_iterator_get_current(iter);
		inhul_item_data_save_item(childItem, builder);
	}

	json_builder_end_array(builder);

	json_builder_end_object(builder);

	g_free(iter);
}

void
inhul_item_data_save_data(const gchar* jsonFile, GPtrArray* items, GError** err)
{
	GError* innErr = NULL;

	JsonBuilder* builder = json_builder_new();

	json_builder_begin_object(builder);

	json_builder_set_member_name(builder, "Groups");
	json_builder_begin_array(builder);

	GvmIterator* iter = g_ptr_array_get_iterator(items);

	while(gvm_iterator_move_next(iter))
	{
		InhulItemGroup* group = (InhulItemGroup*)gvm_iterator_get_current(iter);

		inhul_item_data_save_group(group, builder);
	}

	json_builder_end_array(builder);
	json_builder_end_object(builder);

	JsonNode* root = json_builder_get_root(builder);

	JsonGenerator* generator = json_generator_new();
	json_generator_set_root(generator, root);
	json_generator_set_pretty(generator, TRUE);

	if(!json_generator_to_file(generator, jsonFile, &innErr))
	{
		g_error("%s\n", innErr->message);
		g_assert(0);
	}

	g_object_unref(G_OBJECT(builder));
	g_object_unref(G_OBJECT(generator));
	g_free(iter);
}

InhulItemGroup*
inhul_item_data_build(JsonObject* node)
{
	InhulItemGroup* group = g_new(InhulItemGroup, 1);

	group->name = g_strdup(json_object_get_string_member(node, "Name"));
	group->children = g_ptr_array_new();

	JsonArray* children = json_object_get_array_member(node, "Items");
	guint childrenCount = json_array_get_length(children);
	for(guint i = 0; i < childrenCount; i++)
	{
		JsonObject* jsonItem = json_array_get_object_element(children, i);
		InhulItemData* item = inhul_item_data_build_item(jsonItem, ITEM_TALL);

		g_ptr_array_add(group->children, item);
	}

	return group;
}

InhulItemData*
inhul_item_data_build_item(JsonObject* jsonItem, InhulItemLevel level)
{
	InhulItemData* item = g_new(InhulItemData, 1);

	item->level = level;

	const gchar* type = json_object_get_string_member_with_default(jsonItem, "Type", "Container");

	item->type = inhul_item_data_parse_type(type);	

	if(item->type == INHUL_ITEM_CONTAINER)
	{
		JsonArray* jsonChildren = json_object_get_array_member(jsonItem, "Children");
		GPtrArray* children = g_ptr_array_new();
		guint childrenCount = json_array_get_length(jsonChildren);
		for(guint i = 0; i<childrenCount; i++)
		{
			JsonObject* jsonChild = json_array_get_object_element(jsonChildren, i);

			InhulItemData* childItem = inhul_item_data_build_item(jsonChild, level + 1);

			g_ptr_array_add(children, childItem);
		}

		item->children = children;
	}
	else if(item->type == INHUL_ITEM_DESKTOP_FILE)
	{
		const gchar* desktopFileName = json_object_get_string_member(jsonItem, "FileName");

		item->desktopItemData = inhul_item_data_parse_desktop_file(desktopFileName);
	}

	return item;
}

static InhulItemDataType
inhul_item_data_parse_type(const gchar* type)
{
	if(g_strcmp0(type, "Desktop") == 0)
		return INHUL_ITEM_DESKTOP_FILE;
	else if(g_strcmp0(type, "Container") == 0)
		return INHUL_ITEM_CONTAINER;

	g_assert(FALSE);
}

