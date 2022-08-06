#ifndef _INHUL_ITEM_DATA_H_
#define _INHUL_ITEM_DATA_H_

#include "inhul_command.h"

#include <glib.h>

G_BEGIN_DECLS

#define STACK_DEPTH 4

#define ITEM_TALL   1
#define ITEM_WIDE   2
#define ITEM_COMMON 3
#define ITEM_SMALL  4

typedef struct _InhulItemData InhulItemData;
typedef struct _InhulItemGroup InhulItemGroup;
typedef enum _InhulItemDataType InhulItemDataType;
typedef struct _InhulDesktopItemData InhulDesktopItemData;
typedef struct _InhulItemDataStackItem InhulItemDataStackItem;

struct _InhulDesktopItemData
{
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

struct _InhulItemData
{
	InhulItemDataType type;
	union
	{
		GPtrArray* children; /*Container*/
		InhulDesktopItemData* desktopItemData;
	};
};

struct _InhulItemGroup
{
	const gchar* name;
	GPtrArray* children;
};

struct _InhulItemDataStackItem
{
	InhulItemData* item;
	int idx;
};

GKeyFile*
inhul_item_data_get_desktop_file(const gchar* desktopFileName);

void
inhul_item_data_traverse_group(const InhulItemGroup* group, void (*cb)(InhulItemData*, InhulItemDataStackItem*,gint,gpointer), gpointer data);

G_END_DECLS

#endif //_INHUL_ITEM_DATA_H_
