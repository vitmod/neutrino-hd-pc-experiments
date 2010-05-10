#ifndef __INIT_CS_H
#define __INIT_CS_H
inline void init_cs_api() {}
inline void shutdown_cs_api() {}
inline void * cs_malloc_uncached(size_t size) { return malloc(size); }
inline void cs_free_uncached(void *ptr) { free(ptr); }
inline void * cs_phys_addr(void *ptr) { return ptr; }
#endif
