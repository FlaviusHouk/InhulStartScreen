#include "inhul_command.h"

typedef enum _InhulCommandType InhulCommandType;

enum _InhulCommandType
{
	INHUL_COMMAND_PRINT_EXEC_STRING,
	INHUL_COMMAND_CUSTOM
};

struct _InhulCommand
{
	InhulCommandType type;

	void (*execute)(gpointer data);
	gboolean (*can_execute)(gpointer data);
	void (*free_data)(gpointer);
	gpointer data;
};

static void
print_exec_string_execute(gpointer data)
{
	InhulExecStringCommandData* execData = (InhulExecStringCommandData*)data;

	g_print("%s\n", execData->execStr);
	g_application_release(execData->app);
}

static gboolean
can_execute_always(gpointer data)
{
	return TRUE;
}

InhulCommand*
inhul_command_new_print_exec_string(InhulExecStringCommandData* data)
{
	InhulCommand* command = g_new0(InhulCommand, 1);

	command->type = INHUL_COMMAND_PRINT_EXEC_STRING;
	command->data = data;
	command->execute = print_exec_string_execute;
	command->can_execute = can_execute_always;
	command->free_data = g_free;

	return command;
}

InhulCommand*
inhul_command_new_custom(void (*execute)(gpointer data), gboolean (*can_execute)(gpointer data), gpointer data, void (*free_func)(gpointer data))
{
	InhulCommand* command = g_new0(InhulCommand, 1);

	command->type = INHUL_COMMAND_CUSTOM;
	command->data = data;
	command->execute = execute;
	command->can_execute = can_execute;
	command->free_data = free_func;

	return command;
}

void
inhul_command_execute(const InhulCommand* this)
{
	g_assert(this);

	this->execute(this->data);
}

gboolean
inhul_command_can_execute(const InhulCommand* this)
{
	g_assert(this);

	return this->can_execute(this->data);
}

void
inhul_command_free(InhulCommand* this)
{
	g_assert(this);

	if(this->free_data)
		this->free_data(this->data);

	g_free(this);
}
