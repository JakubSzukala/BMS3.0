/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan;

/* USER CODE BEGIN Private defines */
/*******************************************************************************
 DEFINES
 *******************************************************************************/

#define OPERATIONAL_STATE		0x01
#define STOPPED_STATE			0x02
#define PRE_OPERATIONAL_STATE	0x80
#define RESET_APPLICATION		0x81
#define RESET_COMMUNICATION		0x82

#define SDO_UPLOAD   0b00100000
#define SDO_DOWNLOAD 0b01000000

/*******************************************************************************
 DECLARATIONS
 *******************************************************************************/
typedef struct {
	uint8_t tx_data[8];
	CAN_TxHeaderTypeDef tx_header;
	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];
} CanDataFrameInit;

// create an instance of that in init function
typedef void (*pt2SendFunctions)(void);
extern pt2SendFunctions send_functions[7]; // + stop timer function

extern CanDataFrameInit can_frame_template;
extern CanDataFrameInit can_rx_frame_template;


/* can peripherial initialization */
void CanInit(CAN_HandleTypeDef hcanx);
void CanConfigFilter(CAN_HandleTypeDef hcanx, uint8_t can_filter_bank,
		uint32_t can_filter_id_high, uint32_t can_filter_id_low,
		uint32_t can_filter_mask_id_high, uint32_t can_filter_mask_id_low);

void CanSaveReceivedData(CAN_HandleTypeDef chosen_network, CanDataFrameInit *ptr_can_rx_frame_template);
void CanTransfer(CAN_HandleTypeDef hcanx, uint32_t sender_id,
		uint32_t receiver_id);

void CanClearTxDataFrame(CanDataFrameInit *can_frame_template);
void CanClearRxDataFrame(CanDataFrameInit *ptr_can_frame_template);


/* canopen services */
void CanSendSync(CAN_HandleTypeDef hcanx, CanDataFrameInit *can_frame_template);
void CanSendNmt(CAN_HandleTypeDef hcanx, uint8_t state, uint8_t node_id,
		CanDataFrameInit *can_frame_template);
void CanSendPdo(CAN_HandleTypeDef chosen_network, uint32_t frame_pdo_id,
		uint8_t number_of_bytes, CanDataFrameInit *can_frame_template,
		uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3,
		uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7);
void CanSendSdo(CAN_HandleTypeDef chosen_network, uint8_t frame_sdo_id,
		CanDataFrameInit *ptr_can_frame_template, uint8_t number_of_bytes,
		uint8_t command_byte, uint8_t byte0, uint8_t byte1, uint8_t byte2,
		uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6);

/* Basic frames send functions */
void SendCellVoltages1_8();
void SendCellVoltages9_16();
void SendCellVoltages17_24();
void SendCellVoltages25_28();
void SendCellVoltagePackCurrent();

/* USER CODE END Private defines */

void MX_CAN_Init(void);

/* USER CODE BEGIN Prototypes */
//void CanInit(CAN_HandleTypeDef chosen_network);
//CanDataFrameInit CanSaveReceivedData(CAN_HandleTypeDef chosen_network, CanDataFrameInit *ptr_can_rx_frame_template);
//void CanConfigFilter(CAN_HandleTypeDef chosen_network, uint8_t can_filter_bank,
//		uint32_t can_filter_id_high, uint32_t can_filter_id_low,
//		uint32_t can_filter_mask_id_high, uint32_t can_filter_mask_id_low);
//void CanSendPdo(CAN_HandleTypeDef chosen_network, uint32_t frame_pdo_id,
//		uint8_t number_of_bytes, CanDataFrameInit *ptr_can_frame_template,
//		uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3,
//		uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7);
//void CanClearTxDataFrame(CanDataFrameInit *ptr_can_frame_template);
//void CanClearRxDataFrame(CanDataFrameInit *ptr_can_frame_template);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
