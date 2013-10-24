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

#include "test.h"

#include "../buf.h"

int init_1(int *value) {
	buf_t *b;

	if ((b = buf_alloc()) == NULL) {
		return 1;
	}

	buf_free(b);

	return 0;
}

int init_2(int *value) {
	buf_t *b;
	char t[10];

	if ((b = buf_alloc()) == NULL) {
		return 1;
	}

	if (buf_write(b, "Hello", 5) != 5) {
		return 2;
	}

	if (buf_read(b, t, sizeof(t)) != 5) {
		return 3;
	}

	if (strncmp(t, "Hello", 5)) {
		return 4;
	}

	buf_free(b);

	return 0;
}

int init_3(int *value) {
	buf_t *b;
	char t[10];

	if ((b = buf_alloc()) == NULL) {
		return 1;
	}

	if (buf_write(b, "Hi", 2) != 2) {
		return 2;
	}
	if (buf_write(b, " ", 1) != 1) {
		return 3;
	}
	if (buf_write(b, "You", 3) != 3) {
		return 4;
	}

	if (buf_read(b, t, sizeof(t)) != 6) {
		return 5;
	}

	if (strncmp(t, "Hi You", 6)) {
		return 6;
	}

	buf_free(b);

	return 0;
}

struct test init_tests[] = {
	ADD_FUNC(init_1),
	ADD_FUNC(init_2),
	ADD_FUNC(init_3),
	END_FUNCS(),
};
