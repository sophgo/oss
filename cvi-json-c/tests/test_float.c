/* Copyright (C) 2016 by Rainer Gerhards
 * Released under ASL 2.0 */
#include "config.h"
#include "cvi_json_object.h"
#include "cvi_json_tokener.h"
#include <stdio.h>
int main(void)
{
	cvi_json_object *cvi_json;

	cvi_json = cvi_json_object_new_double(1.0);
	printf("cvi_json = %s\n", cvi_json_object_to_cvi_json_string_ext(cvi_json, JSON_C_TO_STRING_PRETTY));
	cvi_json_object_put(cvi_json);

	cvi_json = cvi_json_object_new_double(-1.0);
	printf("cvi_json = %s\n", cvi_json_object_to_cvi_json_string_ext(cvi_json, JSON_C_TO_STRING_PRETTY));
	cvi_json_object_put(cvi_json);
	cvi_json = cvi_json_object_new_double(1.23);
	printf("cvi_json = %s\n", cvi_json_object_to_cvi_json_string_ext(cvi_json, JSON_C_TO_STRING_PRETTY));
	cvi_json_object_put(cvi_json);
	cvi_json = cvi_json_object_new_double(123456789.0);
	printf("cvi_json = %s\n", cvi_json_object_to_cvi_json_string_ext(cvi_json, JSON_C_TO_STRING_PRETTY));
	cvi_json_object_put(cvi_json);
	cvi_json = cvi_json_object_new_double(123456789.123);
	printf("cvi_json = %s\n", cvi_json_object_to_cvi_json_string_ext(cvi_json, JSON_C_TO_STRING_PRETTY));
	cvi_json_object_put(cvi_json);
	return 0;
}
