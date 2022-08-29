#ifndef _INHUL_SQ_CONTAINER_H_
#define _INHUL_SQ_CONTAINER_H_

#include "gvm_container.h"

G_BEGIN_DECLS

#define INHUL_TYPE_SQ_CONTAINER inhul_sq_container_get_type()

G_DECLARE_FINAL_TYPE(InhulSqContainer, inhul_sq_container, INHUL, SQ_CONTAINER, GvmContainer)

InhulSqContainer*
inhul_sq_container_new(); 

G_END_DECLS

#endif //_INHUL_SQ_CONTAINER_H_ 
