#ifndef _COMMAND_H_
#define _COMMAND_H_

typedef struct _InhulCommand InhulCommand;

InhulCommand*
inhul_command_new_invoke_file(GString* fileName);

void
inhul_command_execute(InhulCommand* this);

gboolean
inhul_command_can_execute(InhulCommand* this);

#endif //_COMMAND_H_
