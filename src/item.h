#ifndef _ITEM_H_
#define _ITEM_H_

#include <gtk/gtk.h>

typedef enum _ItemSizeEnum ItemSizeEnum;

enum _ItemSizeEnum
{
	ItemSizeEnum_Small,
	ItemSizeEnum_Medium,
	ItemSizeEnum_MediumWide,
	ItemSizeEnum_Large
};

#define INHUL_TYPE_ITEM inhul_item_get_type()

G_DECLARE_DERIVABLE_TYPE(InhulItem, inhul_item, INHUL, ITEM, GtkBin)

struct _InhulItemClass
{
	GtkBinClass base;

	GString*   (*get_title)  (InhulItem*);
	GdkPixbuf* (*get_icon)   (InhulItem*);
	GdkRGBA    (*get_bg)     (InhulItem*);
};

InhulItem*
inhul_item_new();

#endif //_ITEM_H_
