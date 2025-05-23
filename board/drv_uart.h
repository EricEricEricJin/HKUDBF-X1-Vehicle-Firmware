/**
 * Modified by Eric Jin in 2025 for DBF X1 Vehicle Firmware
 */

/****************************************************************************
 *  Copyright (C) 2020 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "sys.h"
#include "fifo.h"

#define USART1_RX_BUFFER_SIZE (256)
#define USART1_TX_BUFFER_SIZE (256)
#define USART1_TX_FIFO_SIZE (512)

#define USART2_RX_BUFFER_SIZE (256)

#define USART3_RX_BUFFER_SIZE (512)
#define USART3_TX_BUFFER_SIZE (512)
#define USART3_TX_FIFO_SIZE (2048)

typedef uint32_t (*usart_call_back_t)(uint8_t *buf, uint16_t len);

typedef struct
{
    UART_HandleTypeDef *uart_h;
    DMA_HandleTypeDef *dma_h;
    uint16_t rx_buffer_size;
    uint8_t *rx_buffer;
    uint16_t read_start_index;
    usart_call_back_t call_back_f;

    uint8_t *tx_buffer;
    uint16_t tx_buffer_size;
    fifo_s_t tx_fifo;
    uint8_t *tx_fifo_buffer;
    uint16_t tx_fifo_size;
    uint8_t is_sending;
} usart_manage_obj_t;

typedef enum
{
    INTERRUPT_TYPE_UART = 0,
    INTERRUPT_TYPE_DMA_HALF = 1,
    INTERRUPT_TYPE_DMA_ALL = 2
} interrput_type;

void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back_t fun);
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len);

void usart1_manage_init(void);
void usart2_manage_init(void);
void usart3_manage_init(void);

void usart1_transmit(uint8_t *buff, uint16_t len);
void usart3_transmit(uint8_t *buff, uint16_t len);

void usart1_rx_callback_register(usart_call_back_t fun);
void usart2_rx_callback_register(usart_call_back_t fun);
void usart3_rx_callback_register(usart_call_back_t fun);
// void usart3_idle_callback(void);

#endif // __DRV_UART_H__
