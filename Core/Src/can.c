/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
/************************************************************************************************
 GLOBAL VARIABLES
 ************************************************************************************************/
CanDataFrameInit can_frame_template;
CAN_FilterTypeDef can_filter_template;
CAN_RxHeaderTypeDef can_rx_header;
uint8_t can_rx_data[8];
CanDataFrameInit can_rx_frame_template;

uint32_t can_tx_mailbox;

/* Includes */
//#include "current_sensor.h"

/* Variables */
//CanDataFrameInit can_frame_template;
//CAN_FilterTypeDef can_filter_template;
//CAN_RxHeaderTypeDef can_rx_header;
CAN_TxHeaderTypeDef can_tx_header;
//uint8_t can_rx_data[8];
//uint8_t TxData[8];
//uint8_t count = 0;
//uint32_t TxMailbox;
//CanDataFrameInit can_rx_frame_template;
//uint32_t can_tx_mailbox;

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 8;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  //void (*ptr2func)(void) =
  send_functions[0] = &SendCellVoltages1_8;
  send_functions[1] = &SendCellVoltages9_16;
  send_functions[2] = &SendCellVoltages17_24;
  send_functions[3] = &SendCellVoltages25_28;
  send_functions[4] = &SendCellVoltagePackCurrent;

  // __HAL_TIM_DISABLE(htim);
  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/************************************************************************************************
 FUNCTIONS CREATED OUTSIDE CUBEMX
 ************************************************************************************************/

/**
 * @brief: Initialize CAN network
 * @param chosen_network
 *
 **/
