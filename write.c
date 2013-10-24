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

EXPORT size_t buf_write(buf_t *buf, const uint8_t *data, size_t count) {
	buf_chunk_t **p;
	buf_chunk_t *c;

	if (!buf || !data) {
		errno = EINVAL;
		return -1;
	}
	if (count == 0) return 0;

	if ((c = malloc(sizeof(*c) + count)) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	c->next = NULL;
	c->size = count;
	c->len = count;
	c->pos = 0;
	memcpy(c->data, data, count);

	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next));
	if (p) *p = c;
	pthread_mutex_unlock(&(buf->mutex));

	if (!p) {
		free(c);
		errno = EFAULT;
		return -1;
	}

	return count;
}
