#include "gvm_iterator.h"

struct _GvmIterator
{
	gpointer impl;
	gpointer (*curr_impl)(gpointer);
	gboolean (*next_impl)(gpointer);
	void     (*free)(gpointer);
};

GvmIterator*
gvm_iterator_new(gpointer impl, gpointer (*curr_impl)(gpointer), gboolean (*next_impl)(gpointer), void (*free)(gpointer))
{
	GvmIterator* this = g_new(GvmIterator, 1);

	this->impl = impl;
	this->curr_impl = curr_impl;
	this->next_impl = next_impl;
	this->free = free;

	return this;
}

gpointer
gvm_iterator_get_current(const GvmIterator* this)
{
	return this->curr_impl(this->impl);
}

gboolean
gvm_iterator_move_next(const GvmIterator* this)
{
	return this->next_impl(this->impl);
}

void
gvm_iterator_free(GvmIterator* this)
{
	if(this->free)
		this->free(this->impl);

	g_free(this);
}

typedef struct
{
	GPtrArray* arr;
	guint originalCount;
	gint current;
} GPtrArrayIteratorImpl;

static gpointer
g_ptr_array_iterator_get_current(gpointer data)
{
	GPtrArrayIteratorImpl* impl = (GPtrArrayIteratorImpl*)data;

	if(impl->arr->len != impl->originalCount)
	{
		g_error("Number of elements has changed. Cannot iterate.");
		g_assert(FALSE);
	}

	if(impl->current >= impl->arr->len || impl->current < 0)
		return NULL;

	return impl->arr->pdata[impl->current];
}

static gboolean
g_ptr_array_iterator_move_next(gpointer data)
{
	GPtrArrayIteratorImpl* impl = (GPtrArrayIteratorImpl*)data;

	if(impl->arr->len != impl->originalCount)
	{
		g_error("Number of elements has changed. Cannot iterate.");
		g_assert(FALSE);
	}

	impl->current++;

	return impl->current < impl->arr->len;
}

GvmIterator*
g_ptr_array_get_iterator(GPtrArray* arr)
{
	GPtrArrayIteratorImpl* impl = g_new(GPtrArrayIteratorImpl, 1);

	impl->arr = arr;
	impl->originalCount = arr->len;
	impl->current = -1;

	return gvm_iterator_new(impl, g_ptr_array_iterator_get_current, g_ptr_array_iterator_move_next, g_free);
}
