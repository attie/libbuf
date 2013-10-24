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

EXPORT int buf_printf(buf_t *buf, const char *format, ...) {
	va_list ap;
	int ret;
	va_start(ap, format);
	ret = buf_vnprintf(buf, -1, format, ap);
	va_end(ap);
	return ret;
}

EXPORT int buf_nprintf(buf_t *buf, size_t size, const char *format, ...) {
	va_list ap;
	int ret;
	va_start(ap, format);
	ret = buf_vnprintf(buf, size, format, ap);
	va_end(ap);
	return ret;
}

EXPORT int buf_vprintf(buf_t *buf, const char *format, va_list ap) {
	return buf_vnprintf(buf, -1, format, ap);
}

EXPORT int buf_vnprintf(buf_t *buf, size_t size, const char *format, va_list ap) {
	va_list ap2;
	size_t len, len2;
	buf_chunk_t **p;
	buf_chunk_t *c;

	if (!buf || !format) {
		errno = EINVAL;
		return -1;
	}

	va_copy(ap2, ap);
	len = vsnprintf(NULL, 0, format, ap2);
	va_end(ap2);

	if (len < 0) {
		errno = ENOSPC;
		return -1;
	}
	if (size != -1 && size < len) len = size;

	if ((c = malloc(sizeof(*c) + len)) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	c->next = NULL;
	c->size = len;
	c->len = len;
	c->pos = 0;

	va_copy(ap2, ap);
	len2 = vsnprintf((char*)c->data, len, format, ap2);
	va_end(ap2);
	if (len < 0 || len != len2) {
		free(c);
		errno = ENOSPC;
		return -1;
	}

	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next));
	if (p) *p = c;
	pthread_mutex_unlock(&(buf->mutex));
	
	if (!p) {
		free(c);
		errno = EFAULT;
		return -1;
	}

	return len;
}

