/*************************************************************************************
* Copyright (C) 2013-2015, Cypress Semiconductor Corporation. All rights reserved.
*
* This software, including source code, documentation and related
* materials ( "Software" ), is owned by Cypress Semiconductor
* Corporation ( "Cypress" ) and is protected by and subject to worldwide
* patent protection (United States and foreign), United States copyright
* laws and international treaty provisions. Therefore, you may use this
* Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software ( "EULA" ).
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress's
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ( "High Risk Product" ). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*/
/******************************************************************************/
/** \file sdcard.c
 **
 ** History:
 **   - 2013-05-16  1.0  QXu    First version.
 **   - 2013-07-24  1.1  RQian  Modification.
 **   - 2014-01-28  1.2  MWi    Some comment and code beautifying
 **   - 2015-01-28  1.3  RZh    Modify for FM universal PDL
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "sd_cmd.h"
//#include "dbg.h"
#if 1//(defined(PDL_PERIPHERAL_SD_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup SdGroup
 ******************************************************************************/

//@{
/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/


/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/
static en_result_t Sdcmd_Ops_SendAppCmd( uint32_t u32Arg );

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/
static stc_sdcmd_cmd_t           *gpstcCurCmd        = NULL;
static stc_sdcmd_config_t        *gpstcCmdConfig     = NULL;
static stc_sdcmd_config_t        gstcCmdConfig;
static stc_sdcmd_adma_desc_tbl_t gstcADMADescriptTbl[12] = { 0 };

uint8_t scatter1[BLOCK_SZ];
uint8_t scatter2[BLOCK_SZ];
uint8_t scatter3[BLOCK_SZ];
uint8_t scatter4[BLOCK_SZ];
uint8_t scatter5[BLOCK_SZ];
uint8_t scatter6[BLOCK_SZ];
uint8_t scatter7[BLOCK_SZ];
uint8_t scatter8[BLOCK_SZ];
uint8_t scatter9[BLOCK_SZ];
uint8_t scatter10[BLOCK_SZ];
uint8_t scatter11[BLOCK_SZ];
uint8_t scatter12[BLOCK_SZ];

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/******************************************************************************/
/* Local Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* Global Functions                                                           */
/******************************************************************************/

#if 1//(defined(PDL_PERIPHERAL_SD_ACTIVE))
#define SDIF                (&SDIF0)
/**
 ******************************************************************************
 ** \brief Delay according to specified number
 **
 ** \param [in] u32Delay     Delay number
 **
 ******************************************************************************/
void usdelay( uint32_t u32Delay )
{
#if 1
    // 160M ~ 40M
    volatile uint32_t                i                   = 0;
#ifdef SPAN_BOARD
    uint32_t                         cnt                 = 14;       // @160Mhz
#else
    uint32_t                         cnt                 = 25;       // @30Mhz
  #endif
    cnt =  cnt * u32Delay;

    for ( i = 0; i < cnt; i++ )
    {
        __asm("NOP");
    }
#endif
}

/**
 ******************************************************************************
 ** \brief Initialize SDIO command configuration structure
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval Ok                      SDIO command configuration structure
 **                                 initialized successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **
 ******************************************************************************/
en_result_t Sdcmd_Init( stc_sdcmd_config_t *pstcCfg )
{
   // if (NULL == gpstcCmdConfig)
    if(1)
    {
        gpstcCmdConfig = &gstcCmdConfig;
    }

     memset(gpstcCurCmd, 0, sizeof(stc_sdcmd_cmd_t));
     gpstcCurCmd = NULL;

    if (1)
    {
        //memcpy(gpstcCmdConfig, pstcCfg, sizeof(stc_sdcmd_config_t));
        gpstcCmdConfig->enBusWidth = pstcCfg->enBusWidth;
        gpstcCmdConfig->u32Clock = pstcCfg->u32Clock;
        gpstcCmdConfig->bHighSpeedMode = pstcCfg->bHighSpeedMode;
        gpstcCmdConfig->bUsingADMA = pstcCfg->bUsingADMA;
        return Ok;
    }

    return Error;
}



/**
 ******************************************************************************
 ** \brief The "card insert" interrupt callback function.
 ******************************************************************************/
void SdcmdEventCardInsertCbk( void )
{
    //get the current command and set the flag.
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8CardInsert++;
    }
}

/**
 ******************************************************************************
 ** \brief The "card remove" interrupt callback function.
 ******************************************************************************/
void SdcmdEventCardRemoveCbk( void )
{
    //get the current command and set the flag.
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8CardRemove++;
    }
}


/**
 ******************************************************************************
 ** \brief The "buffer read ready" interrupt callback function.
 ******************************************************************************/
void SdcmdEventBufferReadRdyCbk( void )
{
    //get the current command and set the flag.
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8BuffReadReady++;
    }
}

/**
 ******************************************************************************
 ** \brief The "buffer Write ready" interrupt callback function.
 ******************************************************************************/
void SdcmdEventBufferWriteRdyCbk( void )
{
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8BuffWriteReady++;
    }
}

/**
 ******************************************************************************
 ** \brief The "command complete" interrupt callback function.
 ******************************************************************************/
void SdcmdEventCmdCompleteCbk( void )
{
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8CmdComplete++;
    }
}

/**
 ******************************************************************************
 ** \brief The "Transfer complete" interrupt callback function.
 ******************************************************************************/
void SdcmdEventTransCompleteCbk( void )
{
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8TransComplete++;
    }
}

/**
 ******************************************************************************
 ** \brief The "ADMA transfer complete" interrupt callback function.
 ******************************************************************************/
void SdcmdEventADMACompleteCbk( void )
{
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8ADMAComplete++;
    }
}

/**
 ******************************************************************************
 ** \brief The "Command Time out" interrupt callback function.
 ******************************************************************************/
void SdcmdEventCmdTimeOutCbk( void )
{
    //get the current command and set the flag.
    if (NULL != gpstcCurCmd)
    {
        gpstcCurCmd->stcEvent.u8CmdTimeOut++;
    }
}

/**
 ******************************************************************************
 ** \brief Change host controller sd clock
 **
 ** \param u32Freq New frequency
 **
 ** \retval Ok New frequency is set
 **
 ******************************************************************************/
