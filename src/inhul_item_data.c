#include "inhul_item_data.h"

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <glib/gstdio.h>

static const gchar* localDesktopDir = "/.local/share/applications";
static const gchar* globalDesktopDir = "/usr/share/applications";

#define DESKTOP_EXTENSION_LEN 8

static GKeyFile*
inhul_item_data_load_file(const gchar* fullPath);

static void
inhul_item_data_fill_items_collection(const gchar* folder, GPtrArray* items);

GKeyFile*
inhul_item_data_get_desktop_file(const gchar* desktopFileName)
{
	const gchar* homeDir = g_get_home_dir();
	gchar* fullPath = NULL;
	GKeyFile* keyFile = NULL;

	fullPath = g_build_filename(homeDir, localDesktopDir, desktopFileName, NULL);

	if(g_access(fullPath, F_OK) == 0)
	{
		keyFile = inhul_item_data_load_file(fullPath);
		g_free(fullPath);
		return keyFile;
	}

	g_free(fullPath);
	fullPath = g_build_filename(globalDesktopDir, desktopFileName, NULL);

	if(g_access(fullPath, F_OK) == 0)
	{
		keyFile = inhul_item_data_load_file(fullPath);
		g_free(fullPath);
		return keyFile;
	}

	return keyFile;
}

static GKeyFile*
inhul_item_data_load_file(const gchar* fullPath)
{
	GError* err = NULL;
	GKeyFile* keyFile = g_key_file_new();

	if(!g_key_file_load_from_file(keyFile, fullPath, G_KEY_FILE_NONE, &err))
	{
		g_print(err->message);
		g_assert(0);
	}

	return keyFile;
}

InhulDesktopItemData*
inhul_item_data_parse_desktop_file(const gchar* desktopFileName)
{
	GError* error = NULL;
	InhulDesktopItemData* item = g_new0(InhulDesktopItemData, 1);
	item->desktopFileName = g_strdup(desktopFileName);

	GKeyFile* desktopFile = inhul_item_data_get_desktop_file(desktopFileName);

	if(desktopFile == NULL)
	{
		g_print("Cannot file desktop file %s.\n", desktopFileName);
		g_assert(0);
	}

	gboolean noDisplay = g_key_file_get_boolean(desktopFile, "Desktop Entry", "NoDisplay", &error);

	if(error != NULL)
	{
		if(!g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
		{
			g_print("Cannot get no display flag from %s. Having %s.\n", desktopFileName, error->message);

			g_assert(0);
		}

		noDisplay = FALSE;
		g_error_free(error);
		error = NULL;
	}

	if(noDisplay)
	{
		return NULL;
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
		g_print("domain: %d, key: %d\n", G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND);
		
		if(!g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
		{
			g_print("Cannot get description from %s. Having %s.\n", desktopFileName, error->message);

			g_assert(0);
		}

		item->description = NULL;
		g_error_free(error);
		error = NULL;
	}

	item->icon_name=g_key_file_get_locale_string(desktopFile, "Desktop Entry", "Icon", NULL, &error);

	if(error != NULL)
	{
		if(!g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
		{
			g_print("Cannot get icon from %s. Having %s.\n", desktopFileName, error->message);

			g_assert(0);
		}

		item->icon_name=NULL;
		g_error_free(error);
		error = NULL;
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

GPtrArray*
inhul_item_data_generate_default()
{
	GError* err = NULL;
	GPtrArray* items = g_ptr_array_new();
	const gchar* homeDir = g_get_home_dir();

	gchar* localAppsFolderPath = g_build_path(G_DIR_SEPARATOR_S, homeDir, localDesktopDir, NULL);
	inhul_item_data_fill_items_collection(localAppsFolderPath, items);

	inhul_item_data_fill_items_collection(globalDesktopDir, items);

	g_free(localAppsFolderPath);

	return items;
}

static void
inhul_item_data_fill_items_collection(const gchar* dirPath, GPtrArray* items)
{
	GError* err = NULL;

	GDir* dir = g_dir_open(dirPath, 0, &err);
	if(err != NULL)
	{
		g_print("%s\n", err->message);
		g_assert(0);
	}

	const gchar* child;
	while((child = g_dir_read_name(dir)))
	{
		gint len = strlen(child);

		if(len < DESKTOP_EXTENSION_LEN)
			continue;

		const gchar* extension = child + (len - DESKTOP_EXTENSION_LEN);

		if(strcmp(extension, ".desktop") != 0)
			continue;

		InhulDesktopItemData* desktopItem = inhul_item_data_parse_desktop_file(child);

		if(desktopItem == NULL)
			continue;

		g_ptr_array_add(items, desktopItem);
	}
}
