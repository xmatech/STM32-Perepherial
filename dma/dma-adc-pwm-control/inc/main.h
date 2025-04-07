/**
 * @file        : main.h
 * @brief       : Заголовочный файл для управления яркостью светодиода с помощью потенциометра через АЦП, DMA и ШИМ.
 * @author      : xmatech
 * @date        : 14.03.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : Заголовочный файл содержит прототипы функций для настройки тактирования, инициализации таймера TIM1,
 *                инициализации ADC1, настройки DMA2 Stream 0 и обработчика прерывания DMA2 Stream 0.
 */

#include <stm32f4xx.h>


/* Прототипы функций */ 
void rcc_init(void);   // Настройка тактирования
void tim1_init(void);  // Инициализация TIM1
void adc1_init(void); // Инициализация ADC1
void DMA2_Stream0_Init(void); // Инициализация DMA2 Stream 0
void DMA2_Stream0_IRQHandler(void);



