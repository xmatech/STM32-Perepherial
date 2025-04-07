/**
* @file        : gpio.c
* @brief       : Настройка GPIO для работы с кнопками.
* @author      : xmatech*
* @date        : 16.01.2025
* @board       : JZ-F407VET6
* @MCU         : STM32F407VET6#include "main.h"
* @IDE         : Segger Embedded Studio     

* @Description : Настройка PE10 как входа с подтяжкой к питанию.  Включение тактирования порта GPIOE.                                                       
*/


#include "main.h"


/**
 * @brief Инициализация GPIO.
 * @details Настройка порта GPIOE для работы с кнопкой S1 (PE10).
 * PE10 настраивается как вход с подтяжкой к питанию.
 */
void gpio_init(void) {

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;   // Включаем тактирование GPIOE
 
  GPIOE->MODER &= ~GPIO_MODER_MODE10;    // Настройка PE10 как вход
  GPIOE->PUPDR |= GPIO_PUPDR_PUPD10_0;   // PE10 с подтяжкой к питанию (01: Pull-up)



}

