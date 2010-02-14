#ifndef __INIT_TD_H
#define __INIT_TD_H
void init_cs_api();
void shutdown_cs_api();
#define cs_malloc_uncached	malloc
#define cs_free_uncached	free
#endif
