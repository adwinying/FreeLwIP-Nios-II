#ifndef _NET_VAR_H_
#define _NET_VAR_H_

/*
 *  �ؿ�
 */

extern int_t convert_hexdigit (char *buf, uint_t val, int_t radix, int_t width, char padchar);

/*
 *  �ؿ�
 */

extern uint32_t net_rand (void);
extern void net_srand (uint32_t seed);

#endif	/* of #ifndef _NET_VAR_H_ */
