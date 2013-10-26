#ifndef INTERNAL_H
#define INTERNAL_H
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

#define LH     fprintf(stderr, "%s:%d %s() LOG HERE!\n", __FILE__, __LINE__, __FUNCTION__);
#define LHP(p) fprintf(stderr, "%s:%d %s() LOG_HERE! - %s = %p\n", __FILE__, __LINE__, __FUNCTION__, (#p), (p));

#define EXPORT __attribute__((visibility("default")))

#include <pthread.h>

extern const size_t SYS_PAGE_SIZE;

struct buf_chunk;
typedef struct buf_chunk buf_chunk_t;
struct buf;
typedef struct buf buf_t;

#include "buf.h"

struct buf_chunk {
	struct buf_chunk *next;
	size_t size; /* <-- size of the data element */
	size_t len;  /* <-- length of the populated data */
	size_t pos;  /* <-- offset to beginning of the populated data */
	uint8_t data[0];
};

struct buf {
	buf_chunk_t *head;
	pthread_mutex_t mutex;
};

/* the mutex must be held when calling this function */
buf_chunk_t *_buf_get_space(buf_t *buf, size_t size, void **retData, size_t **retLen);

#endif /* INTERNAL_H */
