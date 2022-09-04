#ifndef _GVM_OBSERVABLE_COLLECTION_H_
#define _GVM_OBSERVABLE_COLLECTION_H_

#include "gvm_iterator.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GVM_OBSERVABLE_COLLECTION_CHANGED_SIGNAL "changed"

typedef enum _GvmObservableCollectionOperation GvmCollectionOperation;

enum _GvmObservableCollectionOperation
{
	COLLECTION_ADD,
	COLLECTION_REMOVE,
	COLLECTION_CHANGE_POSITION,
	COLLECTION_CLEAR
};

typedef struct _GvmObservableCollectionChangedEventArgs GvmCollectionChangedArgs;

struct _GvmObservableCollectionChangedEventArgs
{
	GvmCollectionOperation operation;
	gint index;
	gpointer newItem;
	gpointer oldItem;
};

#define GVM_TYPE_OBSERVABLE_COLLECTION gvm_observable_collection_get_type()

G_DECLARE_FINAL_TYPE(GvmObservableCollection, gvm_observable_collection, GVM, OBSERVABLE_COLLECTION, GObject);

GvmObservableCollection*
gvm_observable_collection_new();

GvmObservableCollection*
gvm_observable_collection_new_with_data(GPtrArray* array);

void
gvm_observable_collection_add(GvmObservableCollection* this, gpointer item);

gpointer
gvm_observable_collection_get(const GvmObservableCollection* this, gint index);

void
gvm_observable_collection_set(GvmObservableCollection* this, gpointer item, gint index);

gint
gvm_observable_collection_get_length(const GvmObservableCollection* this);

gboolean
gvm_observable_collection_find(const GvmObservableCollection* this, gconstpointer item, guint* index);

GvmIterator*
gvm_observable_collection_get_iterator(const GvmObservableCollection* this);

void
gvm_observable_collection_clear(GvmObservableCollection* this);

void
gvm_observable_collection_remove(GvmObservableCollection* this, gpointer item);

G_END_DECLS

#endif //_GVM_OBSERVABLE_COLLECTION_H_
