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
/** \file sdif.h
 **
 ** Headerfile for SDIF functions
 **
 ** History:
 **   - 2015-02-27  1.0  EZh First version for FM Universal PDL.
 **
 ******************************************************************************/

#ifndef __SDIF_H__
#define __SDIF_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "mcu.h"
#include "pdl_user.h"

#if (defined(PDL_PERIPHERAL_SD_ACTIVE))

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup SdifGroup SD Card Interface (SDIF)
 **
 ** Provided functions of SDIF module:
 ** - Sdif_Init()
 ** - Sdif_DeInit()  
 ** - Sdif_EnableInternalClock()
 ** - Sdif_DisableInternalClock()   
 ** - Sdif_EnableSdclk()
 ** - Sdif_DisableSdclk()
 ** - Sdif_SetClkDiv()
 ** - Sdif_SetBusWidth() 
 ** - Sdif_SetBusSpeedMode()   
 ** - Sdif_SelBusVoltage()
 ** - Sdif_BusPowerOn()
 ** - Sdif_BusPowerOff()
 ** - Sdif_SendCommand()
 ** - Sdif_GetResponse()
 ** - Sdif_InitDataTransfer()
 ** - Sdif_ReadData()
 ** - Sdif_WriteData() 
 ** - Sdif_StopAtBlockGap()
 ** - Sdif_RestartTransfer()  
 ** - Sdif_GetErrStatus()  
 ** - Sdif_ClrErrStatus()
 ** - Sdif_GetNormalStatus()  
 ** - Sdif_ClrNormalStatus()   
 ** - Sdif_GetAutoCommandErrStatus()  
 ** - Sdif_CheckCommandInhit()  
 ** - Sdif_CheckCommandWithDataInhit()  
 ** - Sdif_PollCardInsert()
 ** - Sdif_SoftwareReset()
 **  
 ** Sdif_Init() initializes a SD host interface instance. 
 ** Sdif_DeInit() de-initializes a SD host interface instance.
 **
 ** Sdif_EnableInternalClock() Enables SD host internal clock, which makes 
 ** SD host start work. Sdif_DisableInternalClock() disables SD host internal 
 ** clock, which makes SD host stops work.
 **
 ** Sdif_EnableSdclk() enables SDCLK output and Sdif_DisableSdclk() disables
 ** SDCLK output.
 **
 ** The SD clock is first intialized in Sdif_Init() and can be changed by 
 ** Sdif_SetClkDiv()
 **
 ** The bit width is first initialized in Sdif_Init() and can be changed by 
 ** Sdif_SetBusWidth()
 ** 
 ** The bus speed mode is first initialized in Sdif_Init() and can be changed
 ** by Sdif_SetBusSpeedMode()  
 ** 
 ** Sdif_SelBusVoltage() selects SD bus voltage, before Sdif_BusPowerOff() 
 ** should be called to power off SD bus. 
 **
 ** Sdif_BusPowerOn() power on SD bus and Sdif_BusPowerOff() power off SD bus.
 **
 ** Sdif_SendCommand() send the SD command specified by the structure type 
 ** #stc_sdif_cmd_config_t.
 ** 
 ** Sdif_GetResponse() gets the response data, the count of which is specified 
 ** by the parameter Sdif_GetResponse#u8ResponseRegCount
 **
 ** Sdif_InitDataTransfer() initializes data transfer, this function must be 
 ** called before data transfer.
 **
 ** Sdif_ReadData() reads the 32-bit data from data buffer port and 
 ** Sdif_WriteData() writes 32-bit data to data buffer port.
 ** 
 ** Sdif_StopAtBlockGap() stops transfer during block gap in multi-block 
 ** transfer() and Sdif_RestartTransfer() restarts data transfer
 ** 
 ** Sdif_GetErrStatus() gets the error interrupt status and Sdif_ClrErrStatus()
 ** clears error interrupt status.
 **
 ** Sdif_GetNormalStatus() gets the error interrupt status and 
 ** Sdif_ClrNormalStatus() clears error interrupt status. 
 **
 ** Sdif_GetAutoCommandErrStatus() gets the error status of Auto Command.
 **
 ** Sdif_CheckCommandInhit() check whether command without data can be sent. 
 ** Sdif_CheckCommandWithDataInhit() check whether command with data can be sent.   
 **
 ** Sdif_PollCardInsert() polls the card insertion status.
 **
 ** Sdif_SoftwareReset() issues software reset to SD card.
 **
 ******************************************************************************/
