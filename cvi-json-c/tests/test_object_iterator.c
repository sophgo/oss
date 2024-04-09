#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi_json_object.h"
#include "cvi_json_object_iterator.h"
#include "cvi_json_tokener.h"

int main(int atgc, char **argv)
{
	const char *input = "{\n\
		\"string_of_digits\": \"123\",\n\
		\"regular_number\": 222,\n\
		\"decimal_number\": 99.55,\n\
		\"boolean_true\": true,\n\
		\"boolean_false\": false,\n\
		\"big_number\": 2147483649,\n\
		\"a_null\": null,\n\
		}";

	struct cvi_json_object *new_obj;
	struct cvi_json_object_iterator it;
	struct cvi_json_object_iterator itEnd;

	it = cvi_json_object_iter_init_default();
	new_obj = cvi_json_tokener_parse(input);
	it = cvi_json_object_iter_begin(new_obj);
	itEnd = cvi_json_object_iter_end(new_obj);

	while (!cvi_json_object_iter_equal(&it, &itEnd))
	{
		printf("%s\n", cvi_json_object_iter_peek_name(&it));
		printf("%s\n", cvi_json_object_to_cvi_json_string(cvi_json_object_iter_peek_value(&it)));
		cvi_json_object_iter_next(&it);
	}

	cvi_json_object_put(new_obj);

	return 0;
}
