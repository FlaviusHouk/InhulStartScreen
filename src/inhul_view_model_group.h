#ifndef _INHUL_VIEW_MODEL_GROUP_H_
#define _INHUL_VIEW_MODEL_GROUP_H_

#include "inhul_item_data.h"
#include "gvm_observable_collection.h"

#include <glib.h>

G_BEGIN_DECLS

#define INHUL_VIEW_MODEL_TYPE_GROUP inhul_view_model_group_get_type()
#define INHUL_VIEW_MODEL_GROUP_PROP_NAME "name"
#define INHUL_VIEW_MODEL_GROUP_PROP_CHILDREN "children"

G_DECLARE_FINAL_TYPE(InhulViewModelGroup, inhul_view_model_group, INHUL_VIEW_MODEL, GROUP, GObject);

InhulViewModelGroup*
inhul_view_model_group_new(InhulItemGroup* group);

const gchar*
inhul_view_model_group_get_name(InhulViewModelGroup* group);

void
inhul_view_model_group_set_name(InhulViewModelGroup* group, const gchar* name);

/*InhulViewModelItem*/GvmObservableCollection*
inhul_view_model_group_get_children(InhulViewModelGroup* group);

/*Bad design
 *TODO: refactor with some kind of streaming 
 *writer interface.
 * */
InhulItemGroup*
inhul_view_model_group_get_group(InhulViewModelGroup* this);

G_END_DECLS

#endif //_INHUL_VIEW_MODEL_GROUP_H_
