#include "inhul_command.h"

typedef enum _InhulCommandType InhulCommandType;

enum _InhulCommandType
{
	INHUL_COMMAND_PRINT_EXEC_STRING
};

struct _InhulCommand
{
	InhulCommandType type;
	gpointer data;
};


InhulCommand*
inhul_command_new_print_exec_string(InhulExecStringCommandData* data)
{
	InhulCommand* command = g_new0(InhulCommand, 1);

	command->type = INHUL_COMMAND_PRINT_EXEC_STRING;
	command->data = data;

	return command;
}

void
inhul_command_execute(const InhulCommand* this)
{
	g_assert(this);
	g_assert(this->type == INHUL_COMMAND_PRINT_EXEC_STRING);

	InhulExecStringCommandData* data = (InhulExecStringCommandData*)this->data; 
	g_print("%s\n", data->execStr);
	g_application_release(data->app);
}

gboolean
inhul_command_can_execute(const InhulCommand* this)
{
	return this->type==INHUL_COMMAND_PRINT_EXEC_STRING;
}

void
inhul_command_free(InhulCommand* this)
{
	g_free(this->data);
	g_free(this);
}
