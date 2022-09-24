#ifndef _INHUL_VIEW_MODEL_MAIN_H_
#define _INHUL_VIEW_MODEL_MAIN_H_

#include "inhul_view_model_item.h"

#include "gvm_observable_collection.h"

G_BEGIN_DECLS

#define INHUL_VIEW_MODEL_MAIN_PROP_GROUPS "groups"
#define INHUL_VIEW_MODEL_MAIN_PROP_SELECTED_ITEM "selected-item"

#define INHUL_VIEW_MODEL_TYPE_MAIN inhul_view_model_main_get_type()

G_DECLARE_FINAL_TYPE(InhulViewModelMain, inhul_view_model_main, INHUL_VIEW_MODEL, MAIN, GObject);

InhulViewModelMain*
inhul_view_model_main_new ();

/*InhulViewModelGroup*/ GvmObservableCollection*
inhul_view_model_main_get_groups (InhulViewModelMain* this);

InhulViewModelItem*
inhul_view_model_main_get_selected_item(InhulViewModelMain* this);

void
inhul_view_model_main_read_items(InhulViewModelMain* this, const gchar* configFiles, GError** err);

void
inhul_view_model_main_generate_defaults(InhulViewModelMain* this, gint numberOfElemsInColumn);

void
inhul_view_model_main_save_config(InhulViewModelMain* this, const gchar* configFile, GError** err);

G_END_DECLS

#endif //_INHUL_VIEW_MODEL_MAIN_H_
