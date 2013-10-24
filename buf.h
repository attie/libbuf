#ifndef _BUF_H
#define _BUF_H
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

#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#ifndef EXPORT
#define EXPORT
#endif

struct buf;
typedef struct buf buf_t;

EXPORT buf_t *buf_alloc(void);
EXPORT void buf_clear(buf_t *buf);
EXPORT void buf_free(buf_t *buf);

EXPORT size_t buf_getSize(buf_t *buf);
EXPORT size_t buf_getLength(buf_t *buf);

EXPORT int buf_putc(buf_t *buf, int c);
EXPORT int buf_getc(buf_t *buf);

EXPORT size_t buf_write(buf_t *buf, const uint8_t *data, size_t count);
EXPORT size_t buf_read(buf_t *buf, uint8_t *data, size_t count);

EXPORT int buf_printf(buf_t *buf, const char *format, ...);
EXPORT int buf_nprintf(buf_t *buf, size_t size, const char *format, ...);
EXPORT int buf_vprintf(buf_t *buf, const char *format, va_list ap);
EXPORT int buf_vnprintf(buf_t *buf, size_t size, const char *format, va_list ap);

#endif /* _BUF_H */
