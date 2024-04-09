#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "cvi_json.h"
#include "parse_flags.h"

static int sort_fn(const void *j1, const void *j2)
{
	cvi_json_object *const *jso1, *const *jso2;
	int i1, i2;

	jso1 = (cvi_json_object *const *)j1;
	jso2 = (cvi_json_object *const *)j2;
	if (!*jso1 && !*jso2)
		return 0;
	if (!*jso1)
		return -1;
	if (!*jso2)
		return 1;

	i1 = cvi_json_object_get_int(*jso1);
	i2 = cvi_json_object_get_int(*jso2);

	return i1 - i2;
}

#ifdef TEST_FORMATTED
static const char *to_cvi_json_string(cvi_json_object *obj, int flags)
{
	size_t length;
	char *copy;
	const char *result;

	result = cvi_json_object_to_cvi_json_string_length(obj, flags, &length);
	copy = strdup(result);
	if (copy == NULL)
		printf("to_cvi_json_string: Allocation failed!\n");
	else
	{
		result = cvi_json_object_to_cvi_json_string_ext(obj, flags);
		if (length != strlen(result))
			printf("to_cvi_json_string: Length mismatch!\n");
		if (strcmp(copy, result) != 0)
			printf("to_cvi_json_string: Comparison Failed!\n");
		free(copy);
	}
	return result;
}
#define cvi_json_object_to_cvi_json_string(obj) to_cvi_json_string(obj, sflags)
#else
/* no special define */
#endif

cvi_json_object *make_array(void);
cvi_json_object *make_array()
{
	cvi_json_object *my_array;

	my_array = cvi_json_object_new_array();
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(1));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(2));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(3));
	cvi_json_object_array_put_idx(my_array, 4, cvi_json_object_new_int(5));
	cvi_json_object_array_put_idx(my_array, 3, cvi_json_object_new_int(4));
	cvi_json_object_array_put_idx(my_array, 6, cvi_json_object_new_int(7));

	return my_array;
}

void test_array_del_idx(void);
void test_array_del_idx()
{
	int rc;
	size_t ii;
	size_t orig_array_len;
	cvi_json_object *my_array;
#ifdef TEST_FORMATTED
	int sflags = 0;
#endif

	my_array = make_array();
	orig_array_len = cvi_json_object_array_length(my_array);

	printf("my_array=\n");
	for (ii = 0; ii < cvi_json_object_array_length(my_array); ii++)
	{
		cvi_json_object *obj = cvi_json_object_array_get_idx(my_array, ii);
		printf("\t[%d]=%s\n", (int)ii, cvi_json_object_to_cvi_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_array));

	for (ii = 0; ii < orig_array_len; ii++)
	{
		rc = cvi_json_object_array_del_idx(my_array, 0, 1);
		printf("after del_idx(0,1)=%d, my_array.to_string()=%s\n", rc,
		       cvi_json_object_to_cvi_json_string(my_array));
	}

	/* One more time, with the empty array: */
	rc = cvi_json_object_array_del_idx(my_array, 0, 1);
	printf("after del_idx(0,1)=%d, my_array.to_string()=%s\n", rc,
	       cvi_json_object_to_cvi_json_string(my_array));

	cvi_json_object_put(my_array);

	/* Delete all array indexes at once */
	my_array = make_array();
	rc = cvi_json_object_array_del_idx(my_array, 0, orig_array_len);
	printf("after del_idx(0,%d)=%d, my_array.to_string()=%s\n", (int)orig_array_len, rc,
	       cvi_json_object_to_cvi_json_string(my_array));

	cvi_json_object_put(my_array);

	/* Delete *more* than all array indexes at once */
	my_array = make_array();
	rc = cvi_json_object_array_del_idx(my_array, 0, orig_array_len + 1);
	printf("after del_idx(0,%d)=%d, my_array.to_string()=%s\n", (int)(orig_array_len + 1), rc,
	       cvi_json_object_to_cvi_json_string(my_array));

	cvi_json_object_put(my_array);

	/* Delete some array indexes, then add more */
	my_array = make_array();
	rc = cvi_json_object_array_del_idx(my_array, 0, orig_array_len - 1);
	printf("after del_idx(0,%d)=%d, my_array.to_string()=%s\n", (int)(orig_array_len - 1), rc,
	       cvi_json_object_to_cvi_json_string(my_array));
	cvi_json_object_array_add(my_array, cvi_json_object_new_string("s1"));
	cvi_json_object_array_add(my_array, cvi_json_object_new_string("s2"));
	cvi_json_object_array_add(my_array, cvi_json_object_new_string("s3"));

	printf("after adding more entries, my_array.to_string()=%s\n",
	       cvi_json_object_to_cvi_json_string(my_array));
	cvi_json_object_put(my_array);
}

