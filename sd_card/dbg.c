/** @file printf.c
 * 
 *  @brief This file contains functions to print debug info
 *
 * Copyright (c) RSI India Pvt Ltd. 
 * All rights reserved
 *
 * This software embodies materials and concepts which are 
 * confidential to RSI and is made available solely pursuant 
 * to the terms of a written license agreement with RSI.
 */

#include "dbg.h"
#if 1//def DBG_PRINT

//const uint8_t *print_string;

/**
 * @fn      void dbg_printf(const uint8_t *s)
 * @brief   prints debug string
 * @param   s string address
 * @return  void
 */
# if 0
void arm_print( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    vsprintf( print_string, format, args );
    va_end( args );
    dbg_printf(print_string);
}

void dbg_printf(const uint8_t *s)
{
print_string = s;
  while(*s) {
    TA_PRINT_VAL = (uint32_t)*(s++);
  }
}

/**
 * @fn      void dbg_print_val(uint32_t val)
 * @brief   prints debug value in hex
 * @param   val value that is to be printed
 * @return  void
 */

void dbg_print_val(uint32_t num)
{
  TA_HEX_PRINT_VAL = (uint32_t)num;
}

void dbg_print_val2(uint32_t val)
{
  TA_HEX_PRINT_VAL2 = val;
}

void dbg_print_val3(uint32_t val)
{
  TA_HEX_PRINT_VAL3 = val;
}

void dbg_print_val4(uint32_t val)
{
  TA_HEX_PRINT_VAL4 = val;
}

void dbg_print_val5(uint32_t val)
{
  TA_HEX_PRINT_VAL5 = val;
}

#endif
#endif
