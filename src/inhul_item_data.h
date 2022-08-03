#ifndef _INHUL_ITEM_DATA_H_
#define _INHUL_ITEM_DATA_H_

#include "inhul_command.h"

#include <glib.h>

G_BEGIN_DECLS

typedef struct _InhulItemData InhulItemData;
typedef struct _InhulItemGroup InhulItemGroup;
typedef enum _InhulItemDataType InhulItemDataType;
typedef struct _InhulDesktopItemData InhulDesktopItemData;

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

GKeyFile*
inhul_item_data_get_desktop_file(const gchar* desktopFileName);

G_END_DECLS

#endif //_INHUL_ITEM_DATA_H_
