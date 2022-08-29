#ifndef _INHUL_SQ_CONTAINER_VIEW_MODEL_H_
#define _INHUL_SQ_CONTAINER_VIEW_MODEL_H_

#include "inhul_item_data.h"

#include <glib.h>

G_BEGIN_DECLS

typedef struct _InhulSqContainerViewModel InhulSqContainerViewModel;

InhulSqContainerViewModel*
inhul_sq_container_view_model_new();

InhulItemGroup*
inhul_sq_container_view_model_create_group(const InhulSqContainerViewModel* this, const gchar* name);

void
inhul_sq_container_view_model_remove_group(const InhulSqContainerViewModel* this, InhulItemGroup* group);

void
inhul_sq_container_view_model_get_group(const InhulSqContainerViewModel* this, gint idx);

gint
inhul_sq_container_view_model_get_group_count(const InhulSqContainerViewModel* this);

void
inhul_sq_container_view_model_on_group_added(const InhulSqContainerViewModel* this, void (*cb)(InhulSqContainerViewModel*, gpointer));

void
inhul_sq_container_view_model_free(InhulSqContainerViewModel* this);

G_END_DECLS

#endif //_INHUL_SQ_CONTAINER_VIEW_MODEL_H_
