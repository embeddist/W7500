/**
  ******************************************************************************
  * @file    WZTOE/Loopback/main.c 
  * @author  IOP Team
  * @version V1.0.0
  * @date    01-May-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 WIZnet Co.,Ltd.</center></h2>
  ******************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "W7500x.h"

/* Private typedef -----------------------------------------------------------*/
UART_InitTypeDef UART_InitStructure;

/* Private define ------------------------------------------------------------*/
#define __DEF_USED_MDIO__ 
#define __DEF_USED_IC101AG__ //for W7500 Test main Board V001

/* Private function prototypes -----------------------------------------------*/
void delay(__IO uint32_t milliseconds); //Notice: used ioLibray
void TimingDelay_Decrement(void);

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint8_t test_buf[DATA_BUF_SIZE];

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main()
{
    uint8_t tx_size[8] = { (DATA_BUF_SIZE/1024), 0, 0, 0, 0, 0, 0, 0 };
    uint8_t rx_size[8] = { (DATA_BUF_SIZE/1024), 0, 0, 0, 0, 0, 0, 0 };
    
    uint8_t mac_addr[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03}; 
    uint8_t src_addr[4] = {192, 168,  77,  9};
    uint8_t gw_addr[4]  = {192, 168,  77,  1};
    uint8_t sub_addr[4] = {255, 255, 255,  0};		
    //uint8_t dns_server[4] = {8, 8, 8, 8};           // for Example domain name server
    uint8_t tmp[8];

    /* External Clock */
    CRG_PLL_InputFrequencySelect(CRG_OCLK);
    *(volatile uint32_t *)(0x41001014) = 0x000C0200; // 48MHz
		
    /* Set Systme init */
    SystemInit();

    /* UART Init */
    UART_StructInit(&UART_InitStructure);
    UART_Init(UART1,&UART_InitStructure);

    /* SysTick_Config */
    SysTick_Config((GetSystemClock()/1000));
		

    /* Set WZ_100US Register */
    setTIC100US((GetSystemClock()/10000));
    //getTIC100US();	
    //printf(" GetSystemClock: %X, getTIC100US: %X, (%X) \r\n", 
    //      GetSystemClock, getTIC100US(), *(uint32_t *)TIC100US);        


#ifdef __DEF_USED_IC101AG__ //For using IC+101AG
    *(volatile uint32_t *)(0x41003068) = 0x64; //TXD0 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x4100306C) = 0x64; //TXD1 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003070) = 0x64; //TXD2 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003074) = 0x64; //TXD3 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003050) = 0x64; //TXE  - set PAD strengh and pull-up
#endif	
    printf("PHY is linked. \r\n"); 
#ifdef __DEF_USED_MDIO__ 
    /* mdio Init */
    mdio_init(GPIOB, MDC, MDIO );
    /* PHY Link Check via gpio mdio */
    while( link() == 0x0 )
    {
        printf(".");  
        delay(500);
    }
    printf("PHY is linked. \r\n");  
#else
    delay(1000);
    delay(1000);
#endif

    /* Network Configuration */
    setSHAR(mac_addr);
    setSIPR(src_addr);
    setGAR(gw_addr);
    setSUBR(sub_addr);

    getSHAR(tmp);
    printf(" MAC ADDRESS : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]); 
    getSIPR(tmp);
    printf("IP ADDRESS : %.3d.%.3d.%.3d.%.3d\r\n",tmp[0],tmp[1],tmp[2],tmp[3]); 
    getGAR(tmp);
    printf("GW ADDRESS : %.3d.%.3d.%.3d.%.3d\r\n",tmp[0],tmp[1],tmp[2],tmp[3]); 
    getSUBR(tmp);
    printf("SN MASK: %.3d.%.3d.%.3d.%.3d\r\n",tmp[0],tmp[1],tmp[2],tmp[3]); 

    /* Set Network Configuration */
    wizchip_init(tx_size, rx_size);

    printf(" TEST- START \r\n");  
    while(1)
    {
        recvonly_tcps(0, test_buf, 5000);
    }

}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void delay(__IO uint32_t milliseconds)
{
  TimingDelay = milliseconds;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

