#ifndef _IPPORT_H_
#define _IPPORT_H_

/*
 *  エラーコード
 */

#define SUCCESS			0		/* whatever the call was, it worked */

/* programming errors */
#define ENP_PARAM		-10		/* bad parameter */
#define ENP_LOGIC		-11		/* sequence of events that shouldn't happen */
#define ENP_NOCIPHER		-12		/* No corresponding cipher found for the cipher id */

/* system errors */
#define ENP_NOMEM		-20		/* malloc or calloc failed */
#define ENP_NOBUFFER		-21		/* ran out of free packets */
#define ENP_RESOURCE		-22		/* ran out of other queue-able resource */
#define SEND_DROPPED		ENP_RESOURCE	/* full queue or similar lack of resource */
#define ENP_BAD_STATE		-23		/* TCP layer error */
#define ENP_TIMEOUT		-24		/* TCP layer error */

#define ENP_NOFILE		-25		/* expected file was missing */
#define ENP_FILEIO		-26		/* file IO error */

/* net errors */
#define ENP_SENDERR		-30		/* send to net failed at low layer */
#define ENP_NOARPREP		-31		/* no ARP for a given host */
#define ENP_BAD_HEADER		-32		/* bad header at upper layer (for upcalls) */
#define ENP_NO_ROUTE		-33		/* can't find a reasonable next IP hop */
#define ENP_NO_IFACE		-34		/* can't find a reasonable interface */
#define ENP_HARDWARE		-35		/* detected hardware failure */

/* conditions that are not really fatal OR success: */
#define ENP_SEND_PENDING	1		/* packet queued pending an ARP reply */
#define ENP_NOT_MINE		2		/* packet was not of interest (upcall reply) */
#define EHAVEOOB		217		/* out-of-band */
#define EIEIO			227		/* bad input/output on socket */

/* ARP holding packet while awaiting a response from fhost */
#define ARP_WAITING		ENP_SEND_PENDING

#endif	/* of #ifndef _IPPORT_H_ */
