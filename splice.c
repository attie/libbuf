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

EXPORT size_t buf_splice(buf_t *dest, buf_t *src, int flags) {
	size_t count;

	if (!dest || !src) {
		errno = EINVAL;
		return -1;
	}

	pthread_mutex_lock(&(dest->mutex));
	pthread_mutex_lock(&(src->mutex));

	/* find out how many bytes we are taking from src */
	if (src->head == NULL || (count = _buf_getLength(src)) == 0) goto die;

	/* link up the chains */
	if (dest->tail != NULL) {
		src->head->prev = dest->tail;
		dest->tail->next = src->head;
		dest->tail = src->tail;
	} else {
		dest->head = src->head;
		dest->tail = src->tail;
	}

	src->head = NULL;
	src->tail = NULL;

	if (!(flags & BUF_MORE)) {
		_buf_signal(dest);
	}

die:
	pthread_mutex_unlock(&(dest->mutex));
	pthread_mutex_unlock(&(src->mutex));

	if (flags & BUF_GIFT) {
		buf_free(src);
	}

	return count;
}
