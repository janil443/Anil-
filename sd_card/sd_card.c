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
 **   - 2015-01-20  1.3  RZh    Modify for FM universal PDL
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <string.h>
#include "sd_cmd.h"
#include "sd_card.h"
#include "rsi_egpio.h"
#include "sdif.h"
#include "rsi_pll.h"

//#include ""
#if 1//(defined(PDL_PERIPHERAL_SD_ACTIVE))
//#define SIMULATION
extern SDIO_CCCR_REG_Info_t  Reg_Info;
#define __M4_SDMEM_TA_INTR_ENABLE \
  __asm__ ("push {r0}\n ldcr r0, ipl\n or r0,#0x10");  \
  __asm__ ("stcr r0,ipl\n pop {r0}\n ":::"r0");

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
static en_sdcmd_cardtype_t SdCardTypeDetect( stc_sdcard_config_t *pstcCfg );
static en_result_t SdCardInitHost( stc_sdcard_config_t *pstcCfg );
static en_result_t SdCardInitSd( stc_sdcard_config_t *pstcCfg );
en_result_t SDIO_Init( stc_sdcmd_info_t *pstcCfg );
static en_result_t SdCardIniteMMC( stc_sdcard_config_t *pstcCfg );
static en_result_t SdInitPins( void );
/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/******************************************************************************/
/* Local Functions                                                            */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Initialize SDIO pins
 **
 ** \retval Ok   SDIO pins initialized successfully
 **
 ******************************************************************************/

#define PAD_CONFIG_REG(x)           (* (volatile uint32_t *) (0x46004000 +4*x))    /*!< REN enable bit(this should be enable)                              */


#define DDR_PAD_CONFIG_REG(x)           (* (volatile uint32_t *) (0x46006000  +4*x))    /*!< REN enable bit(this should be enable)                              */

#define M4_BASE_ADDR  0x46008000

/** 
 * @def _GLOBAL_INTR_DISABLE_
 * IPL disable
 */
#ifndef _GLOBAL_INTR_DISABLE_
#define _GLOBAL_INTR_DISABLE_ \
  __asm__ ("id\nnop\nnop\nnop\n");
#endif

/** 
 * @def _GLOBAL_INTR_ENABLE_
 * IPL enable
 */

#ifndef _GLOBAL_INTR_ENABLE_
#define _GLOBAL_INTR_ENABLE_ \
  __asm__ ("ie\nnop\nnop\nnop\n");
#endif
static en_result_t SdInitPins( void )
{
	//int i;
#ifdef SPAN_BOARD

#if ( PDL_DEVICE_SERIES_MB9BF56X == PDL_MCU_SERIES || \
		PDL_DEVICE_SERIES_S6E2CCX == PDL_MCU_SERIES || \
		PDL_DEVICE_SERIES_S6E2GMX == PDL_MCU_SERIES || \
		PDL_DEVICE_SERIES_S6E2HGX == PDL_MCU_SERIES \
)
	SetPinFunc_S_CMD_0();
	SetPinFunc_S_CLK_0();
	SetPinFunc_S_CD_0();
	SetPinFunc_S_WP_0();
	SetPinFunc_S_DATA0_0();
	SetPinFunc_S_DATA1_0();
	SetPinFunc_S_DATA2_0();
	SetPinFunc_S_DATA3_0();
#elif( PDL_DEVICE_SERIES_S6E2DHX == PDL_MCU_SERIES )

	SetPinFunc_S_CMD();
	SetPinFunc_S_CLK();
	SetPinFunc_S_CD();
	SetPinFunc_S_WP();
	SetPinFunc_S_DATA0();
	SetPinFunc_S_DATA1();
	SetPinFunc_S_DATA2();
	SetPinFunc_S_DATA3();
#endif
#else 
#ifdef SIMULATION

