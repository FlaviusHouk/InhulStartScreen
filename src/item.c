#include "item.h"
#include "command.h"

typedef struct _InhulItemPrivate InhulItemPrivate;

struct _InhulItemPrivate
{
	InhulCommand* command;
	ItemSizeEnum size;
};

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(InhulItem, inhul_item, GTK_TYPE_BIN);

static void
inhul_item_class_init(InhulItemClass* klass)
{}

static void
inhul_item_init(InhulItem* this)
{}
