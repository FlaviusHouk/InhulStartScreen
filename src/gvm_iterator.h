#ifndef _GVM_ITERATOR_H_
#define _GVM_ITERATOR_H_

#include <glib.h>

G_BEGIN_DECLS;

typedef struct _GvmIterator GvmIterator;

GvmIterator*
gvm_iterator_new(gpointer impl, gpointer (*curr_impl)(gpointer), gboolean (*next_impl)(gpointer), void (*free)(gpointer));

gpointer
gvm_iterator_get_current(const GvmIterator* this);

gboolean
gvm_iterator_move_next(const GvmIterator* this);

void
gvm_iterator_free(GvmIterator* this);

GvmIterator*
g_ptr_array_get_iterator(GPtrArray* arr);

G_END_DECLS;

#endif //_GVM_ITERATOR_H_
