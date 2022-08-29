#include "gvm_observable_collection.h"

static guint changedSignal;

typedef struct _GvmObservableCollectionIterator GvmObservableCollectionIterator;

struct _GvmObservableCollectionIterator
{
	gint index;
	gint originalCollectionLength;
	const GvmObservableCollection* collection;
};

struct _GvmObservableCollection
{
	GObject base;
	GPtrArray* array;
};

G_DEFINE_FINAL_TYPE(GvmObservableCollection, gvm_observable_collection, G_TYPE_OBJECT);

static gpointer
gvm_observable_collection_iterator_current(gpointer iter)
{
	GvmObservableCollectionIterator* iterImpl = (GvmObservableCollectionIterator*)iter;

	GPtrArray* arr = iterImpl->collection->array;

	if(arr->len != iterImpl->originalCollectionLength)
	{
		g_error("Collection was modified while iterating.");
		g_assert(0);
	}

	if(iterImpl->index >= arr->len)
		return NULL;

	return arr->pdata[iterImpl->index];
}

static gboolean
gvm_observable_collection_iterator_move_next(gpointer iter) 
{
	GvmObservableCollectionIterator* iterImpl = (GvmObservableCollectionIterator*)iter;

	GPtrArray* arr = iterImpl->collection->array;

	return iterImpl->index++ < arr->len;
}

static void
gvm_observable_collection_class_init(GvmObservableCollectionClass* klass)
{
	changedSignal =
		g_signal_new(GVM_OBSERVABLE_COLLECTION_CHANGED_SIGNAL, G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void
gvm_observable_collection_init(GvmObservableCollection* this)
{
	this->array = NULL;
}

GvmObservableCollection*
gvm_observable_collection_new()
{
	GvmObservableCollection* this = g_object_new(GVM_TYPE_OBSERVABLE_COLLECTION, NULL);

	this->array = g_ptr_array_new();

	return this;
}

GvmObservableCollection*
gvm_observable_collection_new_with_data(GPtrArray* array)
{
	GvmObservableCollection* this = g_object_new(GVM_TYPE_OBSERVABLE_COLLECTION, NULL);

	this->array = array;

	return this;
}

void
gvm_observable_collection_add(GvmObservableCollection* this, gpointer item)
{
	g_ptr_array_add(this->array, item);

	GvmCollectionChangedArgs args =
	{
		.operation = COLLECTION_ADD,
		.index = this->array->len-1,
		.oldItem = NULL,
		.newItem = item
	};

	g_signal_emit(this, changedSignal, 0, &args);
}

gpointer
gvm_observable_collection_get(const GvmObservableCollection* this, gint index)
{
	return this->array->pdata[index];
}

gint
gvm_observable_collection_get_length(const GvmObservableCollection* this)
{
	return this->array->len;
}

GvmIterator*
gvm_observable_collection_get_iterator(const GvmObservableCollection* this)
{
	GvmObservableCollectionIterator* iterImpl = g_new(GvmObservableCollectionIterator, 1);

	iterImpl->index = 0;
	iterImpl->originalCollectionLength = this->array->len;
	iterImpl->collection = this;

	GvmIterator* iter = gvm_iterator_new(iterImpl, gvm_observable_collection_iterator_current, gvm_observable_collection_iterator_move_next);

	return iter;
}

void
gvm_observable_collection_clear(GvmObservableCollection* this)
{
	g_ptr_array_set_size(this->array, 0);

	GvmCollectionChangedArgs args =
	{
		.operation = COLLECTION_CLEAR,
		.index = 0,
		.oldItem = NULL,
		.newItem = NULL
	};

	g_signal_emit(this, changedSignal, 0, &args);
}

void
gvm_observable_collection_remove(GvmObservableCollection* this, gpointer item)
{
	gint index = -1;
	if(!g_ptr_array_find(this->array, item, &index))
		return;

	g_ptr_array_remove(this->array, item);

	GvmCollectionChangedArgs args =
	{
		.operation = COLLECTION_REMOVE,
		.index = index,
		.oldItem = item,
		.newItem = NULL
	};

	g_signal_emit(this, changedSignal, 0, &args);
}