//@{

/******************************************************************************
 * Global definitions
 ******************************************************************************/
#include "pdl_user.h"

#define stc_sdifn_t   FM_SDIF_TypeDef
#define PDL_INTERRUPT_ENABLE_SD0    PDL_ON
#define SDIF0       (*((volatile stc_sdifn_t *) FM_SDIF_BASE))

#define SDIF_INSTANCE_COUNT                                \
        (uint8_t)(PDL_PERIPHERAL_ENABLE_SD0 == PDL_ON) 
          
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief SD command types 
 ******************************************************************************/                    
typedef enum en_sdif_cmd_type
{
    SdifCmdAbort      = 0u,  ///< CMD12, CMD52 for writing "I/O Abort" in CCCR
    SdifCmdResume     = 1u,  ///< CMD52 for writing "Function Select" in CCCR
    SdifCmdSuspend    = 2u,  ///< CMD52 for writing "Bus Suspend" in CCCR
    SdifCmdNormal     = 3u,  ///< Other commands
    
}en_sdif_cmd_type_t;

/**
 ******************************************************************************
 ** \brief SDIF response types 
 ******************************************************************************/  
typedef enum en_sdif_response_type
{
    SdifResponseNone           = 0u, ///< No Response
    SdifResponse136Bit         = 1u, ///< Response Length 136
    SdifResponse48Bit          = 2u, ///< Response Length 48
    SdifResponse48BitCheckBusy = 3u, ///< Response Length 48 check Busy after response
    
}en_sdif_response_type_t;

/**
 ******************************************************************************
 ** \brief SD data timeout time 
 ******************************************************************************/  
typedef enum en_sd_data_timeout
{
    SdifTimeout_BaseClk_2_13 = 0u,  ///< Timeout time: Base clock*2^13
    SdifTimeout_BaseClk_2_14 = 1u,  ///< Timeout time: Base clock*2^14
    SdifTimeout_BaseClk_2_15 = 2u,  ///< Timeout time: Base clock*2^15
    SdifTimeout_BaseClk_2_16 = 3u,  ///< Timeout time: Base clock*2^16
    SdifTimeout_BaseClk_2_17 = 4u,  ///< Timeout time: Base clock*2^17
    SdifTimeout_BaseClk_2_18 = 5u,  ///< Timeout time: Base clock*2^18
    SdifTimeout_BaseClk_2_19 = 6u,  ///< Timeout time: Base clock*2^19
    SdifTimeout_BaseClk_2_20 = 7u,  ///< Timeout time: Base clock*2^20
    SdifTimeout_BaseClk_2_21 = 8u,  ///< Timeout time: Base clock*2^21
    SdifTimeout_BaseClk_2_22 = 9u,  ///< Timeout time: Base clock*2^22
    SdifTimeout_BaseClk_2_23 = 10u, ///< Timeout time: Base clock*2^23
    SdifTimeout_BaseClk_2_24 = 11u, ///< Timeout time: Base clock*2^24
    SdifTimeout_BaseClk_2_25 = 12u, ///< Timeout time: Base clock*2^25
    SdifTimeout_BaseClk_2_26 = 13u, ///< Timeout time: Base clock*2^26
    SdifTimeout_BaseClk_2_27 = 14u, ///< Timeout time: Base clock*2^27
    
}en_sdif_data_timeout_t;

/**
 ******************************************************************************
 ** \brief SDIF auto command enable selection
 ******************************************************************************/  
typedef enum en_sdif_auto_cmd
{
    SdifAutoCmdDisable  = 0u,  ///< Auto command disable
    SdifAutoCmd12Enable = 1u,  ///< Auto command 12 enable
    SdifAutoCmd23Enable = 2u,  ///< Auto command 23 enable
    
}en_sdif_auto_cmd_t;

/**
 ******************************************************************************
 ** \brief SDIF error status types
 ******************************************************************************/ 
typedef struct stc_sdif_err_irq
{
    union 
    {
        uint16_t u16ErrStatus;     ///< 16-bit error status
        struct 
        {
          uint16_t CmdTimeoutErr    : 1;  ///< Command timeout error
          uint16_t CmdCrcErr        : 1;  ///< Command CRC error
          uint16_t CmdEndBitErr     : 1;  ///< Command end bit error
          uint16_t CmdIndexErr      : 1;  ///< Command index error
          uint16_t DataTimeoutErr   : 1;  ///< Data timeout error
          uint16_t DataCrcErr       : 1;  ///< Data CRC error
          uint16_t DataEndBitErr    : 1;  ///< Data end bit error
          uint16_t CurrentLimitErr  : 1;  ///< Current limit error
          uint16_t AutoCmdErr       : 1;  ///< Auto command error
          uint16_t AdmaErr          : 1;  ///< ADMA error
          uint16_t TuningErr        : 1;  ///< Tuning error
          uint16_t Reserved         : 5;  ///< Reseverd
        }stcErrIrqStatus;
    };
    
}stc_sdif_err_irq_status_t, stc_sdif_err_irq_en_t;

