#ifndef _INHUL_ITEM_DATA_JSON_H_
#define _INHUL_ITEM_DATA_JSON_H_

#include "inhul_item_data.h"

GPtrArray*
inhul_item_data_load_from_json(const gchar* jsonFile, GError** err);

#endif //_INHUL_ITEM_DATA_JSON_H_