void test_array_list_expand_internal(void);
void test_array_list_expand_internal()
{
	int rc;
	size_t ii;
	size_t idx;
	cvi_json_object *my_array;
#ifdef TEST_FORMATTED
	int sflags = 0;
#endif

	my_array = make_array();
	printf("my_array=\n");
	for (ii = 0; ii < cvi_json_object_array_length(my_array); ii++)
	{
		cvi_json_object *obj = cvi_json_object_array_get_idx(my_array, ii);
		printf("\t[%d]=%s\n", (int)ii, cvi_json_object_to_cvi_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_array));

	/* Put iNdex < array->size, no expand. */
	rc = cvi_json_object_array_put_idx(my_array, 5, cvi_json_object_new_int(6));
	printf("put_idx(5,6)=%d\n", rc);

	/* array->size < Put Index < array->size * 2 <= SIZE_T_MAX, the size = array->size * 2. */
	idx = ARRAY_LIST_DEFAULT_SIZE * 2 - 1;
	rc = cvi_json_object_array_put_idx(my_array, idx, cvi_json_object_new_int(0));
	printf("put_idx(%d,0)=%d\n", (int)(idx), rc);

	/* array->size * 2 < Put Index, the size = Put Index. */
	idx = ARRAY_LIST_DEFAULT_SIZE * 2 * 2 + 1;
	rc = cvi_json_object_array_put_idx(my_array, idx, cvi_json_object_new_int(0));
	printf("put_idx(%d,0)=%d\n", (int)(idx), rc);

	/* SIZE_T_MAX <= Put Index, it will fail and the size will no change. */
	idx = SIZE_MAX; // SIZE_MAX = SIZE_T_MAX
	cvi_json_object *tmp = cvi_json_object_new_int(10);
	rc = cvi_json_object_array_put_idx(my_array, idx, tmp);
	printf("put_idx(SIZE_T_MAX,0)=%d\n", rc);
	if (rc == -1)
	{
		cvi_json_object_put(tmp);
	}

	cvi_json_object_put(my_array);
}