/**
 ******************************************************************************
 ** \brief SDIF error interrupt types
 ******************************************************************************/
typedef enum en_sdif_err_irq_sel
{
    SdifCmdTimeoutErrIrq   = 0u,   ///< Command timeout error
    SdifCmdCrcErrIrq       = 1u,   ///< Command CRC error
    SdifCmdEndBitErrIrq    = 2u,   ///< Command end bit error
    SdifCmdIndexErrIrq     = 3u,   ///< Command index error
    SdifDataTimeoutErrIrq  = 4u,   ///< Data timeout error
    SdifDataCrcErrIrq      = 5u,   ///< Data CRC error
    SdifDataEndBitErrIrq   = 6u,   ///< Data end bit error
    SdifCurrentLimitErrIrq = 7u,   ///< Current limit error
    SdifAutoCmdErrIrq      = 8u,   ///< Auto command error
    SdifAdmaErrIrq         = 9u,   ///< ADMA error
    SdifTuningErrIrq       = 10u,  ///< Tuning error

}en_sdif_err_irq_sel_t;

/**
 ******************************************************************************
 ** \brief SDIF error status callback functions
 ******************************************************************************/ 
typedef struct stc_sdif_err_irq_cb
{
    func_ptr_t pfnCmdTimeoutErrIrqCb;    ///< Pointer to command timeout error interrupt callback function
    func_ptr_t pfnCmdCrcErrIrqCb;        ///< Pointer to command CRC error interrupt callback function
    func_ptr_t pfnCmdEndBitErrIrqCb;     ///< Pointer to command end bit error interrupt callback function
    func_ptr_t pfnCmdIndexErrIrqCb;      ///< Pointer to command index error interrupt callback function
    func_ptr_t pfnDataTimeoutErrIrqCb;   ///< Pointer to data timeout error interrupt callback function
    func_ptr_t pfnDataEndBitErrIrqCb;    ///< Pointer to data end bit error interrupt callback function
    func_ptr_t pfnDataCrcErrIrqCb;       ///< Pointer to data CRC error interrupt callback function
    func_ptr_t pfnCurrentLimitErrIrqCb;  ///< Pointer to current limit error interrupt callback function
    func_ptr_t pfnAutoCmdErrIrqCb;       ///< Pointer to command error interrupt callback function
    func_ptr_t pfnAdmaErrIrqCb;          ///< Pointer to ADMA error interrupt callback function
    func_ptr_t pfnTuningErrIrqCb;        ///< Pointer to tuning error interrupt callback function
 
}stc_sdif_err_irq_cb_t;

/**
 ******************************************************************************
 ** \brief SDIF normal status types
 ******************************************************************************/ 
typedef struct stc_sdif_normal_irq
{
    union 
    {
        uint16_t u16NormalStatus;              ///< 16-bit normal status
        struct 
        {
          uint16_t CommandComplete  : 1;   ///< Command complete
          uint16_t TransferComplete : 1;   ///< Transfer complete
          uint16_t BlockGapEvent    : 1;   ///< Block gap event
          uint16_t DmaIrq           : 1;   ///< DMA interrupt
          uint16_t BufferWriteReady : 1;   ///< Buffer write ready
          uint16_t BufferReadReady  : 1;   ///< Buffer read ready
          uint16_t CardInsertion    : 1;   ///< Card insertion
          uint16_t CardRemoval      : 1;   ///< Card removal
          uint16_t CardIrq          : 1;   ///< Card interrupt
          uint16_t Reserved0        : 7;   ///< Reserved bits 
        }stcNormalIrqStatus;
    };
    
}stc_sdif_normal_irq_status_t, stc_sdif_normal_irq_en_t;

/**
 ******************************************************************************
 ** \brief SDIF normal interrupt types
 ******************************************************************************/
