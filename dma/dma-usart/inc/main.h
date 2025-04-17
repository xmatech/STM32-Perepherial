/**
 * @file        : main.h
 * @brief       : Заголовочный файл с объявлением функций
 * @author      : xmatech
 * @date        : 19.03.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : 
 */

#include <stm32f4xx.h>



/* Прототипы функций */ 
void rcc_init(void);                  // Настройка тактирования
void usart1_init(void);               // Настройка USART1
void DMA2_Stream0_MEM2MEM_Init(void); // Инициализация DMA2 Stream 0
void DMA2_Stream7_USART1_Init(void);  // Инициализация DMA2 Stream 7
void delay_ms(uint32_t ms);           // Функция задержки в миллисекундах