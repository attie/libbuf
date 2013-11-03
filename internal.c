/*
  libbuf - a C library that provides fast, flexible and efficient buffers

  Copyright (C) 2013 onwards  Attie Grande (attie@attie.co.uk)

  libbuf is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libbuf is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with libbuf. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

buf_chunk_t *_buf_get_space(buf_t *buf, size_t size, void **retData, size_t **retLen) {
	buf_chunk_t *t;
	size_t msize;

	if (retData) *retData = NULL;
	if (retLen)  *retLen = NULL;

	if (buf->tail) {
		buf_chunk_t *c = buf->tail;

		/* does this chunk have enough space to add the new data? */
		if ((c->size - (c->pos + c->len)) >= size) {
			if (retData) *retData = &(c->data[c->pos + c->len]);
			if (retLen)  *retLen  = &(c->len);
			return c;
		}

		/* as we are moving onto a new chunk, we can make this one smaller
		   we also don't care if this fails...
		*/
		if ((c = realloc(c, c->pos + c->len)) != NULL) {
			c->size = c->pos + c->len;
			buf->tail = c;
			if (c->prev) c->prev->next = c;
		}
	}

	msize = sizeof(*t) + size;
	if (msize < SYS_PAGE_SIZE) {
		size += SYS_PAGE_SIZE - msize;
		msize = SYS_PAGE_SIZE;
	}
	if ((t = malloc(msize)) == NULL) return NULL;

	t->next = NULL;
	t->prev = buf->tail;
	t->size = size;
	t->len = 0;
	t->pos = 0;

	if (!buf->head) buf->head = t;
	if (buf->tail)  buf->tail->next = t;
	buf->tail = t;

	if (retData) *retData = &(t->data[0]);
	if (retLen)  *retLen  = &(t->len);

	return t;
}

size_t _buf_get_data(buf_t *buf, void *data, size_t count) {
	size_t remain;

	remain = count;

	while (buf->head != NULL && (remain > 0 || buf->head->len == 0)) {
		if (buf->head->len == 0) {
			buf_chunk_t *p = buf->head;
			buf->head = p->next;
			if (buf->head) {
				buf->head->prev = NULL;
			} else if (buf->tail == p) {
				buf->tail = NULL;
			}
			free(p);
		} else {
			size_t tmpCount;

			tmpCount = remain >= buf->head->len ? buf->head->len : remain;

			if (data) {
				memcpy(data, &(buf->head->data[buf->head->pos]), tmpCount);
				data += tmpCount;
			}

			remain -= tmpCount;
			buf->head->pos += tmpCount;
			buf->head->len -= tmpCount;
		}
	}

	return count - remain;
}

int _buf_signal(buf_t *buf) {
	return pthread_cond_signal(&(buf->cond));
}

int _buf_wait(buf_t *buf) {
	return pthread_cond_wait(&(buf)->cond, &(buf)->mutex);
}

size_t _buf_getSize(buf_t *buf) {
	buf_chunk_t **p;
	size_t c;

	c = 0;
	for (p = &(buf->head); p && *p; p = &((*p)->next)) {
		c += (*p)->size;
	}

	return c;
}

size_t _buf_getLength(buf_t *buf) {
	buf_chunk_t **p;
	size_t c;

	c = 0;
	for (p = &(buf->head); p && *p; p = &((*p)->next)) {
		c += (*p)->len;
	}

	return c;
}