int main(int argc, char **argv)
{
	cvi_json_object *my_string, *my_int, *my_null, *my_object, *my_array;
	size_t i;
#ifdef TEST_FORMATTED
	int sflags = 0;
#endif

	MC_SET_DEBUG(1);

#ifdef TEST_FORMATTED
	sflags = parse_flags(argc, argv);
#endif

	my_string = cvi_json_object_new_string("\t");
	printf("my_string=%s\n", cvi_json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_string));
	cvi_json_object_put(my_string);

	my_string = cvi_json_object_new_string("\\");
	printf("my_string=%s\n", cvi_json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_string));
	cvi_json_object_put(my_string);

	my_string = cvi_json_object_new_string("/");
	printf("my_string=%s\n", cvi_json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_string));
	printf("my_string.to_string(NOSLASHESCAPE)=%s\n",
	       cvi_json_object_to_cvi_json_string_ext(my_string, JSON_C_TO_STRING_NOSLASHESCAPE));
	cvi_json_object_put(my_string);

	my_string = cvi_json_object_new_string("/foo/bar/baz");
	printf("my_string=%s\n", cvi_json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_string));
	printf("my_string.to_string(NOSLASHESCAPE)=%s\n",
	       cvi_json_object_to_cvi_json_string_ext(my_string, JSON_C_TO_STRING_NOSLASHESCAPE));
	cvi_json_object_put(my_string);

	my_string = cvi_json_object_new_string("foo");
	printf("my_string=%s\n", cvi_json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_string));

	my_int = cvi_json_object_new_int(9);
	printf("my_int=%d\n", cvi_json_object_get_int(my_int));
	printf("my_int.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_int));

	my_null = cvi_json_object_new_null();
	printf("my_null.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_null));

	my_array = cvi_json_object_new_array();
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(1));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(2));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(3));
	cvi_json_object_array_put_idx(my_array, 4, cvi_json_object_new_int(5));
	printf("my_array=\n");
	for (i = 0; i < cvi_json_object_array_length(my_array); i++)
	{
		cvi_json_object *obj = cvi_json_object_array_get_idx(my_array, i);
		printf("\t[%d]=%s\n", (int)i, cvi_json_object_to_cvi_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_array));

	cvi_json_object_put(my_array);

	test_array_del_idx();
	test_array_list_expand_internal();

	my_array = cvi_json_object_new_array_ext(5);
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(3));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(1));
	cvi_json_object_array_add(my_array, cvi_json_object_new_int(2));
	cvi_json_object_array_put_idx(my_array, 4, cvi_json_object_new_int(0));
	printf("my_array=\n");
	for (i = 0; i < cvi_json_object_array_length(my_array); i++)
	{
		cvi_json_object *obj = cvi_json_object_array_get_idx(my_array, i);
		printf("\t[%d]=%s\n", (int)i, cvi_json_object_to_cvi_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_array));
	cvi_json_object_array_sort(my_array, sort_fn);
	printf("my_array=\n");
	for (i = 0; i < cvi_json_object_array_length(my_array); i++)
	{
		cvi_json_object *obj = cvi_json_object_array_get_idx(my_array, i);
		printf("\t[%d]=%s\n", (int)i, cvi_json_object_to_cvi_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_array));

	cvi_json_object *one = cvi_json_object_new_int(1);
	cvi_json_object *result = cvi_json_object_array_bsearch(one, my_array, sort_fn);
	printf("find cvi_json_object(1) in my_array successfully: %s\n",
	       cvi_json_object_to_cvi_json_string(result));
	cvi_json_object_put(one);

	my_object = cvi_json_object_new_object();
	int rc = cvi_json_object_object_add(my_object, "abc", my_object);
	if (rc != -1)
	{
		printf("ERROR: able to successfully add object to itself!\n");
		fflush(stdout);
	}
	cvi_json_object_object_add(my_object, "abc", cvi_json_object_new_int(12));
	cvi_json_object_object_add(my_object, "foo", cvi_json_object_new_string("bar"));
	cvi_json_object_object_add(my_object, "bool0", cvi_json_object_new_boolean(0));
	cvi_json_object_object_add(my_object, "bool1", cvi_json_object_new_boolean(1));
	cvi_json_object_object_add(my_object, "baz", cvi_json_object_new_string("bang"));

	cvi_json_object *baz_obj = cvi_json_object_new_string("fark");
	cvi_json_object_get(baz_obj);
	cvi_json_object_object_add(my_object, "baz", baz_obj);
	cvi_json_object_object_del(my_object, "baz");

	/* baz_obj should still be valid */
	printf("baz_obj.to_string()=%s\n", cvi_json_object_to_cvi_json_string(baz_obj));
	cvi_json_object_put(baz_obj);

	/*cvi_json_object_object_add(my_object, "arr", my_array);*/
	printf("my_object=\n");
	cvi_json_object_object_foreach(my_object, key, val)
	{
		printf("\t%s: %s\n", key, cvi_json_object_to_cvi_json_string(val));
	}
	printf("my_object.to_string()=%s\n", cvi_json_object_to_cvi_json_string(my_object));

	cvi_json_object_put(my_array);
	my_array = cvi_json_object_new_array_ext(INT_MIN + 1);
	if (my_array != NULL)
	{
		printf("ERROR: able to allocate an array of negative size!\n");
		fflush(stdout);
		cvi_json_object_put(my_array);
		my_array = NULL;
	}

#if SIZEOF_SIZE_T == SIZEOF_INT
	my_array = cvi_json_object_new_array_ext(INT_MAX / 2 + 2);
	if (my_array != NULL)
	{
		printf("ERROR: able to allocate an array of insufficient size!\n");
		fflush(stdout);
		cvi_json_object_put(my_array);
		my_array = NULL;
	}
#endif

	cvi_json_object_put(my_string);
	cvi_json_object_put(my_int);
	cvi_json_object_put(my_null);
	cvi_json_object_put(my_object);
	cvi_json_object_put(my_array);

	return EXIT_SUCCESS;
}
