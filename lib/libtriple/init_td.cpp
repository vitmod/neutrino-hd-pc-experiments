#include <stdio.h>

#include "init_td.h"

static const char * FILENAME = "init_td.cpp";

void init_cs_api() { printf("%s:%s\n", FILENAME, __FUNCTION__); }
void shutdown_cs_api() { printf("%s:%s\n", FILENAME, __FUNCTION__); }
