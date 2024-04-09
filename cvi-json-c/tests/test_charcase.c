#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi_json.h"
#include "cvi_json_tokener.h"

static void test_case_parse(void);

int main(int argc, char **argv)
{
	MC_SET_DEBUG(1);

	test_case_parse();

	return 0;
}

/* make sure only lowercase forms are parsed in strict mode */
static void test_case_parse()
{
	struct cvi_json_tokener *tok;
	cvi_json_object *new_obj;

	tok = cvi_json_tokener_new();
	cvi_json_tokener_set_flags(tok, JSON_TOKENER_STRICT);

	new_obj = cvi_json_tokener_parse_ex(tok, "True", 4);
	assert(new_obj == NULL);

	new_obj = cvi_json_tokener_parse_ex(tok, "False", 5);
	assert(new_obj == NULL);

	new_obj = cvi_json_tokener_parse_ex(tok, "Null", 4);
	assert(new_obj == NULL);

	printf("OK\n");

	cvi_json_tokener_free(tok);
}
