#ifndef _INHUL_VIEW_MODEL_ITEM_H_
#define _INHUL_VIEW_MODEL_ITEM_H_

#include "gvm_observable_collection.h"
#include "inhul_item_data.h"

#include <glib.h>

G_BEGIN_DECLS

#define INHUL_VIEW_MODEL_ITEM_PROP_LEVEL "level"
#define INHUL_VIEW_MODEL_ITEM_PROP_IS_SELECTED "is-selected"

#define INHUL_VIEW_MODEL_TYPE_ITEM inhul_view_model_item_get_type()

G_DECLARE_FINAL_TYPE(InhulViewModelItem, inhul_view_model_item, INHUL_VIEW_MODEL, ITEM, GObject);

InhulViewModelItem*
inhul_view_model_item_new(InhulItemData* item);

/*InhulViewModelItem, Borrow*/
GvmObservableCollection*
inhul_view_model_item_get_children(InhulViewModelItem* this);

InhulItemLevel
inhul_view_model_item_get_level(InhulViewModelItem* this);

InhulItemDataType
inhul_view_model_item_get_item_type(InhulViewModelItem* this);

InhulDesktopItemData*
inhul_view_model_item_get_desktop_item_data(InhulViewModelItem* this);

gboolean
inhul_view_model_item_get_is_selected(InhulViewModelItem* this);

void
inhul_view_model_item_set_is_selected (InhulViewModelItem* this, gboolean value);

InhulCommand*
inhul_view_model_item_get_make_bigger_command(InhulViewModelItem* this);

InhulCommand*
inhul_view_model_item_get_make_smaller_command(InhulViewModelItem* this);

G_END_DECLS

#endif //_INHUL_VIEW_MODEL_ITEM_H_