en_result_t SdCardChangeClock( uint32_t u32Freq )
{
    en_result_t enRet;
    uint16_t    u16Div        = 0;
#ifdef SPAN_BOARD
    uint32_t    u32ClockInput = __HCLK;
#else
#ifdef SIMULATION
    uint32_t    u32ClockInput = SDMEM_INPUT_CLOCK; // 32Mhz
#else
    uint32_t    u32ClockInput = 30000000; // 32Mhz
#endif
#endif
#if (PDL_DEVICE_SERIES_MB9BF56X == PDL_MCU_SERIES ) 
  //  u32ClockInput = __HCLK;
#elif (PDL_DEVICE_SERIES_S6E2CCX == PDL_MCU_SERIES )\
        || (PDL_DEVICE_SERIES_S6E2HGX == PDL_MCU_SERIES )\
        || (PDL_DEVICE_SERIES_S6E2GMX == PDL_MCU_SERIES )\
        || (PDL_DEVICE_SERIES_S6E2DHX == PDL_MCU_SERIES )
    //u32ClockInput =   __HCLK / 4;
#else
    return enRet;
#endif

    u16Div = u32ClockInput /2/  (u32Freq);

    enRet = Sdif_DisableSdclk(SDIF);
    enRet = Sdif_SetClkDiv(SDIF, u16Div);
    enRet = Sdif_EnableInternalClock(SDIF);
    enRet = Sdif_EnableSdclk(SDIF);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Wait for command line free
 **
 ** \retval Ok           Command line is free within a certain time
 ** \retval ErrorTimeout Command line is still busy after a certain time.
 **
 ******************************************************************************/
en_result_t SdcmdPollCmdLineFree( void )
{
    en_result_t enRet    = ErrorTimeout;
    uint32_t    u32Retry = RETRY_TIME;

    while (u32Retry-- > 0)
    {
        if (FALSE == Sdif_CheckCommandInhit(SDIF))
        {
            enRet = Ok;
            break;
        }
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Wait for data line free
 **
 ** \retval Ok           Data line is free within a certain time
 ** \retval ErrorTimeout Data line is still busy after a certain time.
 **
 ******************************************************************************/
en_result_t SdcmdPollDataLineFree( void )
{
    en_result_t enRet    = ErrorTimeout;
    uint32_t    u32Retry = RETRY_TIME * 30;

    while (u32Retry-- > 0)
    {
        if (FALSE == Sdif_CheckCommandWithDataInhit(SDIF))
        {
            enRet = Ok;
            break;
        }
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Check if a paticular interrupt event happens.
 **
 ** \param [In]  enType      the Type of interrupt event.
 **
 * \retval      Ok                      the query event is available.
 * \retval      ErrorTimeOut            the event is not available within a period.
 * \retval      ErrorInvalidParameter   the input param is invalid.
 **
 ******************************************************************************/
en_result_t SdcmdWaitEventAndReset( en_sdcmd_event_t enType )
{
    stc_sdcmd_event_flag_t *pstcEvent = NULL;
    en_result_t            enRet      = Error;
    if (NULL == gpstcCurCmd)
    {
        return ErrorInvalidParameter;
    }
    pstcEvent = &gpstcCurCmd->stcEvent;

    while (Error == enRet)
    {
        switch ( enType )
        {
            case SdCmdEventCmdComplete:
                if (pstcEvent->u8CmdComplete > 0)
                {
                    enRet = Ok;
                    pstcEvent->u8CmdComplete--;
                }
                break;
            case SdCmdEventTransferDone:
                if (pstcEvent->u8TransComplete)
                {
                    enRet = Ok;
                    pstcEvent->u8TransComplete--;
                }
                break;
            case SdCmdEventReadRdy:
                if (pstcEvent->u8BuffReadReady > 0)
                {
                    enRet = Ok;
                    pstcEvent->u8BuffReadReady--;
                }
                break;
            case SdCmdEventWrRdy:
                if (pstcEvent->u8BuffWriteReady)
                {
                    enRet = Ok;
                    pstcEvent->u8BuffWriteReady--;
                }
                break;

            case SdCmdEventAdmaComplete:
                if (pstcEvent->u8ADMAComplete)
                {
                    enRet = Ok;
                    pstcEvent->u8ADMAComplete--;
                }
                break;

            default:

                break;
        }
        if (pstcEvent->u8CardRemove)
        {
            pstcEvent->u8CardRemove--;
            enRet = ErrorAccessRights;
        }
        else if (pstcEvent->u8TranTimeOut
                 || pstcEvent->u8CRCError
                 || pstcEvent->u8CmdTimeOut)
        {
            enRet = ErrorTimeout;
        }
         // after interrupt, check card status
        if (FALSE == Sdif_PollCardInsert(SDIF))
        {
            enRet = ErrorTimeout;
        }
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function read the command data
 **
 *  \param [In]  pstcCmd             Pointer to the current command data structure.
 **
 ** \retval      Ok                  Data is read successfully.
 ** \retval      ErrorTimeout        Read data timeout and failed
 **
 ******************************************************************************/
en_result_t SdcmdRxData( stc_sdcmd_cmd_t *pstcCmd )
{
    en_result_t      enRet      = Error;
    uint32_t         u32blksize = 0;
    static  uint32_t u32blkcnt  = 0;
    uint32_t         i          = 0;
    uint32_t         *pu32Buff  = NULL;
    uint32_t         u32Retry   = RETRY_TIME;
    u32blkcnt = pstcCmd->u32BlockCnt;
    u32blksize = pstcCmd->u32BlockSize;
    pu32Buff = (uint32_t *)pstcCmd->pu8Data;
     if (gpstcCmdConfig->bUsingADMA)
    {
         while (Ok != enRet && (u32Retry-- > 0u))
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventTransferDone);
        }

        if (Ok != enRet)
        {
            return enRet;
        }
    }
    else
    {
         enRet = ErrorTimeout;
u32Retry = RETRY_TIME;
        // dummy code.
		while (Ok != enRet )//&& (u32Retry-- > 0u))
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventReadRdy);
        }
        if (Ok != enRet)
        {
            return enRet;
        }
        u32Retry = RETRY_TIME;

        while (u32blkcnt > 0)
        {
			//while (FALSE == SDIF->SPRSTAT_f.BUFRDEN );//&& (u32Retry-- > 0u));
			while (FALSE == ((SDIF->offset_24) & BIT(11)));//&& (u32Retry-- > 0u));

            //if ( FALSE == SDIF->SPRSTAT_f.BUFRDEN )
            if (FALSE == ((SDIF->offset_24) & BIT(11)))
            {
                break;
            }
            for ( i = u32blksize >> 2; i != 0; i-- )
            {
                *pu32Buff++ = Sdif_ReadData(SDIF);
            }
            u32blkcnt--;
        }
        enRet = Error;
        u32Retry = RETRY_TIME;
		while (Ok != enRet)// && (u32Retry-- > 0u))
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventTransferDone);
        }
    }
    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function Write the command data
 **
 *  \param [In]  pstcCmd             Pointer to the current command data structure.
 **
 ** \retval      Ok                  Data is written successfully.
 ** \retval      Error               Param is invalid.
 **              ErrorTimeout        Write data timeout and failed
 **
 ******************************************************************************/
en_result_t SdcmdTxData( stc_sdcmd_cmd_t *pstcCmd )
{
    en_result_t enRet      = Error;
    uint32_t    u32blksize = 0;
    uint32_t    u32blkcnt  = 0;
    uint32_t    i          = 0;
    uint32_t    *pu32Buff  = NULL;
    uint32_t    u32Retry   = RETRY_TIME;

    u32blkcnt = pstcCmd->u32BlockCnt;
    u32blksize = pstcCmd->u32BlockSize;
    pu32Buff = (uint32_t *)pstcCmd->pu8Data;
    if (gpstcCmdConfig->bUsingADMA)
    {
			u32Retry = RETRY_TIME;
        while (Ok != enRet)// && (u32Retry-- > 0u) )
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventTransferDone);
        }
        if (Ok != enRet)
        {
            return enRet;
        }
    }
    else
    {
        asm("debug");
#if 0
         enRet = ErrorTimeout;
        u32Retry = RETRY_TIME;
        //wait for read buffer is ready
        //
		while (Ok != enRet)// && (u32Retry-- > 0u) )
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventWrRdy);
        }

        if (Ok != enRet)
        {
             return enRet;
        }

        while (u32blkcnt > 0)
        {

            if (Ok != enRet)
            {
                break;
            }

            u32Retry = RETRY_TIME;
			//while (FALSE == SDIF->SPRSTAT_f.BUFWREN );//&& (u32Retry-- > 0u));
			while (FALSE == ((SDIF->offset_24) & BIT(10)));//&& (u32Retry-- > 0u));
            // check again, if still not change, means timeout.
            //            if (FALSE == SDIF->SPRSTAT_f.BUFWREN)
            if (FALSE == ((SDIF->offset_24) & BIT(10)))
            {
                 enRet = ErrorTimeout;
                return enRet;
            }

            for (i = u32blksize >> 2; i != 0; i--)
            {
                Sdif_WriteData(SDIF, *pu32Buff++);
            }
            u32blkcnt--;
        }

		while (Ok != enRet)// && (u32Retry-- > 0u) )
        {
            enRet = SdcmdWaitEventAndReset(SdCmdEventTransferDone);
        }

        if (Ok != enRet)
        {
			return enRet;
        }