#else
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 13);
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 14);
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 15);
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 16);
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 17);
	 //(*(volatile uint32_t *)0x46008044) |= (1 << 18);
	 //   RSI_EGPIO_SetPinMux(&EGPIO ,1,9 ,EGPIO_PIN_MUX_MODE8);   // GPIO[46] : M4SS_SMIH_CLK
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,2,4 ,EGPIO_PIN_MUX_MODE4);   // GPIO[46] : M4SS_SMIH_CLK
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,2,5 ,EGPIO_PIN_MUX_MODE5);   // GPIO[46] : M4SS_SMIH_CLK
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,1,10 ,EGPIO_PIN_MUX_MODE8);   // GPIO[47] : M4SS_SMIH_WP
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,1,11 ,EGPIO_PIN_MUX_MODE8);    // GPIO[48] : M4SS_SMIH_CD_N
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,1,13 ,EGPIO_PIN_MUX_MODE8);    // GPIO[49] : M4SS_SMIH_CMD
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,1,14 ,EGPIO_PIN_MUX_MODE8);    // GPIO[50] : M4SS_SMIH_D0
	 //	RSI_EGPIO_SetPinMux(&EGPIO ,3,1 ,EGPIO_PIN_MUX_MODE8);    // GPIO[51] : M4SS_SMIH_D1	
#endif
	 
#endif
	return Ok;
}

/**
 ******************************************************************************
 ** \brief Initialize SDIO Host
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval Ok                      SDIO host initialized successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/

#define SD_PCG 0x46000000
#define PCG_ENABLE      (1UL<<29)
#define CLK_ENABLE_CLEAR_REG1       (0x46000000 + 0x04)

static boolean_t SdEnablePCG(void)
{

	uint32_t u32psg = 0;
	volatile uint32_t * p32Sdpcg = (uint32_t *)SD_PCG;

	*p32Sdpcg |= PCG_ENABLE;
	u32psg = *p32Sdpcg;
	if(PCG_ENABLE == (u32psg & PCG_ENABLE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Clk_Rs_9116_PeripheralClockEnable(void)
{
	/*clock enable */
	SdEnablePCG();

}
en_result_t SdioInitHost( stc_sdcard_config_t *pstcCfg )
{
	en_result_t              enRet          = Error;
	stc_sdif_config_t        stcConfig      = { 0 };
	stc_sdif_err_irq_cb_t    stcErrIrqCb    = { 0 };
	stc_sdif_err_irq_en_t    stcErrIrqEn    = { 0 };
	stc_sdif_normal_irq_en_t stcNormalIrqEn = { 0 };
	stc_sdif_normal_irq_cb_t stcNormalIrqCb = { 0 };
	stc_sdcmd_config_t       stcCmdCfg;
    uint16_t clock = 0;
    uint16_t power = 0;


	if (NULL == pstcCfg)
	{
		return ErrorInvalidParameter;
	}

	PDL_ZERO_STRUCT(stcCmdCfg);

	Clk_Rs_9116_PeripheralClockEnable();

#if 0
	while (FALSE == Sdif_PollCardInsert(SDIF))
	{
		;
	}
#endif
	// Initialize interrupts
	stcErrIrqEn.stcErrIrqStatus.CmdTimeoutErr = TRUE;
	stcErrIrqEn.stcErrIrqStatus.DataEndBitErr = TRUE;
	stcErrIrqEn.stcErrIrqStatus.AdmaErr = TRUE;
	stcErrIrqEn.stcErrIrqStatus.DataEndBitErr = TRUE;

	stcErrIrqCb.pfnCmdTimeoutErrIrqCb = SdcmdEventCmdTimeOutCbk;

	stcNormalIrqEn.stcNormalIrqStatus.BufferReadReady = TRUE;
	stcNormalIrqEn.stcNormalIrqStatus.BufferWriteReady = TRUE;
	stcNormalIrqEn.stcNormalIrqStatus.CommandComplete = TRUE;
	stcNormalIrqEn.stcNormalIrqStatus.TransferComplete = TRUE;
	stcNormalIrqEn.stcNormalIrqStatus.CardRemoval = TRUE;
    stcNormalIrqEn.stcNormalIrqStatus.CardInsertion = 0;
    stcNormalIrqEn.stcNormalIrqStatus.DmaIrq = TRUE;

	stcNormalIrqCb.pfnBufferReadReadyIrqCb = SdcmdEventBufferReadRdyCbk;
	stcNormalIrqCb.pfnBufferWriteReadyIrqCb = SdcmdEventBufferWriteRdyCbk;
	stcNormalIrqCb.pfnCommandCompleteIrqCb = SdcmdEventCmdCompleteCbk;
	stcNormalIrqCb.pfnTransferCompleteIrqCb = SdcmdEventTransCompleteCbk;
	stcNormalIrqCb.pfnCardRemovalIrqCb = SdcmdEventCardRemoveCbk;

	// Initialize configuration
	stcConfig.bEnable4BitMode = TRUE;
	stcConfig.bEnableHighSpeed = TRUE;
	stcConfig.bSelAdma = pstcCfg->bUsingADMA;
	stcConfig.pstcErrIrqCb = &stcErrIrqCb;
	stcConfig.pstcErrIrqEn = &stcErrIrqEn;
	stcConfig.pstcNormalIrqCb = &stcNormalIrqCb;
	stcConfig.pstcNormalIrqEn = &stcNormalIrqEn;
	stcConfig.bTouchNvic = TRUE;

	enRet = Sdif_Init(SDIF, &stcConfig);


	Sdif_SelBusVoltage(SDIF, SdifVoltage33v);
	Sdif_BusPowerOn(SDIF);


    __M4_SDMEM_TA_INTR_ENABLE;  //Unmask only IRQ 4 for TA.

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("ie");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

	enRet = SdCardChangeClock(25*1000*1000);
	//enRet = SdCardChangeClock(SdClk400K);
	if (Ok != enRet)
	{
		return enRet;
	}
	stcCmdCfg.bHighSpeedMode = pstcCfg->bHighSpeedMode;
	stcCmdCfg.bUsingADMA = pstcCfg->bUsingADMA;
	stcCmdCfg.enBusWidth = pstcCfg->enBusWidth;
	stcCmdCfg.u32Clock = pstcCfg->u32Clock;

	enRet = Sdcmd_Init(&stcCmdCfg);

	return enRet;
}

