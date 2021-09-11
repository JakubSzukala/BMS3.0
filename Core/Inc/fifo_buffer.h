/*
 * fifo_buffer.h
 *
 *  Created on: 11 wrz 2021
 *      Author: bursk
 */

#ifndef INC_FIFO_BUFFER_H_
#define INC_FIFO_BUFFER_H_

#define QUEUE_SIZE 5

#include <stdlib.h>
#include <stdint.h>

typedef struct FRAME_TO_SEND{
	uint32_t tx_header;
	uint8_t tx_data[8];
}frame_to_send;

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    void** data;
} queue_t;

extern queue_t Fifo_Queue;

void Queue_Init();
void* Queue_TxCanMessage();
int Queue_AddTxCanMessage(frame_to_send *handle);

#endif /* INC_FIFO_BUFFER_H_ */