#endif
    }

    u32Retry = RETRY_TIME;
  //  while (TRUE == (SDIF->SPRSTAT_f.DATLNACT));// && (u32Retry-- > 0u) );
    do{
       if(!((SDIF->offset_24) & BIT(2))){
            break;
       }
    }while(1); 
    
    //if ( TRUE == (SDIF->SPRSTAT_f.DATLNACT) )
    if ((SDIF->offset_24) & BIT(2) )     
    {
        enRet = ErrorTimeout;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function send out a sd command
 **
 *  \param [In]  pstcCmd             Pointer to the current command data structure.
 **
 ** \retval      Ok                  Command is sent successfully.
 ** \retval      Error               Param is invalid.
 ** \retval      ErrorTimeout        Command sent timeout and failed
 **
 ******************************************************************************/
en_result_t Sdcmd_SendCmd( stc_sdcmd_cmd_t *pstcCmd )
{
    en_result_t            enRet         = Error;
    stc_sdif_data_config_t stcCmdDataCfg = { 0 };
    stc_sdif_cmd_config_t  stcsdifcmdCfg = { 0 };
    uint32_t            u16i            = 0;

    if (NULL == gpstcCmdConfig)
    {
        enRet = ErrorInvalidParameter;
        return enRet;
    }

    memset(gstcADMADescriptTbl, 0x0, sizeof(gstcADMADescriptTbl));


    if (gpstcCmdConfig->bUsingADMA &&
        NULL != pstcCmd->pu8Data)
    {

#if 0
        gstcADMADescriptTbl[0].u16Act = 2;
        gstcADMADescriptTbl[0].u16AttrEnd = 1;
        gstcADMADescriptTbl[0].u16AttrValid = 1;
        gstcADMADescriptTbl[0].u16AttrInt = 1;
        gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
        gstcADMADescriptTbl[0].u32addr = (0x400000 + ((uint32_t)pstcCmd->pu8Data));
        stcCmdDataCfg.u32AdmaDespTableAddress = ((0x400000)+((uint32_t)&gstcADMADescriptTbl[0]));
        ---Working--
            gstcADMADescriptTbl[0].u16Act = 2;
        gstcADMADescriptTbl[0].u16AttrEnd = 0;
        gstcADMADescriptTbl[0].u16AttrValid = 1;
        gstcADMADescriptTbl[0].u16AttrInt = 0;
        //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
        gstcADMADescriptTbl[0].u16Len = 256;
        gstcADMADescriptTbl[0].u32addr = (0x400000 + ((uint32_t)pstcCmd->pu8Data));

        gstcADMADescriptTbl[1].u16Act = 2;
        gstcADMADescriptTbl[1].u16AttrEnd = 0;
        gstcADMADescriptTbl[1].u16AttrValid = 1;
        gstcADMADescriptTbl[1].u16AttrInt = 0;
        //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
        gstcADMADescriptTbl[1].u16Len = 256 ;
        gstcADMADescriptTbl[1].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+256)));

        gstcADMADescriptTbl[2].u16Act = 2;
        gstcADMADescriptTbl[2].u16AttrEnd = 0;
        gstcADMADescriptTbl[2].u16AttrValid = 1;
        gstcADMADescriptTbl[2].u16AttrInt = 0;
        //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
        gstcADMADescriptTbl[2].u16Len = 256;
        gstcADMADescriptTbl[2].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+512)));

        gstcADMADescriptTbl[3].u16Act = 2;
        gstcADMADescriptTbl[3].u16AttrEnd = 1;
        gstcADMADescriptTbl[3].u16AttrValid = 1;
        gstcADMADescriptTbl[3].u16AttrInt = 1;
        //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
        gstcADMADescriptTbl[3].u16Len = 256 ;
        gstcADMADescriptTbl[3].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+768)));
        stcCmdDataCfg.u32AdmaDespTableAddress = ((0x400000)+((uint32_t)&gstcADMADescriptTbl[0]));
