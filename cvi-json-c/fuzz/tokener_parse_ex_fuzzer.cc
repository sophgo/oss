#include <stdint.h>

#include <cvi_json.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	const char *data1 = reinterpret_cast<const char *>(data);
	cvi_json_tokener *tok = cvi_json_tokener_new();
	cvi_json_object *obj = cvi_json_tokener_parse_ex(tok, data1, size);

	cvi_json_object_put(obj);
	cvi_json_tokener_free(tok);
	return 0;
}