void CanInit(CAN_HandleTypeDef chosen_network) {
	if (HAL_CAN_Start(&chosen_network) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_CAN_ActivateNotification(&chosen_network,
	CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief: store received data from chosen network
 * @param chosen_network
 *
 **/
void CanSaveReceivedData(CAN_HandleTypeDef chosen_network, CanDataFrameInit *ptr_can_rx_frame_template) {
	if (HAL_CAN_GetRxMessage(&chosen_network, CAN_RX_FIFO0, &ptr_can_rx_frame_template->rx_header,
			ptr_can_rx_frame_template->rx_data) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
	}
//	CanClearRxDataFrame(ptr_can_rx_frame_template);
}

/**
 * @brief: configure can data frames hardware filter
 * @param chosen_network
 * @param can_filter_bank: Specific filter bank from 0-14
 * @param can_filter_id_high: High byte of CAN ID to be received
 * @param can_filter_id_low: Low byte of CAN ID to be received
 * @param can_filter_mask_id_high: High byte of CAN ID mask - IDs to be received
 * @param can_filter_mask_id_low: Low byte of CAN ID mask - IDs to be received
 *
 **/
void CanConfigFilter(CAN_HandleTypeDef chosen_network, uint8_t can_filter_bank,
		uint32_t can_filter_id_high, uint32_t can_filter_id_low,
		uint32_t can_filter_mask_id_high, uint32_t can_filter_mask_id_low) {
	can_filter_template.FilterBank = can_filter_bank;
	can_filter_template.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter_template.FilterScale = CAN_FILTERSCALE_32BIT;
//	can_filter_template.FilterIdHigh = 0x290 << 5; //can_filter_id_high; //18FF;			//0x321 << 5;
//	can_filter_template.FilterIdLow = 0x00000000; //can_filter_id_low; //50E5;				//0x00000000;
	can_filter_template.FilterIdHigh = 0x0000;
	can_filter_template.FilterIdLow = 0x0000;
	can_filter_template.FilterMaskIdHigh = 0x0000;
	can_filter_template.FilterMaskIdLow = 0x0000;
//	can_filter_template.FilterMaskIdHigh = 0x290 << 5;	//0x111 << 5;
//	can_filter_template.FilterMaskIdLow = 0x00000000;
	can_filter_template.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	can_filter_template.FilterActivation = ENABLE;
	can_filter_template.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&chosen_network, &can_filter_template) != HAL_OK) {
		Error_Handler();
	}

}

/**
 * @brief: synchronize data from nodes with canopen sync frame
 * @param chosen_network
 * @param ptr_can_frame_template: pointer to a structure with basic can frame parameteres
 *
 */
void CanSendSync(CAN_HandleTypeDef chosen_network,
		CanDataFrameInit *ptr_can_frame_template) {
	ptr_can_frame_template->tx_header.StdId = 0x080;
	ptr_can_frame_template->tx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->tx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->tx_header.DLC = 0;
	ptr_can_frame_template->tx_header.TransmitGlobalTime = DISABLE;

	if (HAL_CAN_AddTxMessage(&chosen_network,
			&ptr_can_frame_template->tx_header, ptr_can_frame_template->tx_data,
			&can_tx_mailbox) != HAL_OK) {
		Error_Handler();
	}
	while (HAL_CAN_GetTxMailboxesFreeLevel(&chosen_network) != 3) {
	}
	CanClearTxDataFrame(ptr_can_frame_template);
}

/**
 * @brief: perform network management with canopen nmt frame
 * @param chosen_network
 * @param state: one from global defined states available in ./Core/Src/can.h
 * @param node_id: chosen node to receive the order
 * @param ptr_can_frame_template: pointer to a structure with basic can frame parameteres
 *
 */
void CanSendNmt(CAN_HandleTypeDef chosen_network, uint8_t state,
		uint8_t node_id, CanDataFrameInit *ptr_can_frame_template) {
	ptr_can_frame_template->tx_header.StdId = 0x000;
	ptr_can_frame_template->tx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->tx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->tx_header.DLC = 2;
	ptr_can_frame_template->tx_header.TransmitGlobalTime = DISABLE;

	ptr_can_frame_template->tx_data[0] = state;
	ptr_can_frame_template->tx_data[1] = node_id;

	if (HAL_CAN_AddTxMessage(&chosen_network,
			&ptr_can_frame_template->tx_header, ptr_can_frame_template->tx_data,
			&can_tx_mailbox) != HAL_OK) {
		Error_Handler();
	}
	while (HAL_CAN_GetTxMailboxesFreeLevel(&chosen_network) != 3) {
	}
	CanClearTxDataFrame(ptr_can_frame_template);
}

/**
 * @brief Transfer CAN PDO frame to chosen network
 * @param chosen_network: CAN_HIGH_SPEED or CAN_LOW_SPEED
 * @param frame_pdo_id: choose node's Transmit PDO or Receive PDO ID
 * @param number_of_bytes: between 0-8
 * @param data_to_be_sent: array storing the sent data
 * @param *can_frame_template: pointer to a structure containing basic frame parameters
 *
 **/
void CanSendPdo(CAN_HandleTypeDef chosen_network, uint32_t frame_pdo_id,
		uint8_t number_of_bytes, CanDataFrameInit *ptr_can_frame_template,
		uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3,
		uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7) {
	ptr_can_frame_template->tx_header.StdId = frame_pdo_id;
	ptr_can_frame_template->tx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->tx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->tx_header.DLC = number_of_bytes;
	ptr_can_frame_template->tx_header.TransmitGlobalTime = DISABLE;
	ptr_can_frame_template->tx_data[0] = byte0;
	ptr_can_frame_template->tx_data[1] = byte1;
	ptr_can_frame_template->tx_data[2] = byte2;
	ptr_can_frame_template->tx_data[3] = byte3;
	ptr_can_frame_template->tx_data[4] = byte4;
	ptr_can_frame_template->tx_data[5] = byte5;
	ptr_can_frame_template->tx_data[6] = byte6;
	ptr_can_frame_template->tx_data[7] = byte7;

	if (HAL_CAN_AddTxMessage(&chosen_network,
			&ptr_can_frame_template->tx_header, ptr_can_frame_template->tx_data,
			&can_tx_mailbox) != HAL_OK) {
		Error_Handler();
	}

	while (HAL_CAN_GetTxMailboxesFreeLevel(&chosen_network) != 3) {
	}

	CanClearTxDataFrame(ptr_can_frame_template);

}

/**
 * @brief Transfer CAN SDO frame to chosen network
 * @param chosen_network: CAN_HIGH_SPEED or CAN_LOW_SPEED
 * @param frame_sdo_id: choose node's Transmit SDO or Receive SDO ID
 * @param number_of_bytes: between 0-7
 * @param bytes0-7: data
 * @param *ptr_can_frame_template: pointer to a structure containing basic frame parameters
 *
 **/
void CanSendSdo(CAN_HandleTypeDef chosen_network, uint8_t frame_sdo_id,
		CanDataFrameInit *ptr_can_frame_template, uint8_t number_of_bytes,
		uint8_t command_byte, uint8_t byte0, uint8_t byte1, uint8_t byte2,
		uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6) {
	ptr_can_frame_template->tx_header.StdId = frame_sdo_id;
	ptr_can_frame_template->tx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->tx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->tx_header.DLC = number_of_bytes;
	ptr_can_frame_template->tx_header.TransmitGlobalTime = DISABLE;
	ptr_can_frame_template->tx_data[0] = command_byte;
	ptr_can_frame_template->tx_data[1] = byte0;
	ptr_can_frame_template->tx_data[2] = byte1;
	ptr_can_frame_template->tx_data[3] = byte2;
	ptr_can_frame_template->tx_data[4] = byte3;
	ptr_can_frame_template->tx_data[5] = byte4;
	ptr_can_frame_template->tx_data[6] = byte5;
	ptr_can_frame_template->tx_data[7] = byte6;

	if (HAL_CAN_AddTxMessage(&chosen_network,
			&ptr_can_frame_template->tx_header, ptr_can_frame_template->tx_data,
			&can_tx_mailbox) != HAL_OK) {
		Error_Handler();
	}

	while (HAL_CAN_GetTxMailboxesFreeLevel(&chosen_network) != 3) {
	}

	CanClearTxDataFrame(ptr_can_frame_template);
}

/**
 * @brief: data sent over usb is not correctly shown when structure is not cleared
 *         after every message sent. Assigning zeros has no influence on the network
 * @param ptr_can_frame_template: chosen structure which helds all the data
 *
 **/
void CanClearTxDataFrame(CanDataFrameInit *ptr_can_frame_template) {
	ptr_can_frame_template->tx_header.StdId = 0x00;
	ptr_can_frame_template->tx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->tx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->tx_header.DLC = 0;
	ptr_can_frame_template->tx_header.TransmitGlobalTime = DISABLE;

	ptr_can_frame_template->tx_data[0] = 0x0;
	ptr_can_frame_template->tx_data[1] = 0x0;
	ptr_can_frame_template->tx_data[2] = 0x0;
	ptr_can_frame_template->tx_data[3] = 0x0;
	ptr_can_frame_template->tx_data[4] = 0x0;
	ptr_can_frame_template->tx_data[5] = 0x0;
	ptr_can_frame_template->tx_data[6] = 0x0;
	ptr_can_frame_template->tx_data[7] = 0x0;
}


void CanClearRxDataFrame(CanDataFrameInit *ptr_can_frame_template) {
	ptr_can_frame_template->rx_header.StdId = 0x00;
	ptr_can_frame_template->rx_header.RTR = CAN_RTR_DATA;
	ptr_can_frame_template->rx_header.IDE = CAN_ID_STD;
	ptr_can_frame_template->rx_header.DLC = 0;

	ptr_can_frame_template->rx_data[0] = 0x0;
	ptr_can_frame_template->rx_data[1] = 0x0;
	ptr_can_frame_template->rx_data[2] = 0x0;
	ptr_can_frame_template->rx_data[3] = 0x0;
	ptr_can_frame_template->rx_data[4] = 0x0;
	ptr_can_frame_template->rx_data[5] = 0x0;
	ptr_can_frame_template->rx_data[6] = 0x0;
	ptr_can_frame_template->rx_data[7] = 0x0;
}
/* Interrupt callbacks*/
HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	CanClearRxDataFrame(&can_rx_frame_template);
	CanSaveReceivedData(*hcan, &can_rx_frame_template);
	GetRawData(&current_data, &can_rx_frame_template);
}

void SendCellVoltages1_8()
{

}
void SendCellVoltages9_16()
{

}
void SendCellVoltages17_24()
{

}
void SendCellVoltages25_28()
{

}
void SendCellVoltagePackCurrent()
{

}

/* USER CODE END 1 */

/* Send functions */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
