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
