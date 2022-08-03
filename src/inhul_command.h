#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <gio/gio.h>

typedef struct _InhulCommand InhulCommand;
typedef struct _InhulExecStringCommandData InhulExecStringCommandData;

struct _InhulExecStringCommandData
{
	const gchar* execStr;
	GApplication* app;
};

InhulCommand*
inhul_command_new_print_exec_string(InhulExecStringCommandData* data);

void
inhul_command_execute(const InhulCommand* this);

gboolean
inhul_command_can_execute(const InhulCommand* this);

void
inhul_command_free(InhulCommand* this);

#endif //_COMMAND_H_