/**
 ******************************************************************************
 ** \brief Detect SD card types
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval SdCardTypeNone          No card is detected
 ** \retval SdCardTypeSD            SD card is detected
 ** \retval SdCardTypeSDIO          SDID card is detected
 ** \retval SdCardTypeEMMC          EMMC card is detected
 **
 ******************************************************************************/
static en_sdcmd_cardtype_t SdCardTypeDetect( stc_sdcard_config_t *pstcCfg )
{
	en_result_t         enRet      = Error;
	en_sdcmd_cardtype_t enCardType = SdCardTypeNone;
	stc_sdcmd_info_t    *pstcInfo  = NULL;
	if (NULL == pstcCfg)
	{
		return SdCardTypeNone;
	}
	pstcInfo = &pstcCfg->stcInfo;
	// 1. Reset Card
	enRet = Sdcmd_Ops_GoIdle();

	if (Ok != enRet)
	{
		return SdCardTypeNone;
    }

    enRet = SDIO_Init (pstcInfo); // 2. Check SDIO type. (CMD5 )

	if (Ok != enRet)
	{
		return SdCardTypeNone;
    }



	// 3. check MMC/SD
	enRet = Sdcmd_Ops_SdSendOpCond(pstcInfo, 0x0);
	if (ErrorTimeout == enRet)
	{
		enCardType = SdCardTypeEMMC;
	}
	else if (Ok == enRet)
	{
		enCardType = SdCardTypeSD;
	}
	else
	{
		enCardType = SdCardTypeNone;
	}
	return enCardType;
}

/**
 ******************************************************************************
 ** \brief Initialize SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval Ok                      SD card initialized successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t SdioInitSd( stc_sdcard_config_t *pstcCfg )
{
    en_result_t      enRet       = Error;
    stc_sdcmd_info_t *pstcSdInfo = NULL;
    uint8_t          u8F8        = 0;
    uint32_t         u32Retry    = 0;
    uint32_t         u32Arg      = 0;

    pstcSdInfo = &pstcCfg->stcInfo;
init:
    // 1. Reset Card
    enRet = Sdcmd_Ops_GoIdle();

    if (Ok != enRet)
    {
        goto init;
    }
    // 2.  (CMD5 ) .Get Op-Voltage
    enRet = SDIO_Init(pstcSdInfo);
    if (Ok != enRet)
    {
        goto init;
    }

    // 3.  (CMD5 ) .Set Op-Voltage & get OCR
    enRet = SDIO_ReInit(pstcSdInfo,pstcCfg);
    if (Ok != enRet)
    {
        return enRet;
    }
    if(!(pstcSdInfo->stcOCR.u32CARD_READY))
    {
        goto init;
    }
    if((pstcSdInfo->stcOCR.u32MEM_PRSNT))
    {
        return CARD_TYPE_MEMCARD;
    }
    if(!(pstcSdInfo->stcOCR.u32I_O_NUM))
    {
        return ErrorInvalidMode;
    }

    // 4.  (CMD3 ) .Get RCA
    enRet = Sdcmd_Ops_SendRelativeAddr(pstcSdInfo, 0x0);
    if (Ok != enRet)
    {
        return enRet;
    }
    // 5. (CMD7) .Select Card
    #if 1
    enRet = Sdcmd_Ops_SelectCard(pstcSdInfo->RCA);
    if (Ok != enRet)
    {
        return enRet;
    }
#endif
    enRet = Sdio_Card_Command(pstcSdInfo,CD_DISABLE_ARG);
    if (Ok != enRet)
    {
        return enRet;
    }
bus_send_again:

    enRet = Sdio_Card_Command(pstcSdInfo,LOW_SPEED_CHECK_ARG);
    if(pstcSdInfo->Response[0] & LOW_SPEED_CHECK)
    {
        if(
                pstcSdInfo->Response[0] & BIT4_MODE_CHECK)
        {
            if (SdBusWidth4 == pstcCfg->enBusWidth)
            {
                enRet = Sdcmd_SDChangeBusWidth(pstcSdInfo, SdBusWidth4);
            }
        }
        else
        {
        }
    }
    else
    {
        if (SdBusWidth4 == pstcCfg->enBusWidth)
        {
            enRet = Sdcmd_SDChangeBusWidth(pstcSdInfo, SdBusWidth4);
            if(enRet != Ok)
            {
            }
        }
    }
    // Enable Function.
    while(1)
    {
        enRet = Sdio_Card_Command(pstcSdInfo,FUCNTION1_CHECK_ARG);
        if (Ok == enRet)
        {
            if(!((pstcSdInfo->Response[0]) & FUNC1_ENABLE))
            {
                enRet = Sdio_Card_Command(pstcSdInfo,FUCNTION1_ENB_ARG);
                if(Ok != enRet)
                {
                    return enRet;
                }
                else
                {
                    break;
                }
            }
            else
                break;
        }
    }
    // I/O Function Ready
    while(1)
    {
        enRet = Sdio_Card_Command(pstcSdInfo,FUCNTION1_READY_ARG);
        if (Ok == enRet)
        {
            if(pstcSdInfo->Response[0] & FUNC1_READY)
            {
                break;
            }
        }
    }
    // Interrupt Enable Master & Interrupt Enable for function.
    enRet = Sdio_Card_Command(pstcSdInfo,FUNC1_INTR_ENB_ARG);
    if (Ok != enRet)
    {
        return enRet;
    }
    //Check if Interrupts are Enabled or not.
    enRet = Sdio_Card_Command(pstcSdInfo,FUNC1_INTR_CHECK_ARG);
    if (Ok != enRet)
    {
        return enRet;
    }
    //Check for 1-bit or 4-bit mode of I/O. 
    enRet = Sdio_Card_Command(pstcSdInfo,BIT4_BUS_WIDTH_SET_ARG);
    if (Ok != enRet)
    {
        return enRet;
    }
    if (SdBusWidth4 == pstcCfg->enBusWidth)
    {
        if(!(pstcSdInfo->Response[0] & BUS_BIT))
        {
            goto bus_send_again;
        }
    }
    //Check for CD Disable
    if(!(pstcSdInfo->Response[0] & (1 << 7)))
    {
        return 0;
    }
    enRet = Sdio_Card_Command(pstcSdInfo,CSA_SUPPORT_ARG);
    if (Ok == enRet)
    {
        if(pstcSdInfo->Response[0] & (1 << 6))
        {
            enRet = Sdio_Card_Command(pstcSdInfo,CSA_ENABLE_ARG);
            if (Ok != enRet)
            {
                return enRet;
            }
        }
        enRet = Sdio_Card_Command(pstcSdInfo,I_O_BLOCK_SIZE_ARG);
        if (Ok != enRet)
        {
            return enRet;
        }
        enRet = Sdio_Card_Command(pstcSdInfo,I_O_BLOCK_SIZE_ARG_1);
        if (Ok != enRet)
        {
            return enRet;
        }
    }
    return 0;
}

/**
 ******************************************************************************
 ** \brief Initialize eMMC card
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval Ok                      eMMC card initialized successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
 #if 0 
static en_result_t SdCardIniteMMC( stc_sdcard_config_t *pstcCfg )
{
	en_result_t      enRet        = Ok;
	stc_sdcmd_info_t *pstcMMCInfo = NULL;

	pstcMMCInfo = &pstcCfg->stcInfo;

	if (NULL == pstcCfg)
	{
		return ErrorInvalidParameter;
	}
	SDIF->SSRST_f.SWRSTCMDLN = 1u;
	while (SDIF->SSRST_f.SWRSTCMDLN != 0u);

	// 1. Reset Card  card CMD 0
	enRet = Sdcmd_Ops_GoIdle();
	if (Ok != enRet)
	{
		return enRet;
	}
	enRet = Error;
	while (Ok != enRet)
	{
		enRet = MMCcmd_Ops_SendOpCond(pstcMMCInfo, 0x40FF8000);
	}
	// CMD2.Get CID, CID is about manufactor information. ignore.
	enRet = Sdcmd_Ops_AllSendCid(pstcMMCInfo, 0x0);
	if (Ok != enRet)
	{
		return enRet;
	}
    usdelay(1000); //why need a big delay.
	// step .Get RCA
	enRet = Sdcmd_Ops_SendRelativeAddr(pstcMMCInfo, 0x0);
	if (Ok != enRet)
	{
		return enRet;
	}

	// step   Select Card
	enRet = Sdcmd_Ops_SelectCard(pstcMMCInfo->RCA);
	if (Ok != enRet)
	{
		return enRet;
	}

	enRet = MMCcmd_Ops_SendExtCsd(pstcMMCInfo);
	if (Ok != enRet)
	{
		return enRet;
	}

	if (SdBusWidth4 == pstcCfg->enBusWidth)
	{
		enRet = Sdcmd_MMCChangeBusWidth(pstcMMCInfo, SdBusWidth4);

		if (Ok != enRet)
		{
			return enRet;
		}
        usdelay(5000);
	}

	if (TRUE == pstcCfg->bHighSpeedMode)
	{
		enRet = Sdcmd_MMCEnableHighSpeed(TRUE, pstcCfg->u32Clock);
		if(Ok != enRet)
		{
			return enRet;
		}
	}
    usdelay(10000);
	return enRet;
}
#endif

/******************************************************************************/
/* Global Functions                                                           */
/******************************************************************************/
/**
 ******************************************************************************
 ** \brief Initialize card according to the card card detected
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval Ok                      Card initialized successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t RSI_Sdio_Init(stc_sdcard_config_t *pstcCfg )
{
	en_sdcmd_cardtype_t enType = SdCardTypeNone;
	en_result_t         enRet  = Error;

	enRet = SdioInitHost(pstcCfg);

	enRet = SdioInitSd(pstcCfg);

 	return enRet;
}

/**
 ******************************************************************************
 ** \brief Read a single block data from SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 ** \param u32BlockIdx  Block index
 ** \param pu8BuffOut   Pointer to the buffer containing read data
 **
 ** \retval Ok                      A single block is read successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t Sdcard_Read_SingleBlock( stc_sdcard_config_t *pstcCfg,
		uint32_t u32BlockIdx,
		uint8_t *pu8BuffOut )
{
	en_result_t     enRet = Error;

	if (NULL == pstcCfg || NULL == pu8BuffOut)
	{
		return ErrorInvalidParameter;
	}

	enRet = Sdcmd_Ops_ReadSingleBlock(&pstcCfg->stcInfo, u32BlockIdx, pu8BuffOut);

	return enRet;
}





/**
 ******************************************************************************
 ** \brief Read multi-blocks data of SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 ** \param u32BlockIdx  Block index
 ** \param u32Count     Block count
 ** \param pu8BuffOut   Pointer to the buffer containing read data
 **
 ** \retval Ok                      A single block is read successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t Sdcard_Read_MultiBlock( stc_sdcard_config_t *pstcCfg,
		uint32_t u32BlockIdx,
		uint32_t u32Count,
		uint8_t *pu8BuffOut )
{
	en_result_t     enRet = Error;

	if (NULL == pstcCfg || NULL == pu8BuffOut)
	{
		return ErrorInvalidParameter;
	}

	enRet = Sdcmd_Ops_ReadMultiBlock(&pstcCfg->stcInfo, u32BlockIdx, u32Count, pu8BuffOut);

	return enRet;
}

/**
 ******************************************************************************
 ** \brief Write a single block data of SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 ** \param u32BlockIdx  Block index
 ** \param pu8BuffOut   Pointer to the buffer containing read data
 **
 ** \retval Ok                      A single block is written successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t Sdcard_Write_SingleBlock( stc_sdcard_config_t *pstcCfg,
		uint32_t u32BlockIdx,
		uint8_t *pu8BuffIn )
{
	en_result_t     enRet = Error;

	if (NULL == pstcCfg || NULL == pu8BuffIn)
	{
		return ErrorInvalidParameter;
	}

	enRet = Sdcmd_Ops_WriteSingleBlock(&pstcCfg->stcInfo, u32BlockIdx, pu8BuffIn);

	return enRet;

}

/**
 ******************************************************************************
 ** \brief Write multi-blocks data into SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 ** \param u32BlockIdx  Block index
 ** \param pu8BuffOut   Pointer to the buffer containing read data
 **
 ** \retval Ok                      Multi-block data is written successfully
 ** \retval ErrorInvalidParameter   If one of following conditions macths:
 **                                 - NULL == pstcCfg
 **                                 - Other invalid configuration
 **
 ******************************************************************************/
en_result_t Sdcard_Write_MultiBlock( stc_sdcard_config_t *pstcCfg,
		uint32_t u32BlockIdx,
		uint32_t u32Count,
		uint8_t *pu8BuffIn )
{
	en_result_t     enRet = Error;

	if (NULL == pstcCfg || NULL == pu8BuffIn)
	{
		return ErrorInvalidParameter;
	}

	enRet = Sdcmd_Ops_WriteMultiBlock(&pstcCfg->stcInfo, u32BlockIdx, u32Count, pu8BuffIn);

	return enRet;
}

/**
 * \brief       Sdcard_GetStatus
 *
 * \param [In]  pstcCfg
 *
 * \retval      en_sdcmd_cardstatus_t
 */
/**
 ******************************************************************************
 ** \brief Get the status of SD card
 **
 ** \param pstcCfg Pointer to SD card configuration
 **
 ** \retval SD card status see the enumeration type #en_sdcmd_cardstatus_t
 **
 ******************************************************************************/
en_sdcmd_cardstatus_t Sdcard_GetStatus( stc_sdcard_config_t *pstcCfg )
{
	en_sdcmd_cardstatus_t enState = SdCardStateRev;

	if (NULL == pstcCfg)
	{
		return enState;
	}

	enState = Sdcmd_Ops_SendStatus(&pstcCfg->stcInfo);

	return enState;
}

#endif

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
