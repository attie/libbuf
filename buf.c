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

EXPORT buf_t *buf_alloc(void) {
	buf_t *p;

	if ((p = malloc(sizeof(*p))) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	memset(p, 0, sizeof(*p));

	if (pthread_mutex_init(&(p->mutex), NULL) != 0) {
		free(p);
		errno = ENOTRECOVERABLE;
		return NULL;
	}

	if (pthread_cond_init(&(p->cond), NULL) != 0) {
		pthread_mutex_destroy(&(p->mutex));
		free(p);
		return NULL;
	}

	return p;
}

EXPORT void buf_purge(buf_t *buf) {
	buf_chunk_t *p, *q;

	if (!buf) return;

	pthread_mutex_lock(&(buf->mutex));

	while (buf->head) {
		p = buf->head;
		buf->head = p->next;
		free(p);
	}

	pthread_mutex_unlock(&(buf->mutex));
}

EXPORT void buf_free(buf_t *buf) {
	if (!buf) return;

	buf_purge(buf);

	pthread_mutex_destroy(&(buf->mutex));
	pthread_cond_destroy(&(buf->cond));

	free(buf);
}

buf_chunk_t *_buf_get_space(buf_t *buf, size_t size, void **retData, size_t **retLen) {
	buf_chunk_t **p;
	buf_chunk_t *c;
	size_t msize;

	if (retData) *retData = NULL;
	if (retLen)  *retLen = NULL;
	
	/* get to the last chunk */
	for (c = NULL, p = &(buf->head); p && *p; c = *p, p = &(*p)->next);
	if (!p) return NULL; /* <-- this is an error */

	if (c) {
		/* does this chunk have enough space to add the new data? */
		if ((c->size - (c->pos + c->len)) >= size) {
			if (retData) *retData = &(c->data[c->pos + c->len]);
			if (retLen)  *retLen  = &(c->len);
			return c;
		}
	}

	msize = sizeof(*c) + size;
	if (msize < SYS_PAGE_SIZE) {
		size += SYS_PAGE_SIZE - msize;
		msize = SYS_PAGE_SIZE;
	}
	if ((c = malloc(msize)) == NULL) return NULL;

	c->next = NULL;
	c->size = size;
	c->len = 0;
	c->pos = 0;

	*p = c;

	if (retData) *retData = &(c->data[0]);
	if (retLen)  *retLen  = &(c->len);

	return c;
}

size_t _buf_get_data(buf_t *buf, void *data, size_t count) {
	size_t remain;

	remain = count;

	while (buf->head != NULL && (remain > 0 || buf->head->len == 0)) {
		if (buf->head->len <= 0) {
			buf_chunk_t *p;
			p = buf->head;
			buf->head = p->next;
			free(p);
		} else {
			size_t tmpCount;

			tmpCount = remain >= buf->head->len ? buf->head->len : remain;

			memcpy(data, &(buf->head->data[buf->head->pos]), tmpCount);
			data += tmpCount;

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
