#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cvi_json.h"
#include "cvi_printbuf.h"

struct myinfo
{
	int value;
};

static int freeit_was_called = 0;
static void freeit(cvi_json_object *jso, void *userdata)
{
	struct myinfo *info = userdata;
	printf("freeit, value=%d\n", info->value);
	// Don't actually free anything here, the userdata is stack allocated.
	freeit_was_called = 1;
}
static int custom_serializer(struct cvi_json_object *o, struct printbuf *pb, int level, int flags)
{
	sprintbuf(pb, "Custom Output");
	return 0;
}

int main(int argc, char **argv)
{
	cvi_json_object *my_object, *my_sub_object;

	MC_SET_DEBUG(1);

	printf("Test setting, then resetting a custom serializer:\n");
	my_object = cvi_json_object_new_object();
	cvi_json_object_object_add(my_object, "abc", cvi_json_object_new_int(12));
	cvi_json_object_object_add(my_object, "foo", cvi_json_object_new_string("bar"));

	printf("my_object.to_string(standard)=%s\n", cvi_json_object_to_cvi_json_string(my_object));

	struct myinfo userdata = {.value = 123};
	cvi_json_object_set_serializer(my_object, custom_serializer, &userdata, freeit);

	printf("my_object.to_string(custom serializer)=%s\n",
	       cvi_json_object_to_cvi_json_string(my_object));

	printf("Next line of output should be from the custom freeit function:\n");
	freeit_was_called = 0;
	cvi_json_object_set_serializer(my_object, NULL, NULL, NULL);
	assert(freeit_was_called);

	printf("my_object.to_string(standard)=%s\n", cvi_json_object_to_cvi_json_string(my_object));

	cvi_json_object_put(my_object);

	// ============================================

	my_object = cvi_json_object_new_object();
	printf("Check that the custom serializer isn't free'd until the last cvi_json_object_put:\n");
	cvi_json_object_set_serializer(my_object, custom_serializer, &userdata, freeit);
	cvi_json_object_get(my_object);
	cvi_json_object_put(my_object);
	printf("my_object.to_string(custom serializer)=%s\n",
	       cvi_json_object_to_cvi_json_string(my_object));
	printf("Next line of output should be from the custom freeit function:\n");

	freeit_was_called = 0;
	cvi_json_object_put(my_object);
	assert(freeit_was_called);

	// ============================================

	my_object = cvi_json_object_new_object();
	my_sub_object = cvi_json_object_new_double(1.0);
	cvi_json_object_object_add(my_object, "double", my_sub_object);
	printf("Check that the custom serializer does not include nul byte:\n");
	cvi_json_object_set_serializer(my_sub_object, cvi_json_object_double_to_cvi_json_string, "%125.0f,", NULL);
	printf("my_object.to_string(custom serializer)=%s\n",
	       cvi_json_object_to_cvi_json_string_ext(my_object, JSON_C_TO_STRING_NOZERO));

	cvi_json_object_put(my_object);

	return 0;
}