#else
        if(pstcCmd->u32Arg & BIT(31) )
        {
            for(u16i = 0; u16i<512;u16i++)
            {
                scatter1[u16i] =0x1;
                scatter2[u16i] =0x2;
                scatter3[u16i] =0x3;
                scatter4[u16i] =0x4;
                scatter5[u16i] = 0x5;
                scatter6[u16i] = 0x6;
                scatter7[u16i] = 0x7;
                scatter8[u16i] = 0x8;
                scatter9[u16i] = 0x9;
                scatter10[u16i] = 0x10;
                scatter11[u16i] = 0x11;
                scatter12[u16i] = 0x12;            }
            gstcADMADescriptTbl[0].u16Act = 2;
            gstcADMADescriptTbl[0].u16AttrEnd = 0;
            gstcADMADescriptTbl[0].u16AttrValid = 1;
            gstcADMADescriptTbl[0].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[0].u16Len = 256*2;
            gstcADMADescriptTbl[0].u32addr = (0x400000 + ((uint32_t)scatter1));

            gstcADMADescriptTbl[1].u16Act = 2;
            gstcADMADescriptTbl[1].u16AttrEnd = 0;
            gstcADMADescriptTbl[1].u16AttrValid = 1;
            gstcADMADescriptTbl[1].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[1].u16Len = 256*2 ;
            gstcADMADescriptTbl[1].u32addr = (0x400000 + ((uint32_t)scatter2));

            gstcADMADescriptTbl[2].u16Act = 2;
            gstcADMADescriptTbl[2].u16AttrEnd = 0;
            gstcADMADescriptTbl[2].u16AttrValid = 1;
            gstcADMADescriptTbl[2].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[2].u16Len = 256*2;
            gstcADMADescriptTbl[2].u32addr = (0x400000 + ((uint32_t)scatter3));

            gstcADMADescriptTbl[3].u16Act = 2;
            gstcADMADescriptTbl[3].u16AttrEnd = 0;
            gstcADMADescriptTbl[3].u16AttrValid = 1;
            gstcADMADescriptTbl[3].u16AttrInt = 0;
            gstcADMADescriptTbl[3].u16Len = 256*2 ;
            gstcADMADescriptTbl[3].u32addr = (0x400000 + ((uint32_t)scatter4));

            gstcADMADescriptTbl[4].u16Act = 2;
            gstcADMADescriptTbl[4].u16AttrEnd = 0;
            gstcADMADescriptTbl[4].u16AttrValid = 1;
            gstcADMADescriptTbl[4].u16AttrInt = 0;
            gstcADMADescriptTbl[4].u16Len = 256*2 ;
            gstcADMADescriptTbl[4].u32addr = (0x400000 + ((uint32_t)scatter5));

            gstcADMADescriptTbl[5].u16Act = 2;
            gstcADMADescriptTbl[5].u16AttrEnd = 0;
            gstcADMADescriptTbl[5].u16AttrValid = 1;
            gstcADMADescriptTbl[5].u16AttrInt = 0;
            gstcADMADescriptTbl[5].u16Len = 256*2 ;
            gstcADMADescriptTbl[5].u32addr = (0x400000 + ((uint32_t)scatter6));

            gstcADMADescriptTbl[6].u16Act = 2;
            gstcADMADescriptTbl[6].u16AttrEnd = 0;
            gstcADMADescriptTbl[6].u16AttrValid = 1;
            gstcADMADescriptTbl[6].u16AttrInt = 0;
            gstcADMADescriptTbl[6].u16Len = 256*2 ;
            gstcADMADescriptTbl[6].u32addr = (0x400000 + ((uint32_t)scatter7));

            gstcADMADescriptTbl[7].u16Act = 2;
            gstcADMADescriptTbl[7].u16AttrEnd = 0;
            gstcADMADescriptTbl[7].u16AttrValid = 1;
            gstcADMADescriptTbl[7].u16AttrInt = 0;
            gstcADMADescriptTbl[7].u16Len = 256*2 ;
            gstcADMADescriptTbl[7].u32addr = (0x400000 + ((uint32_t)scatter8));

            gstcADMADescriptTbl[8].u16Act = 2;
            gstcADMADescriptTbl[8].u16AttrEnd = 0;
            gstcADMADescriptTbl[8].u16AttrValid = 1;
            gstcADMADescriptTbl[8].u16AttrInt = 0;
            gstcADMADescriptTbl[8].u16Len = 256*2 ;
            gstcADMADescriptTbl[8].u32addr = (0x400000 + ((uint32_t)scatter9));

            gstcADMADescriptTbl[9].u16Act = 2;
            gstcADMADescriptTbl[9].u16AttrEnd = 0;
            gstcADMADescriptTbl[9].u16AttrValid = 1;
            gstcADMADescriptTbl[9].u16AttrInt = 0;
            gstcADMADescriptTbl[9].u16Len = 256*2 ;
            gstcADMADescriptTbl[9].u32addr = (0x400000 + ((uint32_t)scatter10));

            gstcADMADescriptTbl[10].u16Act = 2;
            gstcADMADescriptTbl[10].u16AttrEnd = 0;
            gstcADMADescriptTbl[10].u16AttrValid = 1;
            gstcADMADescriptTbl[10].u16AttrInt = 0;
            gstcADMADescriptTbl[10].u16Len = 256*2 ;
            gstcADMADescriptTbl[10].u32addr = (0x400000 + ((uint32_t)scatter11));

            gstcADMADescriptTbl[11].u16Act = 2;
            gstcADMADescriptTbl[11].u16AttrEnd = 1;
            gstcADMADescriptTbl[11].u16AttrValid = 1;
            gstcADMADescriptTbl[11].u16AttrInt = 1;
            gstcADMADescriptTbl[11].u16Len = 256*2 ;
            gstcADMADescriptTbl[11].u32addr = (0x400000 + ((uint32_t)scatter12));

            stcCmdDataCfg.u32AdmaDespTableAddress = ((0x400000)+((uint32_t)&gstcADMADescriptTbl[0]));
        } else {
            memset(pstcCmd->pu8Data,0xff,(1024*2));
            gstcADMADescriptTbl[0].u16Act = 2;
            gstcADMADescriptTbl[0].u16AttrEnd = 0;
            gstcADMADescriptTbl[0].u16AttrValid = 1;
            gstcADMADescriptTbl[0].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[0].u16Len = 256*2;
            gstcADMADescriptTbl[0].u32addr = (0x400000 + ((uint32_t)pstcCmd->pu8Data));

            gstcADMADescriptTbl[1].u16Act = 2;
            gstcADMADescriptTbl[1].u16AttrEnd = 0;
            gstcADMADescriptTbl[1].u16AttrValid = 1;
            gstcADMADescriptTbl[1].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[1].u16Len = 256*2;
            gstcADMADescriptTbl[1].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+512)));

            gstcADMADescriptTbl[2].u16Act = 2;
            gstcADMADescriptTbl[2].u16AttrEnd = 0;
            gstcADMADescriptTbl[2].u16AttrValid = 1;
            gstcADMADescriptTbl[2].u16AttrInt = 0;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[2].u16Len = 256*2;
            gstcADMADescriptTbl[2].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*2)));

            gstcADMADescriptTbl[3].u16Act = 2;
            gstcADMADescriptTbl[3].u16AttrEnd = 0;
            gstcADMADescriptTbl[3].u16AttrValid = 1;
            gstcADMADescriptTbl[3].u16AttrInt = 1;
            //gstcADMADescriptTbl[0].u16Len = pstcCmd->u32BlockSize * pstcCmd->u32BlockCnt;
            gstcADMADescriptTbl[3].u16Len = 256*2 ;
            gstcADMADescriptTbl[3].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*3)));


            gstcADMADescriptTbl[4].u16Act = 2;
            gstcADMADescriptTbl[4].u16AttrEnd = 0;
            gstcADMADescriptTbl[4].u16AttrValid = 1;
            gstcADMADescriptTbl[4].u16AttrInt = 0;
            gstcADMADescriptTbl[4].u16Len = 256*2 ;
            gstcADMADescriptTbl[4].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*4)));

            gstcADMADescriptTbl[5].u16Act = 2;
            gstcADMADescriptTbl[5].u16AttrEnd = 0;
            gstcADMADescriptTbl[5].u16AttrValid = 1;
            gstcADMADescriptTbl[5].u16AttrInt = 0;
            gstcADMADescriptTbl[5].u16Len = 256*2 ;
            gstcADMADescriptTbl[5].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*5)));

            gstcADMADescriptTbl[6].u16Act = 2;
            gstcADMADescriptTbl[6].u16AttrEnd = 0;
            gstcADMADescriptTbl[6].u16AttrValid = 1;
            gstcADMADescriptTbl[6].u16AttrInt = 0;
            gstcADMADescriptTbl[6].u16Len = 256*2 ;
            gstcADMADescriptTbl[6].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*6)));

            gstcADMADescriptTbl[7].u16Act = 2;
            gstcADMADescriptTbl[7].u16AttrEnd = 0;
            gstcADMADescriptTbl[7].u16AttrValid = 1;
            gstcADMADescriptTbl[7].u16AttrInt = 0;
            gstcADMADescriptTbl[7].u16Len = 256*2 ;
            gstcADMADescriptTbl[7].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*7)));

            gstcADMADescriptTbl[8].u16Act = 2;
            gstcADMADescriptTbl[8].u16AttrEnd = 0;
            gstcADMADescriptTbl[8].u16AttrValid = 1;
            gstcADMADescriptTbl[8].u16AttrInt = 0;
            gstcADMADescriptTbl[8].u16Len = 256*2 ;
            gstcADMADescriptTbl[8].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*8)));

            gstcADMADescriptTbl[9].u16Act = 2;
            gstcADMADescriptTbl[9].u16AttrEnd = 0;
            gstcADMADescriptTbl[9].u16AttrValid = 1;
            gstcADMADescriptTbl[9].u16AttrInt = 0;
            gstcADMADescriptTbl[9].u16Len = 256*2 ;
            gstcADMADescriptTbl[9].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*9)));

            gstcADMADescriptTbl[10].u16Act = 2;
            gstcADMADescriptTbl[10].u16AttrEnd = 0;
            gstcADMADescriptTbl[10].u16AttrValid = 1;
            gstcADMADescriptTbl[10].u16AttrInt = 0;
            gstcADMADescriptTbl[10].u16Len = 256*2 ;
            gstcADMADescriptTbl[10].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*10)));

            gstcADMADescriptTbl[11].u16Act = 2;
            gstcADMADescriptTbl[11].u16AttrEnd = 1;
            gstcADMADescriptTbl[11].u16AttrValid = 1;
            gstcADMADescriptTbl[11].u16AttrInt = 1;
            gstcADMADescriptTbl[11].u16Len = 256*2 ;
            gstcADMADescriptTbl[11].u32addr = (0x400000 + ((uint32_t)(pstcCmd->pu8Data+BLOCK_SZ*11)));

            stcCmdDataCfg.u32AdmaDespTableAddress = ((0x400000)+((uint32_t)&gstcADMADescriptTbl[0]));
        }
#endif
    }

    enRet = SdcmdPollCmdLineFree();
    if (Ok != enRet)
    {
        return enRet;
    }

    enRet = SdcmdPollDataLineFree();
    if (Ok != enRet)
    {
        return enRet;
    }

    stcCmdDataCfg.u16BlockSize = pstcCmd->u32BlockSize;
    stcsdifcmdCfg.u32Argument = pstcCmd->u32Arg;
    stcCmdDataCfg.bRead = pstcCmd->u8Dir;

    if (gpstcCmdConfig->bUsingADMA && NULL != pstcCmd->pu8Data)
    {
        stcCmdDataCfg.bEnableDma = TRUE;
    }

    // set the CMD register to issue the command.
    stcsdifcmdCfg.bDataPresent = (NULL == pstcCmd->pu8Data) ? 0 : 1;

    switch ( pstcCmd->enRspType )
    {
        case SdCmdRspNone:
            stcsdifcmdCfg.bCmdCrcCheck = FALSE;
            stcsdifcmdCfg.bCmdIndexCheck = FALSE;
            stcsdifcmdCfg.enResponseType = SdifResponseNone;
            break;
        case SdCmdRspR2:
            stcsdifcmdCfg.bCmdCrcCheck = TRUE;
            stcsdifcmdCfg.bCmdIndexCheck = FALSE;
            stcsdifcmdCfg.enResponseType = SdifResponse136Bit;
            break;
        case SdCmdRspR3R4:
            stcsdifcmdCfg.bCmdCrcCheck = FALSE;
            stcsdifcmdCfg.bCmdIndexCheck = FALSE;
            stcsdifcmdCfg.enResponseType = SdifResponse48Bit;
            break;
        case SdCmdRspR1NORMALR5R6R7:
            stcsdifcmdCfg.bCmdCrcCheck = TRUE;
            stcsdifcmdCfg.bCmdIndexCheck = TRUE;
            stcsdifcmdCfg.enResponseType = SdifResponse48Bit;
            break;
        case SdCmdRspR1BNORMAL:
        case SdCmdRspR1AUTO :
        case SdCmdRspR5B:
            stcsdifcmdCfg.bCmdCrcCheck = TRUE;
            stcsdifcmdCfg.bCmdIndexCheck = TRUE;
            stcsdifcmdCfg.enResponseType = SdifResponse48BitCheckBusy;
            break;
        default :
            return ErrorInvalidParameter;
    }

    stcsdifcmdCfg.u8CmdIndex = pstcCmd->u32CmdIdx & 0x3F; ///< filter ACMD offset.

    if (SD_CMD53 == stcsdifcmdCfg.u8CmdIndex && (pstcCmd->u32Arg & (1 << 27)))
    {
        //multi-block mode.
        stcCmdDataCfg.bMultipleBlock = TRUE;
        stcCmdDataCfg.bBlockCountEnable = TRUE;
        stcsdifcmdCfg.enAutoCmdType = SdifAutoCmdDisable;
        stcCmdDataCfg.u16BlockCount = pstcCmd->u32BlockCnt;
    }
    else
    {
        //single block mode.
        stcCmdDataCfg.bMultipleBlock = FALSE;
        stcCmdDataCfg.bBlockCountEnable = FALSE;
        stcsdifcmdCfg.enAutoCmdType = SdifAutoCmdDisable;
       stcCmdDataCfg.u16BlockCount = pstcCmd->u32BlockCnt;
    }

    if (NULL != gpstcCurCmd)
    {
        return ErrorOperationInProgress;
    }

    // hard code
    stcsdifcmdCfg.enCmdType = SdifCmdNormal;
    stcCmdDataCfg.u8DataTimeout = 0xc;

    // set current command to global value for callback function.
    gpstcCurCmd = pstcCmd;

    // send out the command
    Sdif_InitDataTransfer(SDIF, &stcCmdDataCfg);
    Sdif_SendCommand(SDIF, &stcsdifcmdCfg);

    // wait command line free.
    enRet = SdcmdWaitEventAndReset(SdCmdEventCmdComplete);
    if (Ok != enRet)
    {
        gpstcCurCmd = NULL;
        return enRet;
    }

    // read response data.
    enRet = Sdif_GetResponse(SDIF, (uint16_t *)pstcCmd->u32Rsp, sizeof(pstcCmd->u32Rsp) / 2);

    // if have data to TX/RX
    if (NULL != pstcCmd->pu8Data)
    {
        switch ( pstcCmd->u32CmdIdx )
        {
            case SD_CMD24:
            case SD_CMD25:
            case SD_CMD53:
                if(stcsdifcmdCfg.u32Argument & (1 << 31))
                {
                enRet = SdcmdTxData(pstcCmd);
                }
                else
                {
                  enRet = SdcmdRxData(pstcCmd);                
                }
                break;
            case SD_CMD17:
            case MMC_CMD8:
            case SD_CMD18:
            case SD_ACMD51:
                enRet = SdcmdRxData(pstcCmd);
                break;
            default:
                break;
        }
    }

    // finish process.
    memset(gstcADMADescriptTbl, 0x0, sizeof(gstcADMADescriptTbl));

    gpstcCurCmd = NULL;
    return enRet;
}
en_result_t Sdio_Reg_info(SDIO_CCCR_REG_Info_t *Reg_Info)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t enRet             = Error;
    uint8_t cccr[22] = {0};
    stcCmd.u32CmdIdx    = SD_CMD53;
    stcCmd.u32Arg       = CCCR_BYTE_READ;
    stcCmd.pu8Data      = cccr;
    stcCmd.u32BlockSize = 0x16;
    stcCmd.u32BlockCnt  = 1;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    memset((void *)Reg_Info, 0x0, 22);
    memcpy((void *)Reg_Info, &stcCmd.pu8Data, 22);
    return enRet;
}
en_result_t Sdio_Card_Command(stc_sdcmd_info_t *pstcInfo,uint32_t u32arg)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    uint32_t    u32Arg            = u32arg;
    en_result_t enRet             = Error;
    stcCmd.u32CmdIdx    = SD_CMD52;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    memset((void *)&pstcInfo->Response[0], 0x0, sizeof(pstcInfo->Response));
    memcpy((void *)&pstcInfo->Response[0], &stcCmd.u32Rsp[0], sizeof(pstcInfo->Response));

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function send out SD bus width changing command
 **
 ** \param [In]  pstcInfo                Pointer to a sd infomation data structure.
 ** \param [In]  enWidth                 the buswidth to change.
 **
 ** \retval      Ok                  Bus width is changed successfully.
 ** \retval      Error               Param is invalid.
 ** \retval      ErrorTimeout        Bus width change timeout and failed
 **
 ******************************************************************************/
en_result_t Sdcmd_SDChangeBusWidth( stc_sdcmd_info_t *pstcInfo, en_sdcmd_buswidth_t enWidth )
{
    en_result_t enRet         = Error;
    uint32_t    u32Arg        = 0;
    boolean_t   bHost4BitMode = FALSE;

    switch ( enWidth )
    {
        case SdBusWidth1:
            u32Arg = 0;                 // 0 = 1bit
            bHost4BitMode = FALSE;
            break;
        case SdBusWidth4:
      u32Arg  = BIT4_BUS_WIDTH_ARG;                 // 2 = 4biT
            bHost4BitMode = TRUE;
            break;
        default:
            return ErrorInvalidParameter;
    }

    // may add query card capability.

    if (Ok == Sdcmd_Ops_SetBusWdith(pstcInfo, u32Arg))
    {
        // host side setting.
        enRet = Sdif_SetBusWidth(SDIF, bHost4BitMode);
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function send out MMC bus width changing command
 **
 ** \param [In]  pstcInfo                Pointer to a sd infomation data structure.
 ** \param [In]  enWidth                 the buswidth to change.
 **
 ** \retval      Ok                  Bus width is changed successfully.
 ** \retval      Error               Param is invalid.
 ** \retval      ErrorTimeout        Bus width change timeout and failed
 **
 ******************************************************************************/
en_result_t Sdcmd_MMCChangeBusWidth( stc_sdcmd_info_t *pstcInfo, en_sdcmd_buswidth_t enWidth )
{
    en_result_t enRet         = Error;
    uint32_t    u32Arg        = 0;
    boolean_t   bHost4BitMode = FALSE;

    switch ( enWidth )
    {
        // cmd6 ARG data structure
        // [ACCESS:2]<<24 | [EXT_CSD IDX:8]<<16
        // |[Value:8]<<8 | [CMD set:2]<<0
        case SdBusWidth1:
            u32Arg = (0x3 << 24) | (0xB7 << 16) | (0x0 << 8) | 0x0 << 0;
            bHost4BitMode = FALSE;
            break;
        case SdBusWidth4:
            u32Arg = (0x3 << 24) | (0xB7 << 16) | (0x1 << 8) | 0x0 << 0;
            bHost4BitMode = TRUE;
            break;
        case SdBusWidth8:
        default:
            return ErrorInvalidParameter;
    }

    enRet = MMCcmd_Ops_CmdSet(u32Arg);
    if (Ok == enRet)
    {
        Sdif_SetBusWidth(SDIF, bHost4BitMode);
        return enRet;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function enables high speed of MMC card
 **
 ** \param [In]  bEnable             Enable/disable
 ** \param [In]  u32Clock            Clock frequency
 **
 ** \retval      Ok                  New frequency is set.
 **
 ******************************************************************************/
en_result_t Sdcmd_MMCEnableHighSpeed( boolean_t bEnable, uint32_t u32Clock )
{
    en_result_t enRet             = Error;
    uint32_t    u32TargetClk      = 0;
    uint32_t    u32Arg            = 0;
    uint32_t    u32HighSpeedValue = 0;

    if (TRUE == bEnable)
    {
        u32Arg = (0x3 << 24) | (0xB9 << 16) | (0x0 << 8) | 0x0 << 0;
        u32TargetClk = u32Clock;
        u32HighSpeedValue = 1;
    }
    else
    {
        u32Arg = (0x3 << 24) | (0xB9 << 16) | (0x0 << 8) | 0x0 << 0;
#ifdef SIMULATION
        u32TargetClk = SdClk25M;
#else
        u32TargetClk = SdClk400K;
#endif
        u32HighSpeedValue = 0;
    }

    enRet = MMCcmd_Ops_CmdSet(u32Arg);
    if (Ok == enRet)
    {
        SdCardChangeClock(u32TargetClk);
        Sdif_SetBusSpeedMode(SDIF, u32HighSpeedValue);
    }

    //should add wait busy for checking SD card status.


    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function enables high speed of SD card
 **
 ** \param [In]  bEnable             Enable/disable
 ** \param [In]  u32Clock            Clock frequency
 **
 ** \retval      Ok                  New frequency is set.
 **
 ******************************************************************************/
en_result_t Sdcmd_SDEnableHighSpeed( boolean_t bEnable, uint32_t u32Clock )
{
    en_result_t enRet             = Error;
    uint32_t    u32TargetClk      = 0;
    uint32_t    u32Arg            = 0;
    uint32_t    u32HighSpeedValue = 0;

    if (TRUE == bEnable)
    {
        u32TargetClk = u32Clock;
        u32HighSpeedValue = 1;
    }
    else
    {
#ifdef SIMULATION
        u32TargetClk = SdClk25M;
#else
        u32TargetClk = SdClk400K;
#endif
    }

    u32Arg = (1 << 31) | 0xFFF0;
    u32Arg |= (u32HighSpeedValue)&0xF;

    enRet = Sdcmd_Ops_SwitchFunc(u32Arg);
    if (Ok == enRet)
    {
        enRet = SdCardChangeClock(u32TargetClk);

        Sdif_SetBusSpeedMode(SDIF, TRUE);
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief This function erases a number block data of SD card
 **
 ** \param [In]  pstcInfo        Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock   Erase start block number
 ** \param [In]  u32Count        Erase block count
 **
 ** \retval     Ok                      Erase successfully
 ** \retval     ErrorInvalidParameter   Param is invalid.
 ** \retval     ErrorTimeout            Erase timeout
 **
 ******************************************************************************/
en_result_t Sdcmd_EraseBlock( stc_sdcmd_info_t *pstcInfo,
                              uint32_t u32StartBlock,
                              uint32_t u32Count
                             )
{
    en_result_t           enRet    = Error;
    en_sdcmd_cardstatus_t enStatus = SdCardStateRev;
    uint32_t              i        = 0;

    if (NULL == pstcInfo)
    {
        return ErrorInvalidParameter;
    }

    enRet = Sdcmd_Ops_EraseStartAddr(pstcInfo, u32StartBlock);
    if (Ok != enRet)
    {
        return enRet;
    }

    enRet = Sdcmd_Ops_EraseEndAddr(pstcInfo, u32StartBlock + u32Count);
    if (Ok != enRet)
    {
        return enRet;
    }

    enRet = Sdcmd_Ops_Erase(pstcInfo);
    if (Ok != enRet)
    {
        return enRet;
    }

    // polling result
    enRet = Error;
    i = RETRY_TIME;

    while (i-- > 0)
    {
        enStatus = Sdcmd_Ops_SendStatus(pstcInfo);
        if (SdCardStateTran == enStatus)
        {
            enRet = Ok;
            break;
        }

       // usdelay(1000);
       //rsi_delay_ms(1);
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD0 (Go idle)
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_GoIdle( void )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD0;
    stcCmd.u32Arg = 0x0;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0x0;
    stcCmd.u32BlockCnt = 0x0;
    stcCmd.enRspType = SdCmdRspNone;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD55 (Send application command)
 **
 * \param [In]  u32Arg               Command Param data
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
static en_result_t Sdcmd_Ops_SendAppCmd( uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD55;
    stcCmd.u32Arg = u32Arg << 16;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD8 (Send application command)
 **
 * \param [In]  u32Arg               Command Param data
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SendIfCond( uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD8;
    stcCmd.u32Arg = u32Arg;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send ACMD41 (Send operation condition command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg               Command Param data
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SendOpCond( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD8;
    stcCmd.u32Arg = u32Arg;             ///< 4FFF8000
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    if (Ok == Sdcmd_Ops_SendAppCmd(pstcInfo->RCA))
    {
        enRet = Sdcmd_SendCmd(&stcCmd);
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD2 (Send get CID command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 **        [In]  u32Arg                  Command Param data
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_AllSendCid( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    uint32_t        i      = 0;

    stcCmd.u32CmdIdx = SD_CMD2;
    stcCmd.u32Arg = 0;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR2;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    // Manufacturer like info  in CID, no need to parse.
    if (Ok == enRet)
    {
        for ( i = 0; i < 15; i++ )
        {
            // Response data 127:8 is stored in Response reg: 119:0
            // Make clear of LSB/MSB
            pstcInfo->CID[i] = *((uint8_t *)(stcCmd.u32Rsp) + (14 - i));
        }
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD3 (Send relative address command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg                  Command Param data
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SendRelativeAddr( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD3;
    stcCmd.u32Arg = 0;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    if (0x0 == u32Arg)
    {
        // set RCA
        pstcInfo->RCA = stcCmd.u32Rsp[0] >> 16;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD9 (Send CSD command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SendCSD( stc_sdcmd_info_t *pstcInfo )
{
    stc_sdcmd_cmd_t    stcCmd        = { 0 };
    en_result_t        enRet         = Error;
    stc_sdcmd_csd_v1_t *pstcCsdSd    = NULL;
    stc_sdcmd_csd_v2_t *pstcCsdSdHc  = NULL;

    uint32_t           u32NumSector  = 0;
    uint32_t           u32Csize      = 0;
    uint32_t           u32CsizeMulti = 0;

    if (NULL == pstcInfo)
    {
        return ErrorInvalidParameter;
    }

    stcCmd.u32CmdIdx = SD_CMD9;
    stcCmd.u32Arg = pstcInfo->RCA << 16;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR2;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    if (enRet != Ok)
    {
        return enRet;
    }

    memcpy(pstcInfo->CSD, stcCmd.u32Rsp, sizeof(stcCmd.u32Rsp));

    pstcCsdSd = (stc_sdcmd_csd_v1_t *)&pstcInfo->CSD[0];
    pstcCsdSdHc = (stc_sdcmd_csd_v2_t *)&pstcInfo->CSD[0];

    // High Capacity
    if (SdCardCapacityHigh == pstcInfo->enCapacityType)
    {
        u32Csize = ((unsigned int)pstcCsdSdHc->C_SIZE3 << 16)
                   + ((unsigned int)pstcCsdSdHc->C_SIZE2 << 8)
                   + pstcCsdSdHc->C_SIZE1;

        u32NumSector = (u32Csize + 1) * 1024;
    }
    // Standard Capacity
    else
    {
        u32Csize = ((unsigned int)pstcCsdSd->C_SIZE3 << 10)
                   + ((unsigned int)pstcCsdSd->C_SIZE2 << 2)
                   + pstcCsdSd->C_SIZE1;

        u32CsizeMulti = (pstcCsdSd->C_SIZE_MULTI2 << 1) + pstcCsdSd->C_SIZE_MULTI1;
        u32NumSector = (u32Csize + 1) << (u32CsizeMulti + 2);

        if (pstcCsdSd->READ_BL_LEN == 0x0A)
        {
            u32NumSector *= 2;
        }
        else if (pstcCsdSd->READ_BL_LEN == 0x0B)
        {
            u32NumSector *= 4;
        }
    }

    pstcInfo->u32MaxSectorNum = u32NumSector;

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD7 (Send select card command)
 **
 ** \param [In]  u32RCA                  card relative address
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SelectCard( uint32_t u32RCA )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx = SD_CMD7;
    stcCmd.u32Arg = u32RCA << 16;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1BNORMAL;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send ACMD51 (Send select card command)
 **
 ** \param   [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param   [In]  u32RCA                  Card relative address
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SendSCR( stc_sdcmd_info_t *pstcInfo, uint32_t u32RCA )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    stc_sdcmd_scr_t stcSCR = { 0 };

    stcCmd.u32CmdIdx = SD_ACMD51;
    stcCmd.u32Arg = u32RCA << 16;
    stcCmd.pu8Data = (uint8_t *)&stcSCR;
    stcCmd.u32BlockSize = 8;
    stcCmd.u32BlockCnt = 1;
    stcCmd.enRspType = SdCmdRspR1BNORMAL;
    stcCmd.u8Dir = SdcmdDirRead;

    if (Ok == Sdcmd_Ops_SendAppCmd(pstcInfo->RCA))
    {
        enRet = Sdcmd_SendCmd(&stcCmd);
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send ACMD6 (Send set bus width command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg                  Command Param
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SetBusWdith( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx    = SD_CMD52;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirWrite;

        enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD6 (Send switch function command)
 **
 ** \param [In]  u32Arg                  Command Param
 **
 ** \retval     Ok                   Command sent success
 ** \retval     Error                Command sent failed
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_SwitchFunc( uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx    = SD_CMD6;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD17 (Send read single block command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock           Read start block
 ** \param [Out] pu8BuffOut              Pointer to a buffer to store data
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_ReadSingleBlock( stc_sdcmd_info_t *pstcInfo,
                                       uint32_t u32StartBlock,
                                       uint8_t *pu8BuffOut )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo || NULL == pu8BuffOut)
    {
        return ErrorInvalidParameter;
    }
    else if (pstcInfo->u32MaxSectorNum < u32StartBlock)
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32StartBlock = u32StartBlock << 9;
    }

    stcCmd.u32CmdIdx = SD_CMD17;
    stcCmd.u32Arg = u32StartBlock;
    stcCmd.pu8Data = pu8BuffOut;
    stcCmd.u32BlockSize = BLOCK_SZ;
    stcCmd.u32BlockCnt = 1;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD18 (Send read multi block command)
 **
 * \param [In]  pstcInfo                Pointer to SD card info data structure.
 * \param [In]  u32StartBlock           Read start block
 * \param [In]  u32BlockCnt             Read block count
 * \param [out] pu8BuffOut              Pointer to a buffer to store data
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_ReadMultiBlock( stc_sdcmd_info_t *pstcInfo,
                                      uint32_t u32StartBlock,
                                      uint32_t u32BlockCnt,
                                      uint8_t *pu8BuffOut )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo || NULL == pu8BuffOut)
    {
        return ErrorInvalidParameter;
    }
    else if (((pstcInfo->u32MaxSectorNum - u32StartBlock) < u32BlockCnt)
             || (pstcInfo->u32MaxSectorNum < (u32StartBlock + u32BlockCnt)))
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32StartBlock = u32StartBlock << 9;
    }

    stcCmd.u32CmdIdx = SD_CMD18;
    stcCmd.u32Arg = u32StartBlock;
    stcCmd.pu8Data = pu8BuffOut;
    stcCmd.u32BlockSize = BLOCK_SZ;
    stcCmd.u32BlockCnt = u32BlockCnt;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}
en_result_t Sdio_Block_Byte_Write(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x18000) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    //stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 512;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 12;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write_reg(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn,uint16_t Addr)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
		stcCmd.u32Arg   |= (((Addr) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt  = 4;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Read(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x18000) & 0x0001FFFF) << 9);
    //stcCmd.u32Arg   |= (((0x10000) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 512;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 12;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Read_reg(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn ,uint16_t Addr)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((Addr) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 2;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
#if 0
en_result_t Sdio_Block_Byte_Write1(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10100) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write2(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10200) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write3(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10300) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write4(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10400) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write5(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10500) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write6(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10600) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Write7(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10700) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((1 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
#endif
#if 0
en_result_t Sdio_Block_Byte_Read1(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10100) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Read2(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10200) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
en_result_t Sdio_Block_Byte_Read3(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10300) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}en_result_t Sdio_Block_Byte_Read4(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10400) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}en_result_t Sdio_Block_Byte_Read5(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10500) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}en_result_t Sdio_Block_Byte_Read6(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10600) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}en_result_t Sdio_Block_Byte_Read7(stc_sdcard_config_t *pstcCfg,uint8_t *pu8BuffIn)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    if ((NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    stcCmd.u32CmdIdx = SD_CMD53;
    stcCmd.u32Arg    = (pstcCfg->Byte_Block_Count);
    stcCmd.u32Arg   |= (((0x10700) & 0x0001FFFF) << 9);
    stcCmd.u32Arg   |= (pstcCfg->Op_Code << 26);
    stcCmd.u32Arg   |= (pstcCfg->Block_Mode << 27);
    stcCmd.u32Arg   |= (pstcCfg->Func_Num << 28);
    stcCmd.u32Arg   |= (((0 << 0)) << 31);
    stcCmd.pu8Data   = pu8BuffIn;
    stcCmd.u32BlockSize = 256;//pstcCfg->Byte_Block_Count;
    stcCmd.u32BlockCnt = 1;//pstcCfg->Byte_Block_Count;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirRead;
    enRet = Sdcmd_SendCmd(&stcCmd);
    return enRet;
}
#endif
/**
 ******************************************************************************
 ** \brief Send CMD24 (Send write single block command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock           Write start block
 ** \param [In]  pu8BuffIn               Pointer to a buffer to write data
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_WriteSingleBlock( stc_sdcmd_info_t *pstcInfo,
                                        uint32_t u32StartBlock,
                                        uint8_t *pu8BuffIn )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if ((NULL ==  pstcInfo) || (NULL == pu8BuffIn))
    {
        return ErrorInvalidParameter;
    }
    else if (pstcInfo->u32MaxSectorNum < u32StartBlock)
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32StartBlock = u32StartBlock << 9; /// SDSC card uses byte unit address,multiply by 512
    }

    stcCmd.u32CmdIdx = SD_CMD24;
    stcCmd.u32Arg = u32StartBlock;
    stcCmd.pu8Data = pu8BuffIn;
    stcCmd.u32BlockSize = BLOCK_SZ;
    stcCmd.u32BlockCnt = 1;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD25 (Send write multi block command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock           Write start block
 ** \param [In]  pu8BuffIn               Pointer to a buffer to write data
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_WriteMultiBlock( stc_sdcmd_info_t *pstcInfo,
                                       uint32_t u32StartBlock,
                                       uint32_t u32BlockCnt,
                                       uint8_t *pu8BuffIn )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo || NULL == pu8BuffIn)
    {
        return ErrorInvalidParameter;
    }
    else if (((pstcInfo->u32MaxSectorNum - u32StartBlock) < u32BlockCnt)
             || (pstcInfo->u32MaxSectorNum < (u32StartBlock + u32BlockCnt)))
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32StartBlock = u32StartBlock << 9;
    }

    stcCmd.u32CmdIdx = SD_CMD25;
    stcCmd.u32Arg = u32StartBlock;
    stcCmd.pu8Data = pu8BuffIn;
    stcCmd.u32BlockSize = BLOCK_SZ;
    stcCmd.u32BlockCnt = u32BlockCnt;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD32 (Send erase start address command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock           Erase start block
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_EraseStartAddr( stc_sdcmd_info_t *pstcInfo,
                                      uint32_t u32StartBlock
                                     )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo)
    {
        return ErrorInvalidParameter;
    }
    else if (pstcInfo->u32MaxSectorNum < (u32StartBlock))
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32StartBlock = u32StartBlock << 9; /// SDSC card uses byte unit address,multiply by 512
    }

    stcCmd.u32CmdIdx = SD_CMD32;
    stcCmd.u32Arg = u32StartBlock;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD33 (Send erase end address command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32StartBlock           Erase start block
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_EraseEndAddr( stc_sdcmd_info_t *pstcInfo,
                                    uint32_t u32EndBlock
                                   )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo)
    {
        return ErrorInvalidParameter;
    }
    else if (pstcInfo->u32MaxSectorNum < (u32EndBlock))
    {
        return ErrorInvalidParameter;
    }

    if (SdCardCapacityStand == pstcInfo->enCapacityType)
    {
        u32EndBlock = u32EndBlock << 9;
    }

    stcCmd.u32CmdIdx = SD_CMD33;
    stcCmd.u32Arg = u32EndBlock;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD33 (Send erase command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t Sdcmd_Ops_Erase( stc_sdcmd_info_t *pstcInfo )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    if (NULL == pstcInfo)
    {
        return ErrorInvalidParameter;
    }

    stcCmd.u32CmdIdx = SD_CMD38;
    stcCmd.u32Arg = 0x0;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD13 (Send status command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_sdcmd_cardstatus_t Sdcmd_Ops_SendStatus( stc_sdcmd_info_t *pstcInfo )
{
    stc_sdcmd_cmd_t        stcCmd      = { 0 };
    en_result_t            enRet       = Error;

    stc_sdcmd_cardstatus_t *pstcStatus = NULL;
    if (NULL == pstcInfo)
    {
        return SdCardStateRev;
    }

    stcCmd.u32CmdIdx = SD_CMD13;
    stcCmd.u32Arg = pstcInfo->RCA << 16;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    if (Ok != enRet)
    {
        return SdCardStateRev;
    }

    pstcStatus = (stc_sdcmd_cardstatus_t *)&stcCmd.u32Rsp;

    return (en_sdcmd_cardstatus_t)pstcStatus->CURRENT_STATE;
}

/**
 ******************************************************************************
 ** \brief Send ACMD41 (Send SD operation condition command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg                  Command Param
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t SDIO_ReInit( stc_sdcmd_info_t *pstcInfo,stc_sdcard_config_t *pstcCfg)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    stcCmd.u32CmdIdx = SD_CMD5;
    //stcCmd.u32Arg  = *(uint32_t *)(&pstcInfo->stcOCR);
    stcCmd.u32Arg  = 0x00100000;
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR3R4;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    if (Ok == enRet)
    {
    	memcpy((void *)&pstcInfo->stcOCR, &stcCmd.u32Rsp[0], sizeof(pstcInfo->stcOCR));
    }
    return enRet;
}
en_result_t SDIO_Init( stc_sdcmd_info_t *pstcInfo)
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;
    stcCmd.u32CmdIdx = SD_CMD5;
    stcCmd.u32Arg  = *(uint32_t *)&pstcInfo->stcOCR;
    stcCmd.u32Arg  = (stcCmd.u32Arg & 0x00000000);
    stcCmd.pu8Data = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt = 0;
    stcCmd.enRspType = SdCmdRspR3R4;
    stcCmd.u8Dir = SdcmdDirWrite;
    enRet = Sdcmd_SendCmd(&stcCmd);
    if (Ok == enRet)
    {
      memcpy((void *)&pstcInfo->stcOCR, &stcCmd.u32Rsp[0], sizeof(pstcInfo->stcOCR));
    }
    return enRet;
}
en_result_t Sdcmd_Ops_SdSendOpCond( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx    = SD_ACMD41;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR3R4;
    stcCmd.u8Dir        = SdcmdDirWrite;

    enRet = Sdcmd_Ops_SendAppCmd(pstcInfo->RCA);
     if (Ok == enRet)
    {
        enRet = Sdcmd_SendCmd(&stcCmd);
    }
    else
    {
        return enRet;
    }

    memcpy((void *)&pstcInfo->stcOCR, &stcCmd.u32Rsp[0], sizeof(pstcInfo->stcOCR));

    // query mode
    if (0x0 == u32Arg)
    {}
    else if (0 == (stcCmd.u32Rsp[0] & ARG_ACMD41_BUSY))
    {
        // set mode
        enRet = ErrorOperationInProgress;
    }

    pstcInfo->enCapacityType = SdCardCapacityStand;

    if (0 != (stcCmd.u32Rsp[0] & OCR_CAPACITY_MASK)) //need to verify
    {
        pstcInfo->enCapacityType = SdCardCapacityHigh;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD1 (Send MMC operation condition command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg                  Command Param
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t MMCcmd_Ops_SendOpCond( stc_sdcmd_info_t *pstcInfo, uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx    = SD_CMD1;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR3R4;
    stcCmd.u8Dir        = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    if (Ok == enRet && (0x80000000 & stcCmd.u32Rsp[0]))
    {
        pstcInfo->enCapacityType = SdCardCapacityStand;
        if (stcCmd.u32Rsp[0] & 0x40000000)
        {
            pstcInfo->enCapacityType = SdCardCapacityHigh;
        }
    }
    else
    {
        enRet = ErrorNotReady;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD8 (Send extent CSD command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t MMCcmd_Ops_SendExtCsd( stc_sdcmd_info_t *pstcInfo )
{
    stc_sdcmd_cmd_t stcCmd   = { 0 };
    en_result_t     enRet    = Error;
    uint8_t         u8Extcsd[512] = { 0 };
    uint32_t        *pu32Val = NULL;

    stcCmd.u32CmdIdx    = MMC_CMD8;
    stcCmd.u32Arg       = pstcInfo->RCA << 16;
    stcCmd.pu8Data      = u8Extcsd;
    stcCmd.u32BlockSize = BLOCK_SZ;
    stcCmd.u32BlockCnt  = 1;
    stcCmd.enRspType    = SdCmdRspR1NORMALR5R6R7;
    stcCmd.u8Dir        = SdcmdDirRead;

    enRet = Sdcmd_SendCmd(&stcCmd);

    //usdelay(30000); ///< should wait ,why?
    usdelay(30000); ///< should wait ,why?

    if (Ok == enRet)
    {
        pu32Val = (uint32_t *)&u8Extcsd[212];
        pstcInfo->u32MaxSectorNum = *pu32Val;
    }

    return enRet;
}

/**
 ******************************************************************************
 ** \brief Send CMD1 (Send MMC command set command)
 **
 ** \param [In]  pstcInfo                Pointer to SD card info data structure.
 ** \param [In]  u32Arg                  Command Param
 **
 ** \retval      Ok                      the command sent successfully
 ** \retval      ErrorInvalidParameter   Param is invalid.
 ** \retval      ErrorTimeout            the Command sent failed.
 **
 ******************************************************************************/
en_result_t MMCcmd_Ops_CmdSet( uint32_t u32Arg )
{
    stc_sdcmd_cmd_t stcCmd = { 0 };
    en_result_t     enRet  = Error;

    stcCmd.u32CmdIdx    = SD_CMD6;
    stcCmd.u32Arg       = u32Arg;
    stcCmd.pu8Data      = NULL;
    stcCmd.u32BlockSize = 0;
    stcCmd.u32BlockCnt  = 0;
    stcCmd.enRspType    = SdCmdRspR1BNORMAL;
    stcCmd.u8Dir        = SdcmdDirWrite;

    enRet = Sdcmd_SendCmd(&stcCmd);

    return enRet;
}
#endif

#endif

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
