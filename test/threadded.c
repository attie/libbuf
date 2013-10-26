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
#include <pthread.h>

#include "test.h"

#include "../buf.h"

void *threadded_1B(void *v) {
	buf_t *b = v;

	fprintf(stderr, "B: started, sleeping\n");

	sleep(1);

	fprintf(stderr, "B: writing\n");

	buf_write(b, "Hello", 5);

	fprintf(stderr, "B: written, ending\n");

	return NULL;
}
int threadded_1(int *value) {
	buf_t *b;
	pthread_t tid;

	size_t len;
	char buf[10];

	if ((b = buf_alloc(0)) == NULL) {
		return 1;
	}

	fprintf(stderr, "A: starting B\n");

	pthread_create(&tid, NULL, (void*(*)(void*))threadded_1B, (void*)b);

	fprintf(stderr, "A: read()\n");

	len = buf_read(b, buf, sizeof(buf));

	fprintf(stderr, "A: read() complete: %d bytes\n", len);
	fprintf(stderr, "A: read: >%.*s<\n", len, buf);

	pthread_cancel(tid);
	pthread_join(tid, NULL);

	if (len != 5) return 2;
	if (memcmp(buf, "Hello", len)) return 3;

	return 0;
}

void *threadded_2B(void *v) {
	buf_t *b = v;

	fprintf(stderr, "B: started, sleeping\n");

	sleep(1);

	fprintf(stderr, "B: writing\n");

	buf_write(b, "Hello", 5);

	fprintf(stderr, "B: written, ending\n");

	return NULL;
}
int threadded_2(int *value) {
	buf_t *b;
	pthread_t tid;

	size_t len;
	char buf[10];

	if ((b = buf_alloc(BUF_NONBLOCK)) == NULL) {
		return 1;
	}

	fprintf(stderr, "A: starting B\n");

	pthread_create(&tid, NULL, (void*(*)(void*))threadded_1B, (void*)b);

	fprintf(stderr, "A: read()\n");

	len = buf_read(b, buf, sizeof(buf));

	fprintf(stderr, "A: read() complete: %d bytes\n", len);
	fprintf(stderr, "A: read: >%.*s<\n", len, buf);

	pthread_cancel(tid);
	pthread_join(tid, NULL);

	if (len != 0) return 2;

	return 0;
}

struct test threadded_tests[] = {
	ADD_FUNC(threadded_1),
	ADD_FUNC(threadded_2),
	END_FUNCS(),
};
