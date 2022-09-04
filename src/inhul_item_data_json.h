#ifndef _INHUL_ITEM_DATA_JSON_H_
#define _INHUL_ITEM_DATA_JSON_H_

#include "inhul_item_data.h"

GPtrArray*
inhul_item_data_load_from_json(const gchar* jsonFile, GError** err);

void
inhul_item_data_save_data(const gchar* jsonFile, /*InhulItemGroup*/GPtrArray* items, GError** err);

#endif //_INHUL_ITEM_DATA_JSON_H_
