#ifndef _DATA_ITEM_H_
#define _DATA_ITEM_H_

#include "item.h"
#include "command.h"

typedef struct _ItemData ItemData;

struct _ItemData
{
	InhulCommand* command;
	GString*      item_name;
	GString*      icon;
};

#define INHUL_TYPE_DATA_ITEM inhul_data_item_get_type()

G_DECLARE_DERIVABLE_TYPE(InhulDataItem, inhul_data_item, INHUL, DATA_ITEM, Item)

InhulDataItem*
inhul_data_item_new(ItemData* data);

#endif //_DATA_ITEM_H_
