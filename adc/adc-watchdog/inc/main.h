/**
 * @file        : main.h
 * @brief       : Заголовочный файл для программы работы с ADC1 в режиме Analog Watchdog.
 * @author      : xmatech
 * @date        : 23.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : Заголовочный файл содержит прототипы функций для инициализации тактирования (RCC),
 *                настройки таймера TIM2, инициализации ADC1 и обработчика прерывания ADC.
 *                Программа использует ADC1 для отслеживания напряжения на входе A5 (PA5) с помощью
 *                режима Analog Watchdog. При выходе напряжения за пределы заданных порогов
 *                включается светодиод LED1 (PE13).
 */

#include <stm32f4xx.h>

// Прототипы функций
void rcc_init(void);
void tim2_init(void);
void adc1_init(void);
void ADC1_IRQHandler(void);