typedef enum en_sdif_normal_irq_sel
{
    SdifCommandCompleteIrq      = 0u,   ///< Command complete
    SdifTransferCompleteIrq     = 1u,   ///< Transfer complete
    SdifBlockGapEventIrq        = 2u,   ///< Block gap event
    SdifDmaIrq                  = 3u,   ///< DMA interrupt
    SdifBufferWriteReadyIrq     = 4u,   ///< Buffer write ready
    SdifBufferReadReadyIrq      = 5u,   ///< Buffer read ready
    SdifCardInsertionIrq        = 6u,   ///< Card insertion
    SdifCardRemovalIrq          = 7u,   ///< Card removal
    SdifCardIrq                 = 8u,   ///< Card interrupt

}en_sdif_normal_irq_sel_t;

/**
 ******************************************************************************
 ** \brief SDIF error status callback functions
 ******************************************************************************/ 
typedef struct stc_sdif_normal_irq_cb
{
    func_ptr_t pfnCommandCompleteIrqCb;     ///< Pointer to command complete callback function
    func_ptr_t pfnTransferCompleteIrqCb;    ///< Pointer to transfer complete callback function
    func_ptr_t pfnBlockGapIrqCb;            ///< Pointer to Block gap callback function
    func_ptr_t pfnDmaIrqCb;                 ///< Pointer to DMA interrupt callback function
    func_ptr_t pfnBufferWriteReadyIrqCb;    ///< Pointer to buffer write ready callback function
    func_ptr_t pfnBufferReadReadyIrqCb;     ///< Pointer to buffer read ready callback function
    func_ptr_t pfnCardInsertIrqCb;          ///< Pointer to card insertion callback function
    func_ptr_t pfnCardRemovalIrqCb;         ///< Pointer to card removal callback function
    func_ptr_t pfnCardIrqCb;                ///< Pointer to card interrupt callback function

}stc_sdif_normal_irq_cb_t;

/**
 ******************************************************************************
 ** \brief SDIF Auto Command error status
 ******************************************************************************/ 
typedef struct stc_sdif_auto_cmd_err
{
    union 
    {
        uint16_t u16AutoCmdErr;
        struct stcAutoCmdErr
        {
            uint16_t SdifAutoCmd12NotExecuted : 1;      ///< Host Controller cannot issue Auto CMD12 to stop memory multiple block data transfer due to some error
            uint16_t SdifAutoCmdTimeoutErr    : 1;      ///< No response is returned within 64 SDCLK cycles from the end bit of command.
            uint16_t SdifAutoCmdCrcErr        : 1;      ///< This bit is set when detecting a CRC error in the command response.
            uint16_t SdifAutoCmdEndBitErr     : 1;      ///< Detecting that the end bit of command response is 0.
            uint16_t SdifAutoCmdIndexErr      : 1;      ///< Command Index error occurs in response to a command.
            uint16_t Reserved0                : 2;      ///< Reserved
            uint16_t SdifCmdNotIssuedByAutoCmd12 : 1;   ///< CMD_wo_DAT is not executed due to an Auto CMD12 Error (D04-D01) in this register.
            uint16_t Reserved1                : 8;      ///< Reserved
        };
    };
    
}stc_sdif_auto_cmd_err_t;

/**
 ******************************************************************************
 ** \brief SDIF configuration.
 ******************************************************************************/
typedef struct stc_sdif_config
{
    boolean_t            bSelAdma;               ///< TRUE: Select ADMA, FALSE: don't select ADMA
    boolean_t            bEnableHighSpeed;       ///< TRUE: Enable high speed mode, FALSE: Disable high speed mode
    boolean_t            bEnable4BitMode;        ///< TRUE: Use 4-bit mode, FALSE: Use 1-bit mode
#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)    
    stc_sdif_normal_irq_en_t*     pstcNormalIrqEn;     ///< Pointer to normal interrupt enable structure 
    stc_sdif_normal_irq_cb_t*     pstcNormalIrqCb;     ///< Pointer to normal interrupt callback function structure 
    stc_sdif_err_irq_en_t*        pstcErrIrqEn;        ///< Pointer to error interrupt enable structure 
    stc_sdif_err_irq_cb_t*        pstcErrIrqCb;        ///< Pointer to error interrupt callback structure  
    boolean_t                     bTouchNvic;          ///< TRUE: Enable NVIC, FALSE: Disable NVIC
#endif    
}stc_sdif_config_t;

/**
 ******************************************************************************
 ** \brief SDIF command configuration.
 ******************************************************************************/
