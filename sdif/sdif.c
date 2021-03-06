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
/** \file sdif.c
 **
 ** A detailed description is available at 
 ** @link SdifGroup SDIF description @endlink
 **
 ** History:
 **   - 2015-02-28  1.0  EZh First version for FM Universal PDL.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "sdif.h"
#include "sd_cmd.h"

#if 1//(defined(PDL_PERIPHERAL_SD_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup SdifGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
/// Look-up table for all enabled ADC instances and their internal data
stc_sdif_instance_data_t m_astcSdifInstanceDataLut[SDIF_INSTANCE_COUNT] =
{
#if (PDL_PERIPHERAL_ENABLE_SD0 == PDL_ON)
    { 
        &SDIF0,  // pstcInstance
        { {
            0u,
            0u,
        }  }         
    },
#endif

};

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/
#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)
static void SdifInitNvic(volatile stc_sdifn_t* pstcSdif);
static void SdifDeInitNvic(volatile stc_sdifn_t* pstcSdif);
static stc_sdif_intern_data_t* SdifGetInternDataPtr(volatile stc_sdifn_t* pstcSdif);
#endif

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/******************************************************************************/
/* Local Functions                                                            */
/******************************************************************************/

#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON) 
/**
 ******************************************************************************
 ** \brief SDIF instance interrupt service routine
 **
 ** SDIF instance interrupt service routine, clear interrupt cause and
 **
 ** \param  pstcSdif              Pointer to SDIF instance
 ** \param  pstcSdifInternData    Pointer to SDIF intern data
 **        
 ******************************************************************************/
volatile uint16_t irq_status;
void SdifIrqHandler( volatile stc_sdifn_t*   pstcSdif,
                     stc_sdif_intern_data_t* pstcSdifInternData)
{

    stc_sdif_snintst_field_t *SNINTST_f = &pstcSdif->offset_30;
    stc_sdif_seintst_field_t *SEINTST_f = &pstcSdif->offset_32;
     /* Normal interrupt handler */

    if (1u == SNINTST_f->CMDCMPLT)   // Command complete
    {
        pstcSdif->offset_30 = 0x0001u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnCommandCompleteIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnCommandCompleteIrqCb();
        }
    }
    if (1u == SNINTST_f->TRSFCMPLT)   // Transfer complete
    {
        pstcSdif->offset_30 = 0x0002u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnTransferCompleteIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnTransferCompleteIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->BLCKGEVNT)   // block gap event
    {
        pstcSdif->offset_30 = 0x0004u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnBlockGapIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnBlockGapIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->DMAINT)   // DMA interrupt
    {
        pstcSdif->offset_30 = 0x0008u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnDmaIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnDmaIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->BUFWRRDY)   // Buffer write ready
    {
        pstcSdif->offset_30 = 0x0010u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnBufferWriteReadyIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnBufferWriteReadyIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->BUFRDRDY)   // Buffer read ready
    {
        pstcSdif->offset_30 = 0x0020u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnBufferReadReadyIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnBufferReadReadyIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->CARDINS)   // Card insetion
    {
        pstcSdif->offset_30 = 0x0040u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnCardInsertIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnCardInsertIrqCb();
        }
    }
    
    if (1u == SNINTST_f->CARDRMV)   // Card removal
    {
        pstcSdif->offset_30 = 0x0080u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnCardRemovalIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnCardRemovalIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SNINTST_f->CARDINT)   // Card interrupt
    {
        pstcSdif->offset_30 = 0x0100u; // Clear interrupt flag
        irq_status =  pstcSdif->offset_30;

        if (NULL != pstcSdifInternData->stcNormalIrqCb.pfnCardIrqCb)
        {
            pstcSdifInternData->stcNormalIrqCb.pfnCardIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    /* Error interrupt handler */
    if (1u == SEINTST_f->CMDTOERR)   // Command timeout error
    {
        pstcSdif->offset_32 = 0x0001u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnCmdTimeoutErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnCmdTimeoutErrIrqCb();
        } 
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->CMDCRCERR)   // Command CRC error
    {
        pstcSdif->offset_32 = 0x0002u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnCmdCrcErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnCmdCrcErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->CMDEBERR)   // Command end bit error
    {
        pstcSdif->offset_32 = 0x0004u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnCmdEndBitErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnCmdEndBitErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->CMDIDXERR)   // Command index error
    {
        pstcSdif->offset_32 = 0x0008u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnCmdIndexErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnCmdIndexErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->DTTOERR)   // Data timeout error
    {
        pstcSdif->offset_32 = 0x0010u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnDataTimeoutErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnDataTimeoutErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->DTEBERR)   // Data end bit error
    {
        pstcSdif->offset_32 = 0x0020u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnDataEndBitErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnDataEndBitErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->DTCRCERR)   // Data CRC error
    {
        pstcSdif->offset_32 = 0x0040u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnDataCrcErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnDataCrcErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->CRTLMTERR)   // Current limitation error
    {
        pstcSdif->offset_32 = 0x0080u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnCurrentLimitErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnCurrentLimitErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->ACMD12ERR)   // Auto CMD12 error
    {
        pstcSdif->offset_32 = 0x0100u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnAutoCmdErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnAutoCmdErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->ADMAERR)   // ADMA error
    {
        pstcSdif->offset_32 = 0x0200u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnAdmaErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnAdmaErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    if (1u == SEINTST_f->TUNINGERR)   // Tuning error
    {
        pstcSdif->offset_32 = 0x0400u;
        irq_status =  pstcSdif->offset_32;

        if (NULL != pstcSdifInternData->stcErrIrqCb.pfnTuningErrIrqCb)
        {
            pstcSdifInternData->stcErrIrqCb.pfnTuningErrIrqCb();
        }
        else {
                asm("debug");
        }
    }
    
    return;
} // SdifIrqHandler

/**
 ******************************************************************************
 ** \brief Set NVIC Interrupt depending on SDIF instance
 **
 ** \param pstcSdif Pointer to SDIF instance
 **
 ******************************************************************************/
static void SdifInitNvic(volatile stc_sdifn_t* pstcSdif)
{
     NVIC_ClearPendingIRQ(SD_IRQn);
    NVIC_EnableIRQ(SD_IRQn);
    NVIC_SetPriority(SD_IRQn, PDL_IRQ_LEVEL_SD);   
} // SdifInitNvic

/**
 ******************************************************************************
 ** \brief Clear NVIC Interrupt depending on SDIF instance
 **
 ** \param pstcSdif Pointer to SDIF instance
 **
 ******************************************************************************/
static void SdifDeInitNvic(volatile stc_sdifn_t* pstcSdif)
{
    NVIC_ClearPendingIRQ(SD_IRQn);
    NVIC_DisableIRQ(SD_IRQn);
    NVIC_SetPriority(SD_IRQn, PDL_DEFAULT_INTERRUPT_LEVEL);   

} // SdifDeInitNvic
#endif // #if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain SDIF instance.
 **
 ** \param pstcSdif Pointer to SDIF instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_sdif_intern_data_t* SdifGetInternDataPtr(volatile stc_sdifn_t* pstcSdif) 
{
    uint8_t u8Instance;
    
    for (u8Instance = 0u; u8Instance < SDIF_INSTANCE_COUNT; u8Instance++)
    {
        if (pstcSdif == m_astcSdifInstanceDataLut[u8Instance].pstcInstance)
        {
            return &m_astcSdifInstanceDataLut[u8Instance].stcInternData;
        }
    }

    return NULL;
}


/******************************************************************************/
/* Global Functions                                                           */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Initialize SD interface
 **
 ** This function initializes an SDIF module
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  pstcConfig       SDIF module configuration 
 **
 ** \retval Ok                    SDIF initialized normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - pstcConfig == NULL
 **                               - Other invalid configuration
 ******************************************************************************/
en_result_t  Sdif_Init( volatile stc_sdifn_t* pstcSdif,
                        stc_sdif_config_t*    pstcConfig )
{
	
    // Pointer to internal data
    stc_sdif_intern_data_t* pstcSdifInternData ; 
		//dbg_printf_obe_final("Sdif_Init()\n");  
    // Check for NULL pointer
    if ((NULL == pstcSdif) ||
        (NULL == pstcConfig))
    {
        return ErrorInvalidParameter ;
    } 
    
    // Get pointer to internal data structure ...
    pstcSdifInternData = SdifGetInternDataPtr( pstcSdif ) ;
    
    // Check for instance available or not
    if(NULL == pstcSdifInternData)
    {
        return ErrorInvalidParameter ;
    }
    
    // Select ADMA or not
    //pstcSdif->SHCTL1_f.DMASEL = ((TRUE == pstcConfig->bSelAdma) ? 2u : 0u);
    if(pstcConfig->bSelAdma) {
        //pstcSdif->offset_28 |= (2u << 3);
        pstcSdif->offset_28 |= BIT(4);
    } else {
        pstcSdif->offset_28 &= ~(0x18);
    }
    
    // Set speed mode
   // pstcSdif->SHCTL1_f.HIGHSPDEN = ((TRUE == pstcConfig->bEnableHighSpeed) ? 1u : 0u);
    if(pstcConfig->bEnableHighSpeed) {
        pstcSdif->offset_28 |= BIT(2); 
    } else {
        pstcSdif->offset_28 &= ~BIT(2); 
    }
    // Set bit width
    //pstcSdif->SHCTL1_f.DATAWIDTH = ((TRUE == pstcConfig->bEnable4BitMode) ? 1u : 0u);
    if(pstcConfig->bEnable4BitMode) {
        pstcSdif->offset_28 |= BIT(1); 
    } else {
        pstcSdif->offset_28 &= ~BIT(1); 
    }
    
    // Enable all status
    //pstcSdif->SNINTSTE = 0x1FFFu;
    pstcSdif->offset_34 = 0x1FFFu;

    //pstcSdif->SEINTSTE = 0x07FFu;
    pstcSdif->offset_36 = 0x07FFu;

#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)    
    // Enable normal interrupt signal
    if(NULL != pstcConfig->pstcNormalIrqEn)
    {
        //pstcSdif->SNINTSGE = pstcConfig->pstcNormalIrqEn->u16NormalStatus;
        pstcSdif->offset_38 =  pstcConfig->pstcNormalIrqEn->u16NormalStatus;
    }
    
    // Set normal interrupt callback functions
    if(NULL != pstcConfig->pstcNormalIrqCb)
    {
        pstcSdifInternData->stcNormalIrqCb.pfnCommandCompleteIrqCb      = pstcConfig->pstcNormalIrqCb->pfnCommandCompleteIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnTransferCompleteIrqCb     = pstcConfig->pstcNormalIrqCb->pfnTransferCompleteIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnBlockGapIrqCb             = pstcConfig->pstcNormalIrqCb->pfnBlockGapIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnDmaIrqCb                  = pstcConfig->pstcNormalIrqCb->pfnDmaIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnBufferWriteReadyIrqCb     = pstcConfig->pstcNormalIrqCb->pfnBufferWriteReadyIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnBufferReadReadyIrqCb      = pstcConfig->pstcNormalIrqCb->pfnBufferReadReadyIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnCardInsertIrqCb           = pstcConfig->pstcNormalIrqCb->pfnCardInsertIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnCardRemovalIrqCb          = pstcConfig->pstcNormalIrqCb->pfnCardRemovalIrqCb;
        pstcSdifInternData->stcNormalIrqCb.pfnCardIrqCb                 = pstcConfig->pstcNormalIrqCb->pfnCardIrqCb;
    }
    
    // Enable error interrupt signal
    if(NULL != pstcConfig->pstcErrIrqEn)
    {
        //pstcSdif->SEINTSGE = pstcConfig->pstcErrIrqEn->u16ErrStatus;
        pstcSdif->offset_3a = pstcConfig->pstcErrIrqEn->u16ErrStatus;
    }
    
    // Set error interrupt callback functions
    if(NULL != pstcConfig->pstcErrIrqCb)
    {
        pstcSdifInternData->stcErrIrqCb.pfnCmdTimeoutErrIrqCb       = pstcConfig->pstcErrIrqCb->pfnCmdTimeoutErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnCmdCrcErrIrqCb           = pstcConfig->pstcErrIrqCb->pfnCmdCrcErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnCmdEndBitErrIrqCb        = pstcConfig->pstcErrIrqCb->pfnCmdEndBitErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnCmdIndexErrIrqCb         = pstcConfig->pstcErrIrqCb->pfnCmdIndexErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnDataTimeoutErrIrqCb      = pstcConfig->pstcErrIrqCb->pfnDataTimeoutErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnDataEndBitErrIrqCb       = pstcConfig->pstcErrIrqCb->pfnDataEndBitErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnDataCrcErrIrqCb          = pstcConfig->pstcErrIrqCb->pfnDataCrcErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnCurrentLimitErrIrqCb     = pstcConfig->pstcErrIrqCb->pfnCurrentLimitErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnAutoCmdErrIrqCb          = pstcConfig->pstcErrIrqCb->pfnAutoCmdErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnAdmaErrIrqCb             = pstcConfig->pstcErrIrqCb->pfnAdmaErrIrqCb;
        pstcSdifInternData->stcErrIrqCb.pfnTuningErrIrqCb           = pstcConfig->pstcErrIrqCb->pfnTuningErrIrqCb;
    }
    
    // Set NVIC
    if(TRUE == pstcConfig->bTouchNvic)
    {
        SdifInitNvic(pstcSdif);
    }

#endif
     return Ok;
} // Sdif_Init

/**
 ******************************************************************************
 ** \brief De-Initialize SD interface
 **
 ** This function de-initializes an SDIF module
 **
 ** \param [in]  pstcSdif          SDIF instance
 ** \param [in]  bTouchIrq         Disable IRQ or not
 ** \param [in]  bTouchIrqCb       Reset callback functions in internal data or not
 ** \param [in]  bTouchNvic        Disable NVIC or not
 **
 ** \retval Ok                    SDIF de-initialized normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/
en_result_t  Sdif_DeInit( volatile stc_sdifn_t* pstcSdif, 
                          boolean_t bTouchIrq,
                          boolean_t bTouchIrqCb,
                          boolean_t bTouchNvic)
{
#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)      
    // Pointer to internal data
    stc_sdif_intern_data_t* pstcSdifInternData ; 
    
    // Get pointer to internal data structure ...
    pstcSdifInternData = SdifGetInternDataPtr( pstcSdif ) ;
    
    if(NULL == pstcSdifInternData)
    {
        return ErrorInvalidParameter ;
    }
#endif    
    
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    } 
    
    // Clear clock control and power control registers
    //pstcSdif->SCLKCTL = 0x0000u;
      pstcSdif->offset_2c = 0x0000u;


    //pstcSdif->SPWRCTL = 0x00u;
    pstcSdif->offset_28 &= ~(0xF00); 

#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)        
    if(TRUE == bTouchIrq)
    {
        //pstcSdif->SNINTSGE = 0x0000u;
        pstcSdif->offset_38 = 0x0000u;
        //pstcSdif->SEINTSGE = 0x0000u;
        pstcSdif->offset_3a = 0x0000u;

    }
    
    if(TRUE == bTouchIrqCb)
    {
        pstcSdifInternData->stcNormalIrqCb.pfnCommandCompleteIrqCb      = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnTransferCompleteIrqCb     = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnBlockGapIrqCb             = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnDmaIrqCb                  = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnBufferWriteReadyIrqCb     = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnBufferReadReadyIrqCb      = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnCardInsertIrqCb           = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnCardRemovalIrqCb          = NULL;
        pstcSdifInternData->stcNormalIrqCb.pfnCardIrqCb                 = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnCmdTimeoutErrIrqCb       = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnCmdCrcErrIrqCb           = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnCmdEndBitErrIrqCb        = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnCmdIndexErrIrqCb         = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnDataTimeoutErrIrqCb      = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnDataEndBitErrIrqCb       = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnDataCrcErrIrqCb          = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnCurrentLimitErrIrqCb     = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnAutoCmdErrIrqCb          = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnAdmaErrIrqCb             = NULL;
        pstcSdifInternData->stcErrIrqCb.pfnTuningErrIrqCb           = NULL;
    }
    
    if(TRUE == bTouchNvic)
    {
        SdifDeInitNvic(pstcSdif);
    }
#endif
    
    return Ok;
}

#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON) 
/**
 ******************************************************************************
 ** \brief Enable SDIF normal interrupt
 **
 ** This function enable the SDIF interrupt which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  enNormalIrqSel   SDIF normal interrupt selection
 **
 ** \retval Ok                    Enable normal interrupt normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - enNormalIrqSel out of range
 ******************************************************************************/      
en_result_t  Sdif_EnableNormalIrq( volatile stc_sdifn_t* pstcSdif,
                                   en_sdif_normal_irq_sel_t enNormalIrqSel)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    switch (enNormalIrqSel)
    {
        case SdifCommandCompleteIrq:
            //pstcSdif->SNINTSGE_f.CMDCMPLTG = 1u;
            pstcSdif->offset_38 |= BIT(0);
            break;
        case SdifTransferCompleteIrq:
            //pstcSdif->SNINTSGE_f.TRSFCMPLTG = 1u;
            pstcSdif->offset_38 |= BIT(1);
            break;
        case SdifBlockGapEventIrq:
            //pstcSdif->SNINTSGE_f.BLCKGEVNTG = 1u;
            pstcSdif->offset_38 |= BIT(2);
            break;
        case SdifDmaIrq:
            //pstcSdif->SNINTSGE_f.DMAINTG = 1u;
            pstcSdif->offset_38 |= BIT(3);
            break;
        case SdifBufferWriteReadyIrq:
            //pstcSdif->SNINTSGE_f.BUFWRRDYG = 1u;
            pstcSdif->offset_38 |= BIT(4);
            break;
        case SdifBufferReadReadyIrq:
            //pstcSdif->SNINTSGE_f.BUFRDRDYG = 1u;
            pstcSdif->offset_38 |= BIT(5);
            break;
        case SdifCardInsertionIrq:
            //pstcSdif->SNINTSGE_f.CARDINSG = 1u;
            pstcSdif->offset_38 |= BIT(6);
            break;
        case SdifCardRemovalIrq:
            //pstcSdif->SNINTSGE_f.CARDRMVG = 1u;
            pstcSdif->offset_38 |= BIT(7);
            break;    
        case SdifCardIrq:
            //pstcSdif->SNINTSGE_f.CARDINTG = 1u;
            pstcSdif->offset_38 |= BIT(8);
            break;  
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable SDIF normal interrupt
 **
 ** This function disable the SDIF interrupt which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  enNormalIrqSel   SDIF normal interrupt selection
 **
 ** \retval Ok                    Disable normal interrupt normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - enNormalIrqSel out of range
 ******************************************************************************/      
en_result_t  Sdif_DisableNormalIrq( volatile stc_sdifn_t* pstcSdif,
                                   en_sdif_normal_irq_sel_t enNormalIrqSel)
{
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    
    switch (enNormalIrqSel)
    {
        case SdifCommandCompleteIrq:
            //pstcSdif->SNINTSGE_f.CMDCMPLTG = 0u;
            pstcSdif->offset_38 &= ~BIT(0);
            break;
        case SdifTransferCompleteIrq:
            //pstcSdif->SNINTSGE_f.TRSFCMPLTG = 0u;
            pstcSdif->offset_38 &= ~BIT(1);
            break;
        case SdifBlockGapEventIrq:
            //pstcSdif->SNINTSGE_f.BLCKGEVNTG = 0u;
            pstcSdif->offset_38 &= ~BIT(2);
            break;
        case SdifDmaIrq:
            //pstcSdif->SNINTSGE_f.DMAINTG = 0u;
            pstcSdif->offset_38 &= ~BIT(3);
            break;
        case SdifBufferWriteReadyIrq:
            //pstcSdif->SNINTSGE_f.BUFWRRDYG = 0u;
            pstcSdif->offset_38 &= ~BIT(4);
            break;
        case SdifBufferReadReadyIrq:
            //pstcSdif->SNINTSGE_f.BUFRDRDYG = 0u;
            pstcSdif->offset_38 &= ~BIT(5);
            break;
        case SdifCardInsertionIrq:
            //pstcSdif->SNINTSGE_f.CARDINSG = 0u;
            pstcSdif->offset_38 &= ~BIT(6);
            break;
        case SdifCardRemovalIrq:
            //pstcSdif->SNINTSGE_f.CARDRMVG = 0u;
            pstcSdif->offset_38 &= ~BIT(7);
            break;    
        case SdifCardIrq:
            //pstcSdif->SNINTSGE_f.CARDINTG = 0u;
            pstcSdif->offset_38 &= ~BIT(8);
            break;  
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Enable SDIF error interrupt
 **
 ** This function enable the SDIF error interrupt which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  enErrIrqSel      SDIF error interrupt selection
 **
 ** \retval Ok                    Enable error interrupt normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - enErrIrqSel out of range
 ******************************************************************************/      
en_result_t  Sdif_EnableErrIrq( volatile stc_sdifn_t* pstcSdif,
                                en_sdif_err_irq_sel_t enErrIrqSel)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    switch (enErrIrqSel)
    {
        case SdifCmdTimeoutErrIrq:
            //pstcSdif->SEINTSGE_f.CMDTOERRG = 1u;
            pstcSdif->offset_3a |= BIT(0);
            break;
        case SdifCmdCrcErrIrq:
            //pstcSdif->SEINTSGE_f.CMDCRCERRG = 1u;
            pstcSdif->offset_3a |= BIT(1);
            break;
        case SdifCmdEndBitErrIrq:
            //pstcSdif->SEINTSGE_f.CMDEBERRG = 1u;
            pstcSdif->offset_3a |= BIT(2);
            break;
        case SdifCmdIndexErrIrq:
            //pstcSdif->SEINTSGE_f.CMDIDXERRG = 1u;
            pstcSdif->offset_3a |= BIT(3);
            break;
        case SdifDataTimeoutErrIrq:
            //pstcSdif->SEINTSGE_f.DTTOERRG = 1u;
            pstcSdif->offset_3a |= BIT(4);
            break;
        case SdifDataCrcErrIrq:
            //pstcSdif->SEINTSGE_f.DTCRCERRG = 1u;
            pstcSdif->offset_3a |= BIT(5);
            break;
        case SdifDataEndBitErrIrq:
            //pstcSdif->SEINTSGE_f.DTEBERRG = 1u;
            pstcSdif->offset_3a |= BIT(6);
            break;
        case SdifCurrentLimitErrIrq:
            //pstcSdif->SEINTSGE_f.CRTLMTERRG = 1u;
            pstcSdif->offset_3a |= BIT(7);
            break;    
        case SdifAutoCmdErrIrq:
            //pstcSdif->SEINTSGE_f.ACMD12ERRG = 1u;
            pstcSdif->offset_3a |= BIT(8);
            break;  
        case SdifAdmaErrIrq:
            //pstcSdif->SEINTSGE_f.ADMAERRG = 1u;
            pstcSdif->offset_3a |= BIT(9);
            break;  
        case SdifTuningErrIrq:
            //pstcSdif->SEINTSGE_f.TUNINGERRG = 1u;
            pstcSdif->offset_3a |= BIT(10);
            break;              
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable SDIF error interrupt
 **
 ** This function disable the SDIF error interrupt which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  enErrIrqSel      SDIF error interrupt selection
 **
 ** \retval Ok                    Disable error interrupt normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - enErrIrqSel out of range
 ******************************************************************************/      
en_result_t  Sdif_DisableErrIrq( volatile stc_sdifn_t* pstcSdif,
                                 en_sdif_err_irq_sel_t enErrIrqSel)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    switch (enErrIrqSel)
    {
        case SdifCmdTimeoutErrIrq:
            //pstcSdif->SEINTSGE_f.CMDTOERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(0);
            break;
        case SdifCmdCrcErrIrq:
            //pstcSdif->SEINTSGE_f.CMDCRCERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(1);
            break;
        case SdifCmdEndBitErrIrq:
            //pstcSdif->SEINTSGE_f.CMDEBERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(2);
            break;
        case SdifCmdIndexErrIrq:
            //pstcSdif->SEINTSGE_f.CMDIDXERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(3);
            break;
        case SdifDataTimeoutErrIrq:
            //pstcSdif->SEINTSGE_f.DTTOERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(4);
            break;
        case SdifDataCrcErrIrq:
            //pstcSdif->SEINTSGE_f.DTCRCERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(5);
            break;
        case SdifDataEndBitErrIrq:
            //pstcSdif->SEINTSGE_f.DTEBERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(6);
            break;
        case SdifCurrentLimitErrIrq:
            //pstcSdif->SEINTSGE_f.CRTLMTERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(7);
            break;    
        case SdifAutoCmdErrIrq:
            //pstcSdif->SEINTSGE_f.ACMD12ERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(8);
            break;  
        case SdifAdmaErrIrq:
            //pstcSdif->SEINTSGE_f.ADMAERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(9);
            break;  
        case SdifTuningErrIrq:
            //pstcSdif->SEINTSGE_f.TUNINGERRG = 0u;
            pstcSdif->offset_3a &= ~BIT(10);
            break;              
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

#endif

/**
 ******************************************************************************
 ** \brief Get error status of SDIF
 **
 ** This function gets the SDIF error status.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [out] pstcStatus       Pointer to error status selection structure
 **
 ** \retval Ok                    Get error status of SDIF normally
 ** \retval ErrorInvalidParameter If one of following cases matchs:  
 **                               - pstcSdif == NULL
 **                               - pstcStatus == NULL
 ******************************************************************************/   
en_result_t Sdif_GetErrStatus( volatile stc_sdifn_t* pstcSdif, 
                               stc_sdif_err_irq_status_t* pstcStatus)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcStatus->u16ErrStatus = pstcSdif->SEINTST;
    pstcStatus->u16ErrStatus = pstcSdif->offset_32;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Clear error status of SDIF
 **
 ** This function clears the SDIF error status which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  pstcStatus       Pointer to error status selection structure
 **
 ** \retval Ok                    Clear error status of SDIF normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - pstcStatus == NULL
 ******************************************************************************/ 
en_result_t  Sdif_ClrErrStatus( volatile stc_sdifn_t* pstcSdif, 
                                stc_sdif_err_irq_status_t* pstcStatus)
{
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pstcStatus))
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SEINTST = pstcStatus->u16ErrStatus;
    pstcSdif->offset_32 = pstcStatus->u16ErrStatus; 
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get normal status of SDIF
 **
 ** This function gets the SDIF normal status.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [out] pstcStatus       Pointer to normal status selection structure
 **
 ** \retval Ok                    Get normal status of SDIF normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/  
en_result_t Sdif_GetNormalStatus( volatile stc_sdifn_t* pstcSdif, 
                                  stc_sdif_normal_irq_status_t* pstcStatus)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcStatus->u16NormalStatus = pstcSdif->SNINTST;
    pstcStatus->u16NormalStatus = pstcSdif->offset_30;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Clear normal status of SDIF
 **
 ** This function clears the SDIF normal status which is selected.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  pstcStatus       Pointer to normal status selection structure
 **
 ** \retval Ok                    Clear normal status of SDIF normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - pstcStatus == NULL
 ******************************************************************************/ 
en_result_t Sdif_ClrNormalStatus( volatile stc_sdifn_t* pstcSdif, 
                                  stc_sdif_normal_irq_status_t* pstcStatus)
{
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pstcStatus))
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SNINTST = pstcStatus->u16NormalStatus;
    pstcSdif->offset_30 = pstcStatus->u16NormalStatus;

    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get error status of SDIF auto command
 **
 ** This function gets the SDIF error status of auto command
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  pstcStatus       Pointer to normal status selection structure
 **
 ** \retval Ok                    Auto command error status of SDIF gotten normally
 ** \retval ErrorInvalidParameter If one of following cases matchs: 
 **                               - pstcSdif == NULL
 **                               - pstcStatus == NULL
 ******************************************************************************/ 
en_result_t  Sdif_GetAutoCommandErrStatus( volatile stc_sdifn_t* pstcSdif, 
                                           stc_sdif_auto_cmd_err_t* pstcStatus)
{
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pstcStatus))
    {
        return ErrorInvalidParameter ;
    }
    
   // pstcStatus->u16AutoCmdErr = pstcSdif->SACMDEST;
    pstcStatus->u16AutoCmdErr = pstcSdif->offset_3c;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Enable internal clock of SDIF
 **
 ** This function makes SD host controller start working.
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    Internal clock of SDIF enabled normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t  Sdif_EnableInternalClock( volatile stc_sdifn_t* pstcSdif )
{
     // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SCLKCTL_f.INTLCLCKEN = 1u;
      pstcSdif->offset_2c |= BIT(0);
    
    //while(1u != pstcSdif->SCLKCTL_f.INTLCLCKST);
    //while(1u != (pstcSdif->offset_2c) & (BIT(1)));
    do{
        if(pstcSdif->offset_2c & BIT(1))
            break;
    }while(1);
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable internal clock of SDIF
 **
 ** This function makes SD host controller stop working.
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    Internal clock of SDIF enabled normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t  Sdif_DisableInternalClock( volatile stc_sdifn_t* pstcSdif )
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SCLKCTL_f.INTLCLCKEN = 0u;
    pstcSdif->offset_2c &= ~BIT(0);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Enable SDCLK output
 **
 ** SD host drives SDCLK line.
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    SDCLK output of SDIF enabled normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t   Sdif_EnableSdclk( volatile stc_sdifn_t* pstcSdif )
{
     // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SCLKCTL_f.SDCLCKEN = 1u;
    pstcSdif->offset_2c |= BIT(2);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable SDCLK output
 **
 ** SD host don't drive SDCLK line.
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    SDCLK output of SDIF disabled normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t   Sdif_DisableSdclk( volatile stc_sdifn_t* pstcSdif )
{
     // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SCLKCTL_f.SDCLCKEN = 0u;
    pstcSdif->offset_2c &= ~BIT(2);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Set the clock division of SD clock
 **
 ** This function changes the SD clock division.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  u16Div           Division value
 **
 ** \retval Ok                    Clock division is changed normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t Sdif_SetClkDiv( volatile stc_sdifn_t* pstcSdif, uint16_t u16Div)
{
     // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    // Set clock division (10-bit)
    //pstcSdif->SCLKCTL_f.SDCLKSEL = (u16Div & 0xFFu);
    pstcSdif->offset_2c &= ~(0x00FFu); 
    pstcSdif->offset_2c |= ((u16Div & 0xFFu) << 8);
    
    //pstcSdif->SCLKCTL_f.UPSDCLKSEL = ((u16Div >> 8u) &  0x03u);
    pstcSdif->offset_2c &= ~(0x00C0u); 
    pstcSdif->offset_2c |= (((u16Div >> 8u) &  0x03u) << 6);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Set the bus width of SD Bus
 **
 ** This function changes the SD bus width.
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  bEnable4BitMode  Enable 4-bit mode or not
 **
 ** \retval Ok                    Bus width is set normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t Sdif_SetBusWidth( volatile stc_sdifn_t* pstcSdif, 
                              boolean_t bEnable4BitMode )
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    // Set bit width
    //pstcSdif->SHCTL1_f.DATAWIDTH = ((TRUE == bEnable4BitMode) ? 1u : 0u);

    if(bEnable4BitMode) {
        pstcSdif->offset_28 |= BIT(1); 
    } else {
        pstcSdif->offset_28 &= ~BIT(1); 
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Set the bus speed mode of SD Bus
 **
 ** This function changes the SD bus width.
 **
 ** \param [in]  pstcSdif              SDIF instance
 ** \param [in]  bEnableHighSpeedMode  Enable high speed mode or not
 **
 ** \retval Ok                    Bus width is set normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t Sdif_SetBusSpeedMode( volatile stc_sdifn_t* pstcSdif, 
                                  boolean_t bEnableHighSpeedMode )
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    // Set bit width
    //pstcSdif->SHCTL1_f.HIGHSPDEN = ((TRUE == bEnableHighSpeedMode) ? 1u : 0u);

    if(bEnableHighSpeedMode) {
        pstcSdif->offset_28 |= BIT(2); 
    } else {
        pstcSdif->offset_28 &= ~BIT(2); 
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Select bus voltage of SD bus
 **
 ** This function selects the SD bus voltage 
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  enVoltage        Voltage selection
 **
 ** \retval Ok                    Voltage selected normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL or enVoltage out of range
 ******************************************************************************/ 
en_result_t  Sdif_SelBusVoltage( volatile stc_sdifn_t* pstcSdif, 
                                 en_sdif_voltage_sel_t enVoltage)
{
	////arm_print("Sdif_SelBusVoltage()\n");
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    switch (enVoltage)
    {
        case SdifVoltage18v:
            //pstcSdif->SPWRCTL_f.SDBUSVLSEL = 5u;
            pstcSdif->offset_28 &= ~(0xE00);
            pstcSdif->offset_28 |= (5u << 9);
            break;
        case SdifVoltage30v:
            //pstcSdif->SPWRCTL_f.SDBUSVLSEL = 6u;
            pstcSdif->offset_28 &= ~(0xE00);
            pstcSdif->offset_28 |= (6u << 9);
            break;
        case SdifVoltage33v:
            //pstcSdif->SPWRCTL_f.SDBUSVLSEL = 7u;
            pstcSdif->offset_28 &= ~(0xE00);
            pstcSdif->offset_28 |= (7u << 9);
            break;
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Power on SD bus power
 **
 ** This function starts power supply on SD bus
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    Power on normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t  Sdif_BusPowerOn( volatile stc_sdifn_t* pstcSdif)
{
     // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SPWRCTL_f.SDBUSPWR = 1u;
    pstcSdif->offset_28 |= BIT(8);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Power off SD bus power
 **
 ** This function stops power supply on SD bus
 **
 ** \param [in]  pstcSdif         SDIF instance
 **
 ** \retval Ok                    Power off normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL
 ******************************************************************************/ 
en_result_t  Sdif_BusPowerOff( volatile stc_sdifn_t* pstcSdif)
{
    // Check for NULL pointer
    if (NULL == pstcSdif) 
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SPWRCTL_f.SDBUSPWR = 0u;
    pstcSdif->offset_28 &= ~BIT(8);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Send SD command
 **
 ** This function sends command on CMD line
 **
 ** \param [in]  pstcSdif         SDIF instance
 ** \param [in]  pstcConfig       Pointer to command structure
 **
 ** \retval Ok                    Command sent normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL or pstcConfig == NULL
 ******************************************************************************/ 
en_result_t Sdif_SendCommand( volatile stc_sdifn_t* pstcSdif,
                              stc_sdif_cmd_config_t* pstcConfig )
{
    stc_sdif_scmmd_field_t stcCmd;
    
    // Clear structure
    PDL_ZERO_STRUCT(stcCmd);
  
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pstcConfig))
    {
        return ErrorInvalidParameter ;
    }
    
    // Set command type
    switch (pstcConfig->enCmdType)
    {
        case SdifCmdNormal:
            stcCmd.CMDTYPE = 0u;
            break;
        case SdifCmdSuspend:
            stcCmd.CMDTYPE = 1u;
            break;
        case SdifCmdResume:
            stcCmd.CMDTYPE = 2u;
            break;
        case SdifCmdAbort:
            stcCmd.CMDTYPE = 3u;
            break;
        default:
            return ErrorInvalidParameter ;
    }
    
    // Set data present during the command
    stcCmd.DATPRESSEL = ((TRUE == pstcConfig->bDataPresent) ? 1u : 0u);
    
    // Set command index check 
    stcCmd.CMDIDXCHKE = ((TRUE == pstcConfig->bCmdIndexCheck) ? 1u : 0u);
    
    // Set command CRC check
    stcCmd.CMDCRCCHKE = ((TRUE == pstcConfig->bCmdCrcCheck) ? 1u : 0u);
    
    // Set command response type
    switch (pstcConfig->enResponseType)
    {
        case SdifResponseNone:
            stcCmd.RESPTYPE = 0u;
            break;
        case SdifResponse136Bit:
            stcCmd.RESPTYPE = 1u;
            break;
        case SdifResponse48Bit:
            stcCmd.RESPTYPE = 2u;
            break;
        case SdifResponse48BitCheckBusy:
            stcCmd.RESPTYPE = 3u;
            break;
        default:
            return ErrorInvalidParameter ;            
    }
    
    // Set command index
    stcCmd.CMDINDEX = pstcConfig->u8CmdIndex;
    
    // Auto command setting
    switch (pstcConfig->enAutoCmdType)
    {
        case SdifAutoCmdDisable:
            //pstcSdif->STRSFMD_f.AUTOCMDEN = 0u;
            pstcSdif->offset_c &= ~(0xC);
            break;
        case SdifAutoCmd12Enable:
            //pstcSdif->STRSFMD_f.AUTOCMDEN = 1u;
            pstcSdif->offset_c &= ~(0xC);
            pstcSdif->offset_c |= (1u << 2);
            break;
        case SdifAutoCmd23Enable:
            //pstcSdif->STRSFMD_f.AUTOCMDEN = 2u;
            pstcSdif->offset_c &= ~(0xC);
            pstcSdif->offset_c |= (2u << 2);
            break;
        default:
            return ErrorInvalidParameter ;     
    }
    
    // Set augument derectly to hardware register
    //pstcSdif->SSA1 = pstcConfig->u32Argument;
    *(volatile uint32_t*)(&pstcSdif->offset_8) = pstcConfig->u32Argument;

    
    if(pstcConfig->u8CmdIndex == SD_CMD5)
    {
      if((pstcConfig->u32Argument & (1 << 24)))
      {
        //pstcSdif->SHCTL2 = (1 << 3); 
        pstcSdif->offset_3e = (1 << 3);
      }
    }
    // Update command hardware register (Command is sent)
   //according to the spec, this register should be only write once.
   //pstcSdif->SCMMD = *((uint16_t *)&stcCmd);
   pstcSdif->offset_e = *((volatile uint16_t *)&stcCmd);

    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get SD response
 **
 ** This function receives response on CMD line
 **
 ** \param [in]   pstcSdif           SDIF instance
 ** \param [out]  pu16ResponseData   pointer to response data
 ** \param [in]   u8ResponseRegCount Response register count
 **
 ** \retval Ok                    Response received normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL or u8ResponseRegCount > 8u
 ******************************************************************************/ 
en_result_t  Sdif_GetResponse( volatile stc_sdifn_t* pstcSdif,
                               uint16_t* pu16ResponseData, 
                               uint8_t u8ResponseRegCount)
{
    uint16_t* pu16ResponseBaseAddr;
    uint8_t u8i;
  
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pu16ResponseData) || (u8ResponseRegCount > 8u))
    {
        return ErrorInvalidParameter ;
    }
    
    //pu16ResponseBaseAddr = (uint16_t*)&pstcSdif->SRESP0;
    pu16ResponseBaseAddr = (uint16_t*)&pstcSdif->offset_10;
    
    for(u8i=0; u8i<u8ResponseRegCount; u8i++)
    {
        *pu16ResponseData++ = *pu16ResponseBaseAddr++;
    }
    
    return Ok;
}
uint16_t block_cnt;
/**
 ******************************************************************************
 ** \brief Initialize data transfer
 **
 ** This function must be called before data transfer to initialize data 
 ** transfer mode.
 **
 ** \param [in]   pstcSdif           SDIF instance
 ** \param [in]   pstcDataConfig     pointer to data transfer configuration
 **
 ** \retval Ok                    Data is initialized normally
 ** \retval ErrorInvalidParameter pstcSdif == NULL or pstcDataConfig == NULL
 ******************************************************************************/ 
en_result_t  Sdif_InitDataTransfer( volatile stc_sdifn_t* pstcSdif, 
                                    stc_sdif_data_config_t* pstcDataConfig)
{
    // Check for NULL pointer
    if ((NULL == pstcSdif) || (NULL == pstcDataConfig))
    {
        return ErrorInvalidParameter ;
    }
    
    // Set multi-block or single block transfer
    //pstcSdif->STRSFMD_f.BLCKCNTSEL = ((TRUE == pstcDataConfig->bMultipleBlock) ? 1u : 0u);
    if(pstcDataConfig->bMultipleBlock){
        pstcSdif->offset_c |= BIT(5);
    } else {
        pstcSdif->offset_c &= ~BIT(5);
    }
    
    // Set data transfer direction
    //pstcSdif->STRSFMD_f.DTTRSFDIR = ((TRUE == pstcDataConfig->bRead) ? 1u : 0u);
    
    if(pstcDataConfig->bRead) {

        pstcSdif->offset_c |= BIT(4);

    } else {

        pstcSdif->offset_c &= ~BIT(4);
    }
    
    // Set block size
  //  pstcSdif->SBSIZE_f.TRSFBLCKSZ = pstcDataConfig->u16BlockSize;
        pstcSdif->offset_4 &= ~(0xFFF);
        pstcSdif->offset_4 |= (0xFFF & pstcDataConfig->u16BlockSize);

    // Set block count
    //pstcSdif->SBLCNT = pstcDataConfig->u16BlockCount;
    pstcSdif->offset_6 = pstcDataConfig->u16BlockCount;
    
    // Set block count enable
    //pstcSdif->STRSFMD_f.BLCKCNTEN = ((TRUE == pstcDataConfig->bBlockCountEnable) ? 1u : 0u);
    if(pstcDataConfig->bBlockCountEnable) {

        pstcSdif->offset_c |= BIT(1);

    } else {

        pstcSdif->offset_c &= ~BIT(1);

    }

    
    // Enable DMA or not
    //pstcSdif->STRSFMD_f.DMAEN = ((TRUE == pstcDataConfig->bEnableDma) ? 1u : 0u);
    if(pstcDataConfig->bEnableDma) {

        pstcSdif->offset_c |= BIT(0);

    } else {

        pstcSdif->offset_c &= ~BIT(0);

    }

    
    // Set descriptor table of ADMA
    // pstcSdif->SADSA0 = (uint16_t)pstcDataConfig->u32AdmaDespTableAddress;
    pstcSdif->offset_58 = (uint16_t) pstcDataConfig->u32AdmaDespTableAddress; 
    pstcSdif->offset_5a = (uint16_t) (pstcDataConfig->u32AdmaDespTableAddress >> 16u);

    //pstcSdif->SADSA1 = (uint16_t)(pstcDataConfig->u32AdmaDespTableAddress >> 16u);
 //   pstcSdif->offset_5a = (uint16_t) (pstcDataConfig->u32AdmaDespTableAddress >> 16u);
    
    // Set data timeout time
    if(pstcDataConfig->u8DataTimeout > SdifTimeout_BaseClk_2_27)
    {
        return ErrorInvalidParameter ;
    }
    //pstcSdif->STOCTL_f.DTTMOUTVAL = pstcDataConfig->u8DataTimeout;
    pstcSdif->offset_2e &= ~(0x000Fu);
    pstcSdif->offset_2e |= ((0x000Fu) & (pstcDataConfig->u8DataTimeout));
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Read data from SD data buffer port
 **
 ** This function reads 32-bit data from data buffer
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval Data that is read
 ******************************************************************************/ 
uint32_t  Sdif_ReadData( volatile stc_sdifn_t* pstcSdif )
{
    //return pstcSdif->SBUFDP;
    return (pstcSdif->offset_20);
    //return (*(volatile uint32_t *)0x20260020);
}

/**
 ******************************************************************************
 ** \brief Write data to SD data buffer port
 **
 ** This function writes 32-bit data to data buffer
 **
 ** \param [in]   pstcSdif           SDIF instance
 ** \param [in]   u32Data            Data to be written
 **
 ** \retval Ok                    Data is written normally
 ** \retval ErrorInvalidParameter pstcSdif  == NULL
 ******************************************************************************/ 
en_result_t  Sdif_WriteData( volatile stc_sdifn_t* pstcSdif,
                             uint32_t u32Data)
{
    volatile uint32_t *data_ptr = &pstcSdif->offset_20;
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    //pstcSdif->SBUFDP = u32Data;
    //*(volatile uint32_t*)(&pstcSdif->offset_20) = u32Data;
   // pstcSdif->offset_20 = u32Data;
      *data_ptr = u32Data;

    //*(volatile uint32_t *)0x20260020 = u32Data;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Stop data transfer during block gap
 **
 ** This function is used to stop data trasnfer of multi-block transfer
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval Ok                    Data stops tranfer normally
 ** \retval ErrorInvalidParameter pstcSdif  == NULL
 ******************************************************************************/ 
en_result_t  Sdif_StopAtBlockGap( volatile stc_sdifn_t* pstcSdif )
{
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SBLKGPCTL_f.BLCKGSTPREQ = 1u;
    pstcSdif->offset_2a |= BIT(0);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Restart data transfer
 **
 ** This function is used to restart data transfer when transfer is pending
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval Ok                    Data restarts tranfer normally
 ** \retval ErrorInvalidParameter pstcSdif  == NULL
 ******************************************************************************/ 
en_result_t  Sdif_RestartTransfer( volatile stc_sdifn_t* pstcSdif )
{
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SBLKGPCTL_f.CONTREQ = 1u;
    pstcSdif->offset_2a |= BIT(1);
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Poll card insertion status
 **
 ** This function checks card is inserted or not 
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval TRUE                    Card is inserted
 ** \retval FALSE                   Card is removed
 ******************************************************************************/ 

boolean_t Sdif_PollCardInsert( volatile stc_sdifn_t* pstcSdif )
{

    volatile uint32_t card_status = 0;
    do {
        card_status = *(volatile uint32_t*)0x20260024 ; 
        if(card_status & BIT(17)){
            if(card_status & BIT(16)){
                break;
            }
        }
    }while(1);
    return 1u;
#if 0
    // Wait until card is stable
    //while(pstcSdif->SPRSTAT_f.CARDSTB != 1u);
    while(FALSE == (((*TU32(&pstcSdif->offset_24)) & (BIT(17)))));
    //return (boolean_t)pstcSdif->SPRSTAT_f.CARDINS;
    return (boolean_t)((*TU32(&pstcSdif->offset_24)) & BIT(16));
    //  card_status = *(volatile uint32_t*)0x20260024 ; 
    //return (boolean_t)(card_status & 0x10000);
#endif
}

/**
 ******************************************************************************
 ** \brief Issue software reset to SD card
 **
 ** This function issues software reset all command to SD card
 **
 ** \param [in]   pstcSdif           SDIF instance
 ** \param [in]   enReset            Reset type
 **
 ** \retval Ok                       Software reset is done normally
 ** \retval ErrorInvalidParameter    pstcSdif == NULL
 ******************************************************************************/ 
en_result_t Sdif_SoftwareReset( volatile stc_sdifn_t* pstcSdif, 
                                en_sdif_reset_t enReset)
{
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    
    switch (enReset)
    {
        case SdifResetDataLine:
            //pstcSdif->SSRST_f.SWRSTDATLN = 1u;
            pstcSdif->offset_2e |= BIT(10);
            //while(0u != pstcSdif->SSRST_f.SWRSTDATLN); // Wait until reset finish
            while(0u != (pstcSdif->offset_2e & BIT(10))); // Wait until reset finish
            break;
        case SdifResetCmdLine:
            //pstcSdif->SSRST_f.SWRSTCMDLN = 1u;
            pstcSdif->offset_2e |= BIT(9);
            
            //while(0u != pstcSdif->SSRST_f.SWRSTCMDLN); // Wait until reset finish
            while(0u != (pstcSdif->offset_2e & BIT(9))); // Wait until reset finish
            break;
        case SdifResetAll:
            //pstcSdif->SSRST_f.SWRSTALL = 1u;
            pstcSdif->offset_2e |= BIT(8);

            //while(0u != pstcSdif->SSRST_f.SWRSTALL); // Wait until reset finish
            while(0u != (pstcSdif->offset_2e & BIT(8))); // Wait until reset finish
            break;
        default:
            return ErrorInvalidParameter ;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Check the status of CMD line
 **
 ** This function checks whether a commmand wiout data can be sent
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval TRUE                     Command is ready to send
 ** \retval FALSE                    Command is inhitted to send
 ******************************************************************************/ 
boolean_t Sdif_CheckCommandInhit( volatile stc_sdifn_t* pstcSdif )
{
    //  return (boolean_t)pstcSdif->SPRSTAT_f.CMDINH;
    return (boolean_t)(pstcSdif->offset_24 & BIT(0));
}

/**
 ******************************************************************************
 ** \brief Check the status of DAT line
 **
 ** This function checks whether a commmand with data can be sent
 **
 ** \param [in]   pstcSdif           SDIF instance
 **
 ** \retval TRUE                     Command is ready to send
 ** \retval FALSE                    Command is inhitted to send
 ******************************************************************************/ 
boolean_t Sdif_CheckCommandWithDataInhit( volatile stc_sdifn_t* pstcSdif )
{
    //return (boolean_t)pstcSdif->SPRSTAT_f.CMDDATINH;
    return (boolean_t)((pstcSdif->offset_24) & BIT(1));
}

/**
 ******************************************************************************
 ** \brief Set the wakeup factor
 **
 ** This function sets SD host wakeup factors.
 **
 ** \param [in]   pstcSdif            SDIF instance
 ** \param [in]   bCardRemovalWakeup  Whether card removal triggers SD host wakeup
 ** \param [in]   bCardInsertWakeup   Whether card insertion triggers SD host wakeup
 ** \param [in]   bCardIrqWakeup      Whether card interrupt triggers SD host wakeup
 **
 ** \retval Ok                       SD host wakeup factor set normally 
 ** \retval FALSE                    Command is inhitted to send
 ******************************************************************************/
en_result_t  Sdif_SetWakeupFactor( volatile stc_sdifn_t* pstcSdif,
                                   boolean_t bCardRemovalWakeup,
                                   boolean_t bCardInsertWakeup, 
                                   boolean_t bCardIrqWakeup)
{
    // Check for NULL pointer
    if (NULL == pstcSdif)
    {
        return ErrorInvalidParameter ;
    }
    
    //pstcSdif->SWKUPCTL_f.WKUPEVNTEN2 = ((TRUE == bCardRemovalWakeup) ? 1u : 0u);
    if(bCardRemovalWakeup) {
        pstcSdif->offset_2a |= BIT(10);
    } else {
        pstcSdif->offset_2a &= ~BIT(10);
    }


    //pstcSdif->SWKUPCTL_f.WKUPEVNTEN1 = ((TRUE == bCardInsertWakeup) ? 1u : 0u);

    if(bCardInsertWakeup) {
        pstcSdif->offset_2a |= BIT(9);
    } else {
        pstcSdif->offset_2a &= ~BIT(9);
    }
    //pstcSdif->SWKUPCTL_f.WKUPEVNTEN0 = ((TRUE == bCardIrqWakeup) ? 1u : 0u);
    
    if(bCardInsertWakeup) {
        pstcSdif->offset_2a |= BIT(8);
    } else {
        pstcSdif->offset_2a &= ~BIT(8);
    }
    
    return Ok;
}

//@} // SdifGroup

#endif // #if (defined(PDL_PERIPHERAL_SD_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
