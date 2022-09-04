#ifndef _INHUL_SQ_CONTAINER_GROUP_H_
#define _INHUL_SQ_CONTAINER_GROUP_H_

#include "gvm_container.h"
#include "inhul_view_model_group.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GROUP_WIDTH 12

#define INHUL_TYPE_SQ_CONTAINER_GROUP inhul_sq_container_group_get_type()

G_DECLARE_FINAL_TYPE(InhulSqContainerGroup, inhul_sq_container_group, INHUL, SQ_CONTAINER_GROUP, GvmContainer);

InhulSqContainerGroup*
inhul_sq_container_group_new(InhulViewModelGroup* group);

G_END_DECLS

#endif //_INHUL_SQ_CONTAINER_GROUP_H_
