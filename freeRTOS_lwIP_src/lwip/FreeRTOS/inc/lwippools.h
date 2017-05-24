/* OPTIONAL: Pools to replace heap allocation
 * Optional: Pools can be used instead of the heap for mem_malloc. If
 * so, these should be defined here, in increasing order according to
 * the pool element size.
 *
 * LWIP_MALLOC_MEMPOOL(number_elements, element_size)
 */
#if MEM_USE_POOLS
LWIP_MALLOC_MEMPOOL_START
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_64, 64)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_128, 128)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_256, 256)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_512, 512)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_768, 768)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_1024, 1024)
LWIP_MALLOC_MEMPOOL(CONF_LWIP_MALLOC_MEMPOOL_1536, 1536)
LWIP_MALLOC_MEMPOOL_END
#endif /* MEM_USE_POOLS */
