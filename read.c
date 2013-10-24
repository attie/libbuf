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

EXPORT size_t buf_read(buf_t *buf, uint8_t *data, size_t count) {
	buf_chunk_t **p;
	size_t remain;
	size_t tmpCount;

	if (!buf || !data) {
		errno = EINVAL;
		return -1;
	}
	if (count == 0) return 0;

	remain = count;

	pthread_mutex_lock(&(buf->mutex));

	while (buf->head != NULL && (remain > 0 || buf->head->len == 0)) {
		if (buf->head->len <= 0) {
			void *p;
			p = buf->head;
			buf->head = buf->head->next;
			free(p);
		} else {
			tmpCount = remain >= buf->head->len ? buf->head->len : remain;

			memcpy(data, &(buf->head->data[buf->head->pos]), tmpCount);
			data += tmpCount;

			remain -= tmpCount;
			buf->head->pos += tmpCount;
			buf->head->len -= tmpCount;
		}
	}

	pthread_mutex_unlock(&(buf->mutex));

	return count - remain;
}

