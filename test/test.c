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

#include "test.h"

struct test *tests[];

int run_tests(int *count) {
	int x,y;
	int fails = 0;
	if (count) *count = 0;

	for (x = 0; tests[x] != NULL; x++) {
		for (y = 0; tests[x][y].func != NULL; y++) {
			int ret, value;
			printf("running '%s' registered on %s:%d...\n",
			       tests[x][y].name, tests[x][y].file, tests[x][y].line);
			if (count) (*count)++;
			value = 0;
			if ((ret = tests[x][y].func(&value)) != 0) {
				printf("  --> *** '%s' returned %d, and passed back 0x%08X / %d  ***\n",
				       tests[x][y].name, ret, value, value);
				fails++;
				return fails;
			}
		}
	}
	return fails;
}

extern struct test init_tests[];
extern struct test threadded_tests[];

struct test *tests[] = {
	init_tests,
	threadded_tests,
	NULL
};
