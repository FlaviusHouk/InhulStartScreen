#include "inhul_item_data_json.h"

#include <json-glib/json-glib.h>

InhulItemGroup*
inhul_item_data_build(JsonObject* jsonGroup);

InhulItemData*
inhul_item_data_build_item(JsonObject* jsonItem);

static InhulItemDataType
inhul_item_data_parse_type(const gchar* type);

static InhulDesktopItemData*
inhul_item_data_parse_desktop_file(const gchar* desktopFile);

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
		InhulItemData* item = inhul_item_data_build_item(jsonItem);

		g_ptr_array_add(group->children, item);
	}

	return group;
}

InhulItemData*
inhul_item_data_build_item(JsonObject* jsonItem)
{
	InhulItemData* item = g_new(InhulItemData, 1);

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

			InhulItemData* childItem = inhul_item_data_build_item(jsonChild);

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

static InhulDesktopItemData*
inhul_item_data_parse_desktop_file(const gchar* desktopFileName)
{
	GError* error = NULL;
	InhulDesktopItemData* item = g_new0(InhulDesktopItemData, 1);

	GKeyFile* desktopFile = inhul_item_data_get_desktop_file(desktopFileName);

	if(desktopFile == NULL)
	{
		g_print("Cannot file desktop file %s.\n", desktopFileName);
		g_assert(0);
	}

	item->title=g_key_file_get_locale_string(desktopFile, "Desktop Entry", "Name", NULL,  &error);

	if(error != NULL)
	{
		g_print("Cannot get title from %s. Having %s.\n", desktopFileName, error->message);

		g_assert(0);
	}

	item->description=g_key_file_get_locale_string(desktopFile, "Desktop Entry", "Comment", NULL, &error);

	if(error != NULL)
	{
		g_print("Cannot get description from %s. Having %s.\n", desktopFileName, error->message);

		g_assert(0);
	}

	item->icon_name=g_key_file_get_locale_string(desktopFile, "Desktop Entry", "Icon", NULL, &error);

	if(error != NULL)
	{
		g_print("Cannot get icon from %s. Having %s.\n", desktopFileName, error->message);

		g_assert(0);
	}

	const gchar* execString = g_key_file_get_string(desktopFile, "Desktop Entry", "Exec", &error);

	if(error != NULL)
	{
		g_print("Cannot get exec string from %s. Having %s.\n", desktopFileName, error->message);

		g_assert(0);
	}

	InhulExecStringCommandData* data = g_new(InhulExecStringCommandData, 1);
	data->execStr = execString;
	data->app = g_application_get_default();
	item->command = inhul_command_new_print_exec_string(data);
	
	g_key_file_unref(desktopFile);

	return item;
}

