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

EXPORT int buf_printf(buf_t *buf, int flags, const char *format, ...) {
	va_list ap;
	int ret;
	va_start(ap, format);
	ret = buf_vnprintf(buf, flags, -1, format, ap);
	va_end(ap);
	return ret;
}

EXPORT int buf_nprintf(buf_t *buf, int flags, size_t size, const char *format, ...) {
	va_list ap;
	int ret;
	va_start(ap, format);
	ret = buf_vnprintf(buf, flags, size, format, ap);
	va_end(ap);
	return ret;
}

EXPORT int buf_vprintf(buf_t *buf, int flags, const char *format, va_list ap) {
	return buf_vnprintf(buf, flags, -1, format, ap);
}

EXPORT int buf_vnprintf(buf_t *buf, int flags, size_t size, const char *format, va_list ap) {
	va_list ap2;
	size_t size2;

	void *d;
	size_t *len;
	buf_chunk_t *c;

	int ret;

	if (!buf || !format) {
		errno = EINVAL;
		return -1;
	}

	va_copy(ap2, ap);
	size2 = vsnprintf(NULL, 0, format, ap2);
	va_end(ap2);

	if (size2 < 0) {
		errno = ENOSPC;
		return -1;
	}
	if (size == -1 || size > size2) size = size2;

	pthread_mutex_lock(&(buf->mutex));

	/* size + 1 to give space for the nul */
	if ((c = _buf_get_space(buf, size + 1, &d, &len)) == NULL) {
		errno = ENOMEM;
	} else {
		va_copy(ap2, ap);
		ret = vsnprintf((char*)d, size, format, ap2);
		va_end(ap2);

		/* if ret != size, then either
		     the output was truncated (bad)
		   or 
		     the output was shorter than before (also bad - perhaps some data changed in the meantime?) */
		if (ret < 0 || ret != size) {
			errno = ECANCELED;
			c = NULL;
		} else {
			*len += ret;
		}
	}

	pthread_mutex_unlock(&(buf->mutex));
	
	/* c == NULL indicates an error, and errno should have been set appropriately */
	if (!c) return -1;

	if (!(flags & BUF_MORE)) _buf_signal(buf);

	return ret;
}

