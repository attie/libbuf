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

EXPORT size_t buf_getSize(buf_t *buf) {
	buf_chunk_t **p;
	size_t c;

	if (!buf) {
		errno = EINVAL;
		return -1;
	}

	c = 0;
	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next)) {
		c += (*p)->size;
	}
	pthread_mutex_unlock(&(buf->mutex));

	return c;
}

EXPORT size_t buf_getLength(buf_t *buf) {
	buf_chunk_t **p;
	size_t c;

	if (!buf) {
		errno = EINVAL;
		return -1;
	}

	c = 0;
	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next)) {
		c += (*p)->len;
	}
	pthread_mutex_unlock(&(buf->mutex));

	return c;
}
