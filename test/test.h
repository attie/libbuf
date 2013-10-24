#ifndef TEST_H
#define TEST_H
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

int run_tests(int *count);

typedef int (*test_func)(int *value);

struct test {
	test_func func;
	char *name;
	char *file;
	int line;
};

#define ADD_FUNC(f) { (f), #f, __FILE__, __LINE__ }
#define END_FUNCS() { NULL, NULL, NULL, 0 }

#endif /* TEST_H */
