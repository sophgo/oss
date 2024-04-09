/*
 * Tests if casting within the cvi_json_object_get_* functions work correctly.
 * Also checks the cvi_json_object_get_type and cvi_json_object_is_type functions.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi_json_inttypes.h"
#include "cvi_json_object.h"
#include "cvi_json_tokener.h"
#include "cvi_json_util.h"

static void getit(struct cvi_json_object *new_obj, const char *field);
static void checktype_header(void);
static void checktype(struct cvi_json_object *new_obj, const char *field);

int main(int argc, char **argv)
{
	const char *input = "{\n\
		\"string_of_digits\": \"123\",\n\
		\"regular_number\": 222,\n\
		\"decimal_number\": 99.55,\n\
		\"boolean_true\": true,\n\
		\"boolean_false\": false,\n\
		\"int64_number\": 2147483649,\n\
		\"negative_number\": -321321321,\n\
		\"a_null\": null,\n\
		\"empty_array\": [],\n\
		\"nonempty_array\": [ 123 ],\n\
		\"array_with_zero\": [ 0 ],\n\
		\"empty_object\": {},\n\
		\"nonempty_object\": { \"a\": 123 },\n\
	}";
	/* Note: 2147483649 = INT_MAX + 2 */
	/* Note: 9223372036854775809 = INT64_MAX + 2 */
	/* Note: 18446744073709551617 = UINT64_MAX + 2 */

	struct cvi_json_object *new_obj;

	new_obj = cvi_json_tokener_parse(input);
	printf("Parsed input: %s\n", input);
	printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
	if (!new_obj)
		return 1; // oops, we failed.

	getit(new_obj, "string_of_digits");
	getit(new_obj, "regular_number");
	getit(new_obj, "decimal_number");
	getit(new_obj, "boolean_true");
	getit(new_obj, "boolean_false");
	getit(new_obj, "int64_number");
	getit(new_obj, "negative_number");
	getit(new_obj, "a_null");
	getit(new_obj, "empty_array");
	getit(new_obj, "nonempty_array");
	getit(new_obj, "array_with_zero");
	getit(new_obj, "empty_object");
	getit(new_obj, "nonempty_object");

	// Now check the behaviour of the cvi_json_object_is_type() function.
	printf("\n================================\n");
	checktype_header();
	checktype(new_obj, NULL);
	checktype(new_obj, "string_of_digits");
	checktype(new_obj, "regular_number");
	checktype(new_obj, "decimal_number");
	checktype(new_obj, "boolean_true");
	checktype(new_obj, "boolean_false");
	checktype(new_obj, "int64_number");
	checktype(new_obj, "negative_number");
	checktype(new_obj, "a_null");

	cvi_json_object_put(new_obj);

	return 0;
}

static void getit(struct cvi_json_object *new_obj, const char *field)
{
	struct cvi_json_object *o = NULL;
	if (!cvi_json_object_object_get_ex(new_obj, field, &o))
		printf("Field %s does not exist\n", field);

	enum cvi_json_type o_type = cvi_json_object_get_type(o);
	printf("new_obj.%s cvi_json_object_get_type()=%s\n", field, cvi_json_type_to_name(o_type));
	printf("new_obj.%s cvi_json_object_get_int()=%d\n", field, cvi_json_object_get_int(o));
	printf("new_obj.%s cvi_json_object_get_int64()=%" PRId64 "\n", field, cvi_json_object_get_int64(o));
	printf("new_obj.%s cvi_json_object_get_uint64()=%" PRIu64 "\n", field,
	       cvi_json_object_get_uint64(o));
	printf("new_obj.%s cvi_json_object_get_boolean()=%d\n", field, cvi_json_object_get_boolean(o));
	printf("new_obj.%s cvi_json_object_get_double()=%f\n", field, cvi_json_object_get_double(o));
}

static void checktype_header()
{
	printf("cvi_json_object_is_type: %s,%s,%s,%s,%s,%s,%s\n", cvi_json_type_to_name(cvi_json_type_null),
	       cvi_json_type_to_name(cvi_json_type_boolean), cvi_json_type_to_name(cvi_json_type_double),
	       cvi_json_type_to_name(cvi_json_type_int), cvi_json_type_to_name(cvi_json_type_object),
	       cvi_json_type_to_name(cvi_json_type_array), cvi_json_type_to_name(cvi_json_type_string));
}
static void checktype(struct cvi_json_object *new_obj, const char *field)
{
	struct cvi_json_object *o = new_obj;
	if (field && !cvi_json_object_object_get_ex(new_obj, field, &o))
		printf("Field %s does not exist\n", field);

	printf("new_obj%s%-18s: %d,%d,%d,%d,%d,%d,%d\n", field ? "." : " ", field ? field : "",
	       cvi_json_object_is_type(o, cvi_json_type_null), cvi_json_object_is_type(o, cvi_json_type_boolean),
	       cvi_json_object_is_type(o, cvi_json_type_double), cvi_json_object_is_type(o, cvi_json_type_int),
	       cvi_json_object_is_type(o, cvi_json_type_object), cvi_json_object_is_type(o, cvi_json_type_array),
	       cvi_json_object_is_type(o, cvi_json_type_string));
}
