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

#include "usart.h"
#include "dma.h"
#include "drv_uart.h"

// UART1
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

static uint8_t usart1_rx_buff[USART1_RX_BUFFER_SIZE];
static uint8_t usart1_tx_buff[USART1_TX_BUFFER_SIZE];
static uint8_t usart1_tx_fifo_buff[USART1_TX_FIFO_SIZE];


// UART2 
// todo

// UART3
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
static uint8_t usart3_rx_buff[USART3_RX_BUFFER_SIZE];
static uint8_t usart3_tx_buff[USART3_TX_BUFFER_SIZE];
static uint8_t usart3_tx_fifo_buff[USART3_TX_FIFO_SIZE];


usart_manage_obj_t usart1_manage_obj = {0};
usart_manage_obj_t usart3_manage_obj = {0};

static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type);
static void usart_transmit_hook(usart_manage_obj_t *m_obj);



void usart1_manage_init(void)
{
    usart1_manage_obj.rx_buffer = usart1_rx_buff;
    usart1_manage_obj.rx_buffer_size = USART1_RX_BUFFER_SIZE;
    usart1_manage_obj.dma_h = &hdma_usart1_rx;
    usart1_manage_obj.uart_h = &huart1;
    usart1_manage_obj.tx_fifo_buffer = usart1_tx_fifo_buff;
    usart1_manage_obj.tx_fifo_size = USART1_TX_FIFO_SIZE;
    usart1_manage_obj.tx_buffer_size = USART1_TX_BUFFER_SIZE;
    usart1_manage_obj.tx_buffer = usart1_tx_buff;
    usart1_manage_obj.is_sending = 0;

    fifo_s_init(&(usart1_manage_obj.tx_fifo), usart1_tx_fifo_buff, USART1_TX_FIFO_SIZE);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart1_rx_buff, USART1_RX_BUFFER_SIZE);
}

void usart3_manage_init(void)
{
    usart3_manage_obj.rx_buffer = usart3_rx_buff;
    usart3_manage_obj.rx_buffer_size = USART3_RX_BUFFER_SIZE;
    usart3_manage_obj.dma_h = &hdma_usart3_rx;
    usart3_manage_obj.uart_h = &huart3;
    usart3_manage_obj.tx_fifo_buffer = usart3_tx_fifo_buff;
    usart3_manage_obj.tx_fifo_size = USART3_TX_FIFO_SIZE;
    usart3_manage_obj.tx_buffer_size = USART3_TX_BUFFER_SIZE;
    usart3_manage_obj.tx_buffer = usart3_tx_buff;
    usart3_manage_obj.is_sending = 0;

    fifo_s_init(&(usart3_manage_obj.tx_fifo), usart3_tx_fifo_buff, USART3_TX_FIFO_SIZE);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, usart3_rx_buff, USART3_RX_BUFFER_SIZE);
}

void usart1_transmit(uint8_t *buff, uint16_t len)
{
    usart_transmit(&usart1_manage_obj, buff, len);
}

void usart3_transmit(uint8_t *buff, uint16_t len)
{
    usart_transmit(&usart3_manage_obj, buff, len);
}


void usart1_rx_callback_register(usart_call_back_t fun)
{
    usart_rx_callback_register(&usart1_manage_obj, fun);
}

void usart3_rx_callback_register(usart_call_back_t fun)
{
    usart_rx_callback_register(&usart3_manage_obj, fun);
}

void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back_t fun)
{
    m_obj->call_back_f = fun;
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_UART);
        // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart1_rx_buff, USART1_RX_BUFFER_SIZE);
    }
}


/**
 * @brief  tx complete interupt
 * @param
 * @retval void
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        usart_transmit_hook(&usart1_manage_obj);
    }
    else if (huart->Instance == USART3)
    {
        usart_transmit_hook(&usart3_manage_obj);
    }
    

    return;
}

/**
 * @brief  uart fifo transmit
 * @param
 * @retval error code
 */
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len)
{
    uint16_t to_send_len;
    uint16_t to_tx_fifo_len;

    if (m_obj->is_sending == 0)
    {
        if (len < m_obj->tx_buffer_size)
        {
            to_send_len = len;
            to_tx_fifo_len = 0;
        }
        else if (len < m_obj->tx_buffer_size + m_obj->tx_fifo_size)
        {
            to_send_len = m_obj->tx_buffer_size;
            to_tx_fifo_len = len - m_obj->tx_buffer_size;
        }
        else
        {
            to_send_len = m_obj->tx_buffer_size;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }
    else
    {
        if (len < m_obj->tx_fifo_size)
        {
            to_send_len = 0;
            to_tx_fifo_len = len;
        }
        else
        {
            to_send_len = 0;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }

    if (to_send_len > 0)
    {
        memcpy(m_obj->tx_buffer, buf, to_send_len);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, to_send_len);
    }
    if (to_tx_fifo_len > 0)
    {
        uint8_t len;
        len = fifo_s_puts(&(m_obj->tx_fifo), (char *)(buf) + to_send_len, to_tx_fifo_len);

        if (len != to_tx_fifo_len)
        {
            return -1;
        }
    }

    return 0;
}

static void usart_transmit_hook(usart_manage_obj_t *m_obj)
{
    uint16_t fifo_data_num = 0;
    uint16_t send_num = 0;

    fifo_data_num = m_obj->tx_fifo.used_num;

    if (fifo_data_num != 0)
    {
        if (fifo_data_num < m_obj->tx_buffer_size)
        {
            send_num = fifo_data_num;
        }
        else
        {
            send_num = m_obj->tx_buffer_size;
        }
        fifo_s_gets(&(m_obj->tx_fifo), (char *)(m_obj->tx_buffer), send_num);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, send_num);
    }
    else
    {
        m_obj->is_sending = 0;
    }
    return;
}

/**
 * @brief  rx to fifo
 * @param
 * @retval void
 */
static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type)
{
    uint16_t read_end_ptr = 0;
    uint16_t read_length = 0;
    uint16_t read_success_length = 0;
    uint16_t read_start_ptr = m_obj->read_start_index;
    uint8_t *pdata = m_obj->rx_buffer;

    UNUSED(read_success_length);

    // uint16_t buff_left = m_obj->dma_h->Instance->NDTR;
    uint16_t buff_left = m_obj->dma_h->Instance->CNDTR;

    if (int_type == INTERRUPT_TYPE_UART)
    {
        read_end_ptr = m_obj->rx_buffer_size - buff_left;
    }

    if (int_type == INTERRUPT_TYPE_DMA_HALF)
    {
        read_end_ptr = m_obj->rx_buffer_size / 2;
    }

    if (int_type == INTERRUPT_TYPE_DMA_ALL)
    {
        read_end_ptr = m_obj->rx_buffer_size;
    }

    read_length = read_end_ptr - m_obj->read_start_index;

    if (m_obj->call_back_f != NULL)
    {
        uint8_t *read_ptr = pdata + read_start_ptr;
        read_success_length = m_obj->call_back_f(read_ptr, read_length);
    }

    m_obj->read_start_index = (m_obj->read_start_index + read_length) % (m_obj->rx_buffer_size);

    return;
}
