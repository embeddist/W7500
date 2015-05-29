#ifndef _IPERF_H_
#define _IPERF_H_

#include <stdint.h>

/* Recv only test debug message printout enable */
#define	_IPERF_DEBUG_

/* DATA_BUF_SIZE define for Iperf example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			(2048*4)
#endif

/************************/
/* Select LOOPBACK_MODE */
/************************/
#define IPERF_MAIN_NOBLOCK    0
#define IPERF_MODE   IPERF_MAIN_NOBLOCK


/* TCP server Recv only test example */
int32_t recvonly_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

/* UDP Recv only test example */
int32_t recvonly_udps(uint8_t sn, uint8_t* buf, uint16_t port);

#endif
