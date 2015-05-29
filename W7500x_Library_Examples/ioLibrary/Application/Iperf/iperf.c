#include <stdio.h>
#include "iperf.h"
#include "socket.h"
#include "wizchip_conf.h"

#if IPERF_MODE == IPERF_MAIN_NOBLCOK
extern uint8_t ref_data[];
int32_t idx = 0;
int32_t recvonly_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0;
   int32_t i;

#ifdef _IPERF_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _IPERF_DEBUG_
			getSn_DIPR(sn, destip);
			destport = getSn_DPORT(sn);
			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            printf("---------------size :%d\r\n", size);
			      ret = recv(sn, buf, size);				 
            for(i=0; i<ret; i++)
            {
                if( ref_data[idx++] != buf[i] )
                {
                   printf("data_invaild: idx[%d]: %c, buf[%d]:%c\r\n", idx-2, ref_data[idx-2],i-1, buf[i-1]);  
									 printf("data_invaild: idx[%d]: %c, buf[%d]:%c  ", idx, ref_data[idx-1],i, buf[i]);
                    while(1);
                }
                if( idx == 2024 ) idx = 0;

            }
         }
         break;
      case SOCK_CLOSE_WAIT :
#ifdef _IPERF_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef _IPERF_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _IPERF_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef _IPERF_DEBUG_
         //printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, Sn_MR_ND)) != sn) return ret;
#ifdef _IPERF_DEBUG_
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}


int32_t recvonly_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {
#ifdef _IPERF_DEBUG_
               printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
         }
         break;
      case SOCK_CLOSED:
#ifdef _IPERF_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _IPERF_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}

#endif
