/*
 * Copyright (c) 2012 Eric Haszlakiewicz
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 */
#include "config.h"

#include "cvi_json_c_version.h"
#ifndef REMOVE_UNUSED_FUNCTION
const char *cvi_json_c_version(void)
{
	return JSON_C_VERSION;
}

int cvi_json_c_version_num(void)
{
	return JSON_C_VERSION_NUM;
}
#endif
