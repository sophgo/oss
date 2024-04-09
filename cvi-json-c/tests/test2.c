#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi_json.h"
#include "parse_flags.h"

#ifdef TEST_FORMATTED
#define cvi_json_object_to_cvi_json_string(obj) cvi_json_object_to_cvi_json_string_ext(obj, sflags)
#else
/* no special define */
#endif

int main(int argc, char **argv)
{
	cvi_json_object *new_obj;
#ifdef TEST_FORMATTED
	int sflags = 0;
#endif

	MC_SET_DEBUG(1);

#ifdef TEST_FORMATTED
	sflags = parse_flags(argc, argv);
#endif

	new_obj = cvi_json_tokener_parse(
	    "/* more difficult test case */"
	    "{ \"glossary\": { \"title\": \"example glossary\", \"GlossDiv\": { \"title\": \"S\", "
	    "\"GlossList\": [ { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard "
	    "Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", "
	    "\"GlossDef\": \"A meta-markup language, used to create markup languages such as "
	    "DocBook.\", \"GlossSeeAlso\": [\"GML\", \"XML\", \"markup\"] } ] } } }");
	printf("new_obj.to_string()=%s\n", cvi_json_object_to_cvi_json_string(new_obj));
	cvi_json_object_put(new_obj);

	return EXIT_SUCCESS;
}
