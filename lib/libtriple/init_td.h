#ifndef __INIT_TD_H
#define __INIT_TD_H
void init_td_api();
void shutdown_td_api();

inline void init_cs_api()
{
	init_td_api();
};

inline void shutdown_cs_api()
{
	shutdown_td_api();
};
#define cs_malloc_uncached	malloc
#define cs_free_uncached	free
#endif
