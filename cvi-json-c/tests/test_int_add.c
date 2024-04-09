#include <assert.h>
#include <stdio.h>

#include "cvi_json.h"

int main(int argc, char **argv)
{
	cvi_json_object *tmp = cvi_json_object_new_int(123);
	cvi_json_object_int_inc(tmp, 123);
	assert(cvi_json_object_get_int(tmp) == 246);
	cvi_json_object_put(tmp);
	printf("INT ADD PASSED\n");
	tmp = cvi_json_object_new_int(INT32_MAX);
	cvi_json_object_int_inc(tmp, 100);
	assert(cvi_json_object_get_int(tmp) == INT32_MAX);
	assert(cvi_json_object_get_int64(tmp) == (int64_t)INT32_MAX + 100L);
	cvi_json_object_put(tmp);
	printf("INT ADD OVERFLOW PASSED\n");
	tmp = cvi_json_object_new_int(INT32_MIN);
	cvi_json_object_int_inc(tmp, -100);
	assert(cvi_json_object_get_int(tmp) == INT32_MIN);
	assert(cvi_json_object_get_int64(tmp) == (int64_t)INT32_MIN - 100L);
	cvi_json_object_put(tmp);
	printf("INT ADD UNDERFLOW PASSED\n");
	tmp = cvi_json_object_new_int64(321321321);
	cvi_json_object_int_inc(tmp, 321321321);
	assert(cvi_json_object_get_int(tmp) == 642642642);
	cvi_json_object_put(tmp);
	printf("INT64 ADD PASSED\n");
	tmp = cvi_json_object_new_int64(INT64_MAX);
	cvi_json_object_int_inc(tmp, 100);
	assert(cvi_json_object_get_int64(tmp) == INT64_MAX);
	assert(cvi_json_object_get_uint64(tmp) == (uint64_t)INT64_MAX + 100U);
	cvi_json_object_int_inc(tmp, -100);
	assert(cvi_json_object_get_int64(tmp) == INT64_MAX);
	assert(cvi_json_object_get_uint64(tmp) == (uint64_t)INT64_MAX);
	cvi_json_object_put(tmp);
	printf("INT64 ADD OVERFLOW PASSED\n");
	tmp = cvi_json_object_new_int64(INT64_MIN);
	cvi_json_object_int_inc(tmp, -100);
	assert(cvi_json_object_get_int64(tmp) == INT64_MIN);
	cvi_json_object_int_inc(tmp, 100);
	assert(cvi_json_object_get_int64(tmp) != INT64_MIN);
	cvi_json_object_put(tmp);
	printf("INT64 ADD UNDERFLOW PASSED\n");
	// uint64 + negative int64--> negative int64
	tmp = cvi_json_object_new_uint64(400);
	cvi_json_object_int_inc(tmp, -200);
	assert(cvi_json_object_get_int64(tmp) == 200);
	assert(cvi_json_object_get_uint64(tmp) == 200);
	cvi_json_object_int_inc(tmp, 200);
	assert(cvi_json_object_get_int64(tmp) == 400);
	assert(cvi_json_object_get_uint64(tmp) == 400);
	cvi_json_object_put(tmp);
	printf("UINT64 ADD PASSED\n");
	tmp = cvi_json_object_new_uint64(UINT64_MAX-50);
	cvi_json_object_int_inc(tmp, 100);
	assert(cvi_json_object_get_int64(tmp) == INT64_MAX);
	assert(cvi_json_object_get_uint64(tmp) == UINT64_MAX);
	cvi_json_object_put(tmp);
	printf("UINT64 ADD OVERFLOW PASSED\n");
	tmp = cvi_json_object_new_uint64(100);
	cvi_json_object_int_inc(tmp, -200);
	assert(cvi_json_object_get_int64(tmp) == -100);
	assert(cvi_json_object_get_uint64(tmp) == 0);
	cvi_json_object_put(tmp);
	printf("UINT64 ADD UNDERFLOW PASSED\n");

	printf("PASSED\n");
	return 0;
}
