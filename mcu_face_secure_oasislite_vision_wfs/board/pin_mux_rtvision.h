/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_RTVISION_H_
#define _PIN_MUX_RTVISION_H_

#if RTVISION_BOARD
/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UARTPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSDRAMPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitCSIPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLCDPins(void);

#if RTVISION_BOARD
void BOARD_InitLPSPI3Pins(void);
void BOARD_InitLPSPI4Pins(void);
void BOARD_InitJDispPins(void);
void BOARD_InitA71CHPins(void);
void BOARD_InitWifi(void);
void BOARD_InitBluetooth(void);
void BOARD_InitFlash();
#endif

void BOARD_InitLPUART2Pins(void);
void BOARD_InitLPUART5Pins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
#endif /* _PIN_MUX_RTVISION_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
