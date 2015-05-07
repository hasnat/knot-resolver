/*  Copyright (C) 2015 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tests/test.h"
#include "lib/generic/array.h"

mm_ctx_t global_mm;

static void test_array(void **state)
{
	int ret = 0;
	array_t(int) arr;
	array_init(arr);

	/* Basic access */
	assert_int_equal(arr.len, 0);
	assert_int_equal(array_push(arr, 5), 0);
	assert_int_equal(arr.at[0], 5);
	assert_int_equal(array_tail(arr), 5);
	array_clear(arr);

	/* Reserve capacity and fill. */
	assert_true(array_reserve(arr, 5) >= 0);
	for (unsigned i = 0; i < 100; ++i) {
		ret = array_push(arr, i);
		assert_true(ret >= 0);
	}

	/* Make sure reservation holds. */
	assert_true(array_reserve(arr, 5) >= 0);

	/* Delete elements. */
	array_del(arr, 0);
	for (size_t i = arr.len; --i;) {
		ret = array_pop(arr);
		assert_true(ret == 0);
	}

	/* Overfill. */
	for (unsigned i = 0; i < 4096; ++i) {
		ret = array_push(arr, i);
		assert_true(ret >= 0);
	}

	array_clear(arr);
}

/** Reservation through tracked memory allocator. */
static int test_reserve(void *baton, char **mem, size_t elm_size, size_t want, size_t *have)
{
	if (want > *have) {
		void *new_mem = mm_alloc(baton, elm_size * want);
		if (*mem != NULL) {
			memcpy(new_mem, *mem, (*have) * elm_size);
			mm_free(baton, *mem);
		}
		*mem = new_mem;
		*have = want;
	}

	return 0;
}

/** Reservation through fake memory allocator. */
static int fake_reserve(void *baton, char **mem, size_t elm_size, size_t want, size_t *have)
{
	return -1;
}

static void test_array_mm(void **state)
{
	array_t(int) arr;
	array_init(arr);

	/* Reserve using fake memory allocator. */
	assert_false(array_reserve_mm(arr, 5, fake_reserve, NULL) >= 0);

	/* Reserve capacity and fill. */
	assert_true(array_reserve_mm(arr, 100, test_reserve, &global_mm) >= 0);
	for (unsigned i = 0; i < 100; ++i) {
		int ret = array_push(arr, i);
		assert_true(ret >= 0);
	}

	array_clear_mm(arr, mm_free, &global_mm);

}

int main(void)
{
	test_mm_ctx_init(&global_mm);

	const UnitTest tests[] = {
		unit_test(test_array),
		unit_test(test_array_mm)
	};

	return run_tests(tests);
}