typedef struct stc_sdif_cmd_config
{
    uint8_t               u8CmdIndex;           ///< Command index
    uint32_t              u32Argument;          ///< The argment of command          
    en_sdif_cmd_type_t    enCmdType;            ///< Command type
    boolean_t             bDataPresent;         ///< TRUE: Data is present and shall be transferred using the DAT line, FALSE:  Commands using only CMD line
    boolean_t             bCmdIndexCheck;       ///< TRUE: Host Controller shall check the Index field in the response to see if it has the same value as the command index. If it is not, it is reported as a Command Index Error, FALSE: the Index field is not checked.
    boolean_t             bCmdCrcCheck;         ///< TRUE: Host Controller shall check the CRC field in the response. If an error is detected, it is reported as a Command CRC Error. FALSE: the CRC field is not checked. The position of CRC field is determined according to the length of the response.  
    en_sdif_response_type_t enResponseType;     ///< Response type, see #en_adc_timer_select_t for details
    en_sdif_auto_cmd_t      enAutoCmdType;      ///< Auto command enable selection, see #en_sdif_auto_cmd_t for details
    
}stc_sdif_cmd_config_t;

/**
 ******************************************************************************
 ** \brief SDIF data transfer configuration.
 ******************************************************************************/
typedef struct stc_sdif_data_config
{
    boolean_t  bMultipleBlock;           ///< TRUE: use multiple block transfer, FALSE: use single block transfer
    boolean_t  bRead;                    ///< TRUE: Read, FALSE: write
    uint16_t   u16BlockSize;             ///< Block size
    uint16_t   u16BlockCount;            ///< Block count
    boolean_t  bBlockCountEnable;        ///< Block count enable setting
    boolean_t  bEnableDma;               ///< TRUE: use ADMA, FALSE: don't use ADMA
    uint32_t   u32AdmaDespTableAddress;  ///< The address of ADMA descriptor table
    uint8_t    u8DataTimeout;            ///< see #en_sdif_data_timeout_t for details 
    
}stc_sdif_data_config_t;

/**
 ******************************************************************************
 ** \brief SDIF voltage selection
 ******************************************************************************/
typedef enum en_sdif_voltage_sel
{
    SdifVoltage18v = 0u,    ///< SD bus voltge: 1.8V
    SdifVoltage30v = 1u,    ///< SD bus voltge: 3.0V
    SdifVoltage33v = 2u,    ///< SD bus voltge: 3.3V
    
}en_sdif_voltage_sel_t;

/**
 ******************************************************************************
 ** \brief SDIF software reset types 
 ******************************************************************************/
typedef enum en_sdif_reset
{
    SdifResetDataLine = 0u, ///<  Only reset data circuit
    SdifResetCmdLine  = 1u, ///<  Only reset command circuit
    SdifResetAll      = 2u, ///<  Reset all SDIF host controller
    
}en_sdif_reset_t;

/// Enumeration to define an index for each enabled SDIF instance
typedef enum en_sdif_instance_index
{
  #if (PDL_PERIPHERAL_ENABLE_SD0 == PDL_ON)
    SdifInstanceIndexSdif0,
  #endif
    SdifInstanceIndexMax
} en_sdif_instance_index_t;

typedef struct stc_sdif_intern_data
{
    stc_sdif_normal_irq_cb_t stcNormalIrqCb;
    stc_sdif_err_irq_cb_t    stcErrIrqCb;
    
}stc_sdif_intern_data_t;

/// SDIF instance data type
typedef struct stc_sdif_instance_data
{
  volatile stc_sdifn_t*  pstcInstance;  ///< pointer to registers of an instance
  stc_sdif_intern_data_t stcInternData; ///< module internal data of instance
} stc_sdif_instance_data_t;

/******************************************************************************/
/* Global variable declarations ('extern', definition in C source)            */
/******************************************************************************/
/// Look-up table for all enabled SD instances and their internal data
extern stc_sdif_instance_data_t m_astcSdifInstanceDataLut[SDIF_INSTANCE_COUNT];

/******************************************************************************/
/* Global function prototypes ('extern', definition in C source)              */
/******************************************************************************/
#if (PDL_INTERRUPT_ENABLE_SD0 == PDL_ON)
/* Int Enable/Disable */
en_result_t               Sdif_EnableErrIrq( volatile stc_sdifn_t* pstcSdif,
                                             en_sdif_err_irq_sel_t enIrqSel);
en_result_t               Sdif_DisableErrIrq( volatile stc_sdifn_t* pstcSdif,
                                              en_sdif_err_irq_sel_t enIrqSel);
en_result_t               Sdif_EnableNormalIrq( volatile stc_sdifn_t* pstcSdif,
                                                en_sdif_normal_irq_sel_t enIrqSel);
en_result_t               Sdif_DisableNomralIrq( volatile stc_sdifn_t* pstcSdif,
                                                 en_sdif_normal_irq_sel_t enIrqSel);
/* SD IRQ */
void                      SdifIrqHandler( volatile stc_sdifn_t*   pstcSdif,
                                          stc_sdif_intern_data_t* pstcSdifInternData );
#endif

en_result_t               Sdif_Init( volatile stc_sdifn_t* pstcSdif,
                                     stc_sdif_config_t*    pstcConfig );
en_result_t               Sdif_DeInit( volatile stc_sdifn_t* pstcSdif, 
                                       boolean_t bTouchIrq,
                                       boolean_t bTouchIrqCb,
                                       boolean_t bTouchNvic);
en_result_t               Sdif_EnableInternalClock( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_DisableInternalClock( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_EnableSdclk( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_DisableSdclk( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_SetClkDiv( volatile stc_sdifn_t* pstcSdif, uint16_t u16Div);
en_result_t               Sdif_SetBusWidth( volatile stc_sdifn_t* pstcSdif, 
                                            boolean_t bEnable4BitMode);
en_result_t               Sdif_SetBusSpeedMode( volatile stc_sdifn_t* pstcSdif, 
                                               boolean_t bEnableHighSpeedMode);
en_result_t               Sdif_SelBusVoltage( volatile stc_sdifn_t* pstcSdif, 
                                              en_sdif_voltage_sel_t enVoltage);
en_result_t               Sdif_BusPowerOn( volatile stc_sdifn_t* pstcSdif);
en_result_t               Sdif_BusPowerOff( volatile stc_sdifn_t* pstcSdif);

en_result_t               Sdif_SendCommand( volatile stc_sdifn_t* pstcSdif,
                                            stc_sdif_cmd_config_t* pstcConfig );

en_result_t               Sdif_GetResponse( volatile stc_sdifn_t* pstcSdif,
                                            uint16_t* pu16ResponseData, 
                                            uint8_t u8ResponseRegCount);

en_result_t               Sdif_InitDataTransfer( volatile stc_sdifn_t* pstcSdif, 
                                                 stc_sdif_data_config_t* pstcDataConfig);

uint32_t                  Sdif_ReadData( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_WriteData( volatile stc_sdifn_t* pstcSdif,
                                          uint32_t u32Data);
en_result_t               Sdif_StopAtBlockGap( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_RestartTransfer( volatile stc_sdifn_t* pstcSdif );

en_result_t               Sdif_GetErrStatus( volatile stc_sdifn_t* pstcSdif, 
                                             stc_sdif_err_irq_status_t* pstcStatus);
en_result_t               Sdif_ClrErrStatus( volatile stc_sdifn_t* pstcSdif, 
                                             stc_sdif_err_irq_status_t* pstcStatus);
en_result_t               Sdif_GetNormalStatus( volatile stc_sdifn_t* pstcSdif, 
                                                stc_sdif_normal_irq_status_t* pstcStatus);
en_result_t               Sdif_ClrNormalStatus( volatile stc_sdifn_t* pstcSdif, 
                                                stc_sdif_normal_irq_status_t* pstcStatus);
en_result_t               Sdif_GetAutoCommandErrStatus( volatile stc_sdifn_t* pstcSdif, 
                                                        stc_sdif_auto_cmd_err_t* pstcStatus);
boolean_t                 Sdif_CheckCommandInhit( volatile stc_sdifn_t* pstcSdif );
boolean_t                 Sdif_CheckCommandWithDataInhit( volatile stc_sdifn_t* pstcSdif );
boolean_t                 Sdif_PollCardInsert( volatile stc_sdifn_t* pstcSdif );
en_result_t               Sdif_SoftwareReset( volatile stc_sdifn_t* pstcSdif, 
                                              en_sdif_reset_t enReset);
en_result_t               Sdif_SetWakeupFactor( volatile stc_sdifn_t* pstcSdif,
                                                boolean_t bCardRemovalWakeup,
                                                boolean_t bCardInsertWakeup, 
                                                boolean_t bCardIrqWakeup);
#ifdef __cplusplus
}
#endif

//@}

#endif // #if (defined(PDL_PERIPHERAL_SDIF_ACTIVE))

#endif /* __SDIF_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
