/*
 *********************************************************************
 * @file    : wztoe.c
 * @version : 0.0.1
 * @author  : WIZnet
 * @data    
 * @brief   : wztoe dirver for W7500
 *********************************************************************
 * @attention
 */
#include "W7500x.h"
uint8_t WIZCHIP_READ(uint32_t Addr)
{
    uint8_t ret;
    WIZCHIP_CRITICAL_ENTER();
    ret = (uint8_t)(*(volatile uint8_t *)(Addr));
    WIZCHIP_CRITICAL_EXIT();
    return ret;
}

void WIZCHIP_WRITE(uint32_t Addr, uint8_t Data)
{
    WIZCHIP_CRITICAL_ENTER();
    *(volatile uint8_t *)(Addr) = Data;
    WIZCHIP_CRITICAL_EXIT();
}
#define _DEF_ACCESS_DMA_BUF_
#ifdef _DEF_ACCESS_DMA_BUF_
#include "W7500x_dma.h"
#define MAX_TRANSNUM  1024
void WIZCHIP_READ_DMA (uint32_t BaseAddr, uint16_t ptr, uint8_t* pBuf, uint16_t len)
{
    uint16_t i, loop;
    uint32_t curr_state;
    uint32_t size, dst;

    dst =0;
    size = 0;
    //printf("@1- ptr: %X, len : %d, loop : %d\r\n",ptr, len, (len>>10)); 
    loop = (len>>10);  
    for(i=0; i<loop; i++)
    {
        size =0;
        //printf("--- ptr+MAX_TRANSNUM : %X, ptr :%X \r\n",(uint32_t)(ptr+MAX_TRANSNUM), ptr); 
        if((uint32_t)(ptr+MAX_TRANSNUM) > 0x10000 )  
        {
            size = 0x10000-ptr;
            //printf("--- size : %d, ptr+size : %X \r\n",size , (ptr+size)); 
            dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)ptr), (unsigned int)&pBuf[dst], 0, (size));
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);  
            //printf("--- dst : %d\r\n",dst); 
            dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)(0)), (unsigned int)&pBuf[dst+size], 0, (MAX_TRANSNUM-size));
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);    

        }
        else
        {
            //printf("--- dst : %d\r\n",dst); 
            dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)(ptr)), (unsigned int)&pBuf[dst], 0, MAX_TRANSNUM);
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);     

        }

        len -= MAX_TRANSNUM;
        ptr += MAX_TRANSNUM;
        dst += MAX_TRANSNUM;
    }
    if(len == 0) return;
    //printf("@2- ptr: %X, dst: %d, len : %d\r\n",ptr, dst, len); 


    size =0;
    //printf("--- ptr+len : %X\r\n",(uint32_t)(ptr+len)); 
    if( ((uint32_t)(ptr+len))>0x10000  )  			
    {
        size = 0x10000-ptr;
        //printf("--- size : %d ptr: %X, len: %X \r\n",size, ptr, len); 
        dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)ptr), (unsigned int)&pBuf[dst], 0, size);
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);    
        dst += size;
        size = len - size;
        //printf("--- dst : %d\r\n",dst); 
        dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)(0)), (unsigned int)&pBuf[dst], 0, size);
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);    				

        len -= size;
        ptr += size;
        dst += size;						
    }
    else
    {
        dma_memory_copy ( DMA_M2M5, (BaseAddr+(uint16_t)(ptr)), (unsigned int)&pBuf[dst], 0, len);
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);     
    }
    //printf("@-3 ptr: %X, size: %d, len : %d\r\n",ptr, size, len); 


}
void WIZCHIP_WRITE_DMA(uint32_t BaseAddr, uint16_t ptr, uint8_t* pBuf, uint16_t len)
{
    uint16_t i, loop;
    uint32_t curr_state;
    uint32_t size, src;

    src =0;
    size = 0;
    //printf("#1- ptr: %X, len : %d, loop : %d\r\n",ptr, len, (len>>10)); 
    loop = (len>>10);  
    for(i=0; i<loop; i++)
    {
        size =0;
        //printf("--- ptr+MAX_TRANSNUM : %X, ptr :%X \r\n",(uint32_t)(ptr+MAX_TRANSNUM), ptr); 
        if((uint32_t)(ptr+MAX_TRANSNUM) > 0x10000 )  
        {
            size = 0x10000-ptr;
            //printf("--- size : %d, ptr+size : %X \r\n",size , (ptr+size)); 
            dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src], (BaseAddr+(uint16_t)ptr), 0, size);
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);    
            //printf("--- src : %d\r\n",src); 
            dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src+size], (BaseAddr+(uint16_t)(0)), 0, (MAX_TRANSNUM-size));
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);    
        }
        else
        {
            dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src], (BaseAddr+(uint16_t)(ptr)), 0, MAX_TRANSNUM);
            do { /* Wait until PL230 DMA controller return to idle state */
                curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
            } while (curr_state!=0);     
        }


        len -= MAX_TRANSNUM;
		ptr += MAX_TRANSNUM;
        src += MAX_TRANSNUM;
    }
    if(len == 0) return;
    //printf("#2- ptr: %X, src: %d, len : %d\r\n",ptr, src, len); 


    size =0;
    //printf("--- ptr+len : %X\r\n",(uint32_t)(ptr+len)); 
    if((uint32_t)(ptr+len) > 0x10000 )  
    {
        size = 0x10000-ptr;
        //printf("--- size : %d ptr: %X, len: %X \r\n",size, ptr, len); 
        dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src], (BaseAddr+(uint16_t)ptr), 0, size);
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);    
        src += size;
        size = len - size;
        //printf("--- src : %d\r\n",src); 
        dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src], (BaseAddr+(uint16_t)(0)), 0, size );
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);    

        len -= size;
        ptr += size;
        src += size;						
    }
    else
    {
        dma_memory_copy ( DMA_M2M5, (unsigned int)&pBuf[src], (BaseAddr+(uint16_t)(ptr)), 0, len);
        do { /* Wait until PL230 DMA controller return to idle state */
            curr_state = (DMA->DMA_STATUS >> 4)  & 0xF;
        } while (curr_state!=0);     
    }
    //printf("#-3 ptr: %X, size: %d, len : %d\r\n",ptr, size, len); 
}
#else
void WIZCHIP_READ_BUF (uint32_t BaseAddr, uint32_t ptr, uint8_t* pBuf, uint16_t len)
{
    uint16_t i = 0;
    WIZCHIP_CRITICAL_ENTER();

    for(i = 0; i < len; i++)
        pBuf[i] = *(volatile uint8_t *)(BaseAddr +((ptr+i)&0xFFFF));

    WIZCHIP_CRITICAL_EXIT();
}


