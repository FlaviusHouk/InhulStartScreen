#include "inhul_item_data.h"

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <glib/gstdio.h>

static const gchar* localDesktopDir = "/.local/share/applications";
static const gchar* globalDesktopDir = "/usr/share/applications";

static GKeyFile*
inhul_item_data_load_file(const gchar* fullPath);

GKeyFile*
inhul_item_data_get_desktop_file(const gchar* desktopFileName)
{
	gchar* homeDir = NULL;
	gchar* fullPath = NULL;
	GKeyFile* keyFile = NULL;

	const gchar* homeDirFromEnv = getenv("HOME");

	if(homeDirFromEnv != NULL)
	{
		homeDir = g_strdup(homeDirFromEnv);
	}
	else
	{
		struct passwd* pwd = getpwuid(getuid());

		homeDir = g_strdup(pwd->pw_dir);
	}

	fullPath = g_build_filename(homeDir, localDesktopDir, desktopFileName, NULL);
	g_free(homeDir);

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

