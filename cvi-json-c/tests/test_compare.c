/*
* Tests if cvi_json_object_equal behaves correct.
*/

#include "config.h"
#include <stdio.h>
#include <string.h>

#include "cvi_json_inttypes.h"
#include "cvi_json_object.h"

int main()
{
	/* integer tests */
	struct cvi_json_object *int1 = cvi_json_object_new_int(0);
	struct cvi_json_object *int2 = cvi_json_object_new_int(1);
	struct cvi_json_object *int3 = cvi_json_object_new_int(1);
	struct cvi_json_object *int4 = cvi_json_object_new_int(-1);
	struct cvi_json_object *uint1 = cvi_json_object_new_uint64(0);
	struct cvi_json_object *uint2 = cvi_json_object_new_uint64(1);
	struct cvi_json_object *uint3 = cvi_json_object_new_uint64(1);
	struct cvi_json_object *uint4 = cvi_json_object_new_uint64((uint64_t)INT64_MAX + 100);

	if (!cvi_json_object_equal(int1, int2))
		printf("JSON integer comparison is correct\n");
	else
		printf("JSON integer comparison failed\n");

	if (cvi_json_object_equal(int1, int1))
		printf("JSON same object comparison is correct\n");
	else
		printf("JSON same object comparison failed\n");

	if (cvi_json_object_equal(int2, int3))
		printf("JSON same integer comparison is correct\n");
	else
		printf("JSON same integer comparison failed\n");

	if (!cvi_json_object_equal(uint1, uint2))
		printf("JSON usigned integer comparison is correct\n");
	else
		printf("JSON usigned integer comparison failed\n");

	if (cvi_json_object_equal(uint1, uint1))
		printf("JSON same usigned object comparison is correct\n");
	else
		printf("JSON same usigned object comparison failed\n");

	if (cvi_json_object_equal(uint2, uint3))
		printf("JSON same usigned integer comparison is correct\n");
	else
		printf("JSON same usigned integer comparison failed\n");

	if (cvi_json_object_equal(int2, uint2))
		printf("JSON integer & usigned integer comparison is correct\n");
	else
		printf("JSON integer & usigned integer comparison failed\n");

	if (!cvi_json_object_equal(int2, uint4))
		printf("JSON integer & usigned integer comparison is correct\n");
	else
		printf("JSON integer & usigned integer comparison failed\n");

	if (!cvi_json_object_equal(int4, uint2))
		printf("JSON integer & usigned integer comparison is correct\n");
	else
		printf("JSON integer & usigned integer comparison failed\n");

	if (!cvi_json_object_equal(int4, uint4))
		printf("JSON integer & usigned integer comparison is correct\n");
	else
		printf("JSON integer & usigned integer comparison failed\n");

	if (cvi_json_object_equal(uint2, int2))
		printf("JSON usigned integer & integer comparison is correct\n");
	else
		printf("JSON usigned integer & integer comparison failed\n");

	if (!cvi_json_object_equal(uint2, int4))
		printf("JSON usigned integer & integer comparison is correct\n");
	else
		printf("JSON usigned integer & integer comparison failed\n");

	if (!cvi_json_object_equal(uint4, int2))
		printf("JSON usigned integer & integer comparison is correct\n");
	else
		printf("JSON usigned integer & integer comparison failed\n");

	if (!cvi_json_object_equal(uint4, int4))
		printf("JSON usigned integer & integer comparison is correct\n");
	else
		printf("JSON usigned integer & integer comparison failed\n");

	cvi_json_object_put(int1);
	cvi_json_object_put(int2);
	cvi_json_object_put(int3);
	cvi_json_object_put(int4);
	cvi_json_object_put(uint1);
	cvi_json_object_put(uint2);
	cvi_json_object_put(uint3);
	cvi_json_object_put(uint4);

	/* string tests */
	struct cvi_json_object *str1 = cvi_json_object_new_string("TESTSTRING");
	struct cvi_json_object *str2 = cvi_json_object_new_string("TESTSTRING");
	struct cvi_json_object *str3 = cvi_json_object_new_string("DIFFERENT");

	if (cvi_json_object_equal(str1, str2))
		printf("Comparing equal strings is correct\n");
	else
		printf("Comparing equal strings failed\n");

	if (!cvi_json_object_equal(str1, str3))
		printf("Comparing different strings is correct\n");
	else
		printf("Comparing different strings failed\n");

	cvi_json_object_put(str1);
	cvi_json_object_put(str2);
	cvi_json_object_put(str3);

	/* double tests */
	struct cvi_json_object *dbl1 = cvi_json_object_new_double(3.14159);
	struct cvi_json_object *dbl2 = cvi_json_object_new_double(3.14159);
	struct cvi_json_object *dbl3 = cvi_json_object_new_double(3.0);

	if (cvi_json_object_equal(dbl1, dbl2))
		printf("Comparing equal doubles is correct\n");
	else
		printf("Comparing equal doubles failed\n");

	if (!cvi_json_object_equal(dbl1, dbl3))
		printf("Comparing different doubles is correct\n");
	else
		printf("Comparing different doubles failed\n");

	cvi_json_object_put(dbl1);
	cvi_json_object_put(dbl2);
	cvi_json_object_put(dbl3);

	/* array tests */
	struct cvi_json_object *ar1 = cvi_json_object_new_array();
	struct cvi_json_object *ar2 = cvi_json_object_new_array();
	struct cvi_json_object *ar3 = cvi_json_object_new_array();
	struct cvi_json_object *ar4 = cvi_json_object_new_array();

	cvi_json_object_array_add(ar1, cvi_json_object_new_int(1));
	cvi_json_object_array_add(ar1, cvi_json_object_new_int(2));

	cvi_json_object_array_add(ar2, cvi_json_object_new_int(1));
	cvi_json_object_array_add(ar2, cvi_json_object_new_int(2));

	cvi_json_object_array_add(ar3, cvi_json_object_new_int(1));
	cvi_json_object_array_add(ar3, cvi_json_object_new_int(1));

	if (cvi_json_object_equal(ar1, ar2))
		printf("Comparing equal arrays is correct\n");
	else
		printf("Comparing equal arrays failed\n");

	cvi_json_object_array_add(ar2, cvi_json_object_new_int(1));
	if (!cvi_json_object_equal(ar1, ar2))
		printf("Comparing arrays of different len is correct\n");
	else
		printf("Comparing arrays of different len failed\n");

	if (!cvi_json_object_equal(ar1, ar3))
		printf("Comparing different arrays is correct\n");
	else
		printf("Comparing different arrays failed\n");

	if (!cvi_json_object_equal(ar1, ar4))
		printf("Comparing different arrays (one empty) is correct\n");
	else
		printf("Comparing different arrays (one empty) failed\n");

	cvi_json_object_put(ar1);
	cvi_json_object_put(ar2);
	cvi_json_object_put(ar3);
	cvi_json_object_put(ar4);

	/* object tests */
	struct cvi_json_object *obj1 = cvi_json_object_new_object();
	struct cvi_json_object *obj2 = cvi_json_object_new_object();

	cvi_json_object_object_add(obj1, "test1", cvi_json_object_new_int(123));
	cvi_json_object_object_add(obj1, "test2", cvi_json_object_new_int(321));
	cvi_json_object_object_add(obj1, "test3", cvi_json_object_new_int(320));
	cvi_json_object_object_add(obj1, "test4", cvi_json_object_new_int(319));
	cvi_json_object_object_add(obj1, "test5", cvi_json_object_new_int(318));

	cvi_json_object_object_add(obj2, "test5", cvi_json_object_new_int(318));
	cvi_json_object_object_add(obj2, "test4", cvi_json_object_new_int(319));
	cvi_json_object_object_add(obj2, "test3", cvi_json_object_new_int(320));
	cvi_json_object_object_add(obj2, "test2", cvi_json_object_new_int(321));
	cvi_json_object_object_add(obj2, "test1", cvi_json_object_new_int(123));

	/* key-order is different between obj1 and obj2, should still be equal */
	if (cvi_json_object_equal(obj1, obj2))
		printf("Comparing JSON object with different key order is correct\n");
	else
		printf("Comparing JSON object with different key order is incorrect\n");

	/* make obj2 look different to obj1 */
	cvi_json_object_object_add(obj2, "test3", cvi_json_object_new_int(234));
	if (!cvi_json_object_equal(obj1, obj2))
		printf("Comparing different objects is correct\n");
	else
		printf("Comparing different objects is incorrect\n");

	/* iterate over jso2 keys to see if any exist that are not in jso1 */
	cvi_json_object_object_add(obj2, "test3", cvi_json_object_new_int(320));
	cvi_json_object_object_add(obj2, "test6", cvi_json_object_new_int(321));
	if (!cvi_json_object_equal(obj1, obj2))
		printf("Comparing different objects is correct\n");
	else
		printf("Comparing different objects is incorrect\n");

	/* iterate over jso1 keys and see if they exist in jso1 */
	cvi_json_object_object_add(obj1, "test6", cvi_json_object_new_int(321));
	if (cvi_json_object_equal(obj1, obj2))
		printf("Comparing different objects is correct\n");
	else
		printf("Comparing different objects is incorrect\n");
	cvi_json_object_object_add(obj1, "test7", cvi_json_object_new_int(322));
	if (!cvi_json_object_equal(obj1, obj2))
		printf("Comparing different objects is correct\n");
	else
		printf("Comparing different objects is incorrect\n");

	cvi_json_object_put(obj1);
	cvi_json_object_put(obj2);

	/* different types tests */
	struct cvi_json_object *int5 = cvi_json_object_new_int(0);
	struct cvi_json_object *dbl5 = cvi_json_object_new_double(3.14159);

	if (!cvi_json_object_equal(int5, NULL))
		printf("JSON integer and NULL comparison is correct\n");
	else
		printf("JSON integer and NULL comparison failed\n");

	if (!cvi_json_object_equal(NULL, dbl5))
		printf("JSON NULL and double comparison is correct\n");
	else
		printf("JSON NULL and double comparison failed\n");

	if (!cvi_json_object_equal(int5, dbl5))
		printf("JSON integer and double comparison is correct\n");
	else
		printf("JSON integer and double comparison failed\n");

	cvi_json_object_put(int5);
	cvi_json_object_put(dbl5);

	return 0;
}