void WIZCHIP_WRITE_BUF(uint32_t BaseAddr, uint32_t ptr, uint8_t* pBuf, uint16_t len)
{
    uint16_t i = 0;
    WIZCHIP_CRITICAL_ENTER();

    for( i=0; i<len; i++)
        *(volatile uint8_t *)(BaseAddr + ((ptr+i)&0xFFFF)) = pBuf[i];

    WIZCHIP_CRITICAL_EXIT();
}
#endif


void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
    uint32_t ptr = 0;
    uint32_t sn_tx_base = 0;

    if(len == 0)  return;
    ptr = getSn_TX_WR(sn);
    sn_tx_base = (TXMEM_BASE) | ((sn&0x7)<<18);
#ifdef _DEF_ACCESS_DMA_BUF_
    WIZCHIP_WRITE_DMA (sn_tx_base, ptr, wizdata, len);
#else
    WIZCHIP_WRITE_BUF(sn_tx_base, ptr, wizdata, len);
#endif		
    ptr += len;
    setSn_TX_WR(sn,ptr);
}

void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
    uint32_t ptr = 0;
    uint32_t sn_rx_base = 0; 

    if(len == 0) return;
    ptr = getSn_RX_RD(sn);
    sn_rx_base = (RXMEM_BASE) | ((sn&0x7)<<18);
#ifdef _DEF_ACCESS_DMA_BUF_
    WIZCHIP_READ_DMA (sn_rx_base, ptr, wizdata, len);
#else
    WIZCHIP_READ_BUF(sn_rx_base, ptr, wizdata, len);
#endif	
    ptr += len;
    setSn_RX_RD(sn,ptr);
}


void wiz_recv_ignore(uint8_t sn, uint16_t len)
{
    uint32_t ptr = 0;
    uint32_t sn_rx_base = 0; 
    if(len == 0) return;
    ptr = getSn_RX_RD(sn);
    sn_rx_base = (RXMEM_BASE) | ((sn&0x7)<<18);
    ptr = sn_rx_base + ((ptr+len)&0xFFFF);
    setSn_RX_RD(sn,ptr);
}

