#include "gvm_iterator.h"

struct _GvmIterator
{
	gpointer impl;
	gpointer (*curr_impl)(gpointer);
	gboolean (*next_impl)(gpointer);
};

GvmIterator*
gvm_iterator_new(gpointer impl, gpointer (*curr_impl)(gpointer), gboolean (*next_impl)(gpointer))
{
	GvmIterator* this = g_new(GvmIterator, 1);

	this->impl = impl;
	this->curr_impl = curr_impl;
	this->next_impl = next_impl;

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
	g_free(this);
}

