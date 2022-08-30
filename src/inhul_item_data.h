#ifndef _INHUL_ITEM_DATA_H_
#define _INHUL_ITEM_DATA_H_

#include "inhul_command.h"
#include "gvm_observable_collection.h"

#include <glib.h>

G_BEGIN_DECLS

#define STACK_DEPTH 4

typedef enum _InhulItemDataType InhulItemDataType;
typedef enum _InhulItemLevel InhulItemLevel;

typedef struct _InhulItemData InhulItemData;
typedef struct _InhulDesktopItemData InhulDesktopItemData;
typedef struct _InhulItemDataStackItem InhulItemDataStackItem;
typedef struct _InhulItemGroup InhulItemGroup;

struct _InhulDesktopItemData
{
	const gchar* desktopFileName;
	const gchar* title;
	const gchar* description;
	const gchar* icon_name;
	InhulCommand* command;
};

enum _InhulItemDataType
{
	INHUL_ITEM_CONTAINER,
	INHUL_ITEM_DESKTOP_FILE
};

enum _InhulItemLevel
{
	ITEM_TALL,
	ITEM_WIDE,
	ITEM_COMMON,
	ITEM_SMALL
};

struct _InhulItemData
{
	InhulItemDataType type;
	InhulItemLevel level;
	union
	{
		GvmObservableCollection* children; /*Container*/
		InhulDesktopItemData* desktopItemData;
	};
};

struct _InhulItemDataStackItem
{
	InhulItemData* item;
	int idx;
};

struct _InhulItemGroup
{
	const gchar* name;
	/*InhulItemData*/ GvmObservableCollection* children;
};

GKeyFile*
inhul_item_data_get_desktop_file(const gchar* desktopFileName);

InhulDesktopItemData*
inhul_item_data_parse_desktop_file(const gchar* desktopFileName);

GPtrArray*
inhul_item_data_generate_default();

G_END_DECLS

#endif //_INHUL_ITEM_DATA_H_
