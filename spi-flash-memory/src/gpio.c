/**
-------------------------------------------------------------------------------------------------------------------------------------------------------
 * @file        : gpio.c
 * @brief       : Инициализация GPIO и управление светодиодами (LED1, LED2, LED3) и кнопками (S1, S2, S3).
 * @author      : xmatech
 * @date        : 13.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 * @Description : Этот файл содержит функции для настройки GPIO и управления светодиодами на основе данных, считанных из внешней памяти W25Q64.
 *                - Функция gpio_init() настраивает порт GPIOE: выводы E13, E14, E15 как выходы для управления светодиодами (LED1, LED2, LED3),
 *                  а выводы E10, E11, E12 как входы для обработки нажатий кнопок (S1, S2, S3).
 *                - Функция switch_led() управляет светодиодами в зависимости от значения переменной memrd, которая содержит данные, считанные из памяти.
----------------------------------------------------------------------------------------------------------------------------------------------------------
*/

#include "main.h"
#include "gpio.h"
#include "w25q64.h"

/**
 * @brief Инициализация GPIO для управления светодиодами и кнопками.
 * 
 * Функция настраивает порт GPIOE:
 * - Выводы E13, E14, E15 настроены как выходы (LED1, LED2, LED3).
 * - Выводы E10, E11, E12 настроены как входы (S1, S2, S3).
 * - По умолчанию все светодиоды выключены.
 */
void gpio_init(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Включение тактирования порта E

  // Настройка выводов E13, E14, E15 как выходов (LED1, LED2, LED3)
  GPIOE->MODER |= GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0;
  GPIOE->ODR |= GPIO_ODR_OD13 | GPIO_ODR_OD14 | GPIO_ODR_OD15; // Выключение всех светодиодов по умолчанию
  GPIOE->OTYPER &= ~(GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15); // Режим выхода Push-Pull
  GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD15); // Без подтягивающих резисторов

  // Настройка выводов E10, E11, E12 как входов (S1, S2, S3)
  GPIOE->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);
}

/**
 * @brief Управление светодиодами на основе считанных данных.
 * 
 * Функция переключает светодиоды в зависимости от значения переменной memrd:
 * - Если memrd == LED1, включается LED1, остальные выключаются.
 * - Если memrd == LED2, включается LED2, остальные выключаются.
 * - Если memrd == LED3, включается LED3, остальные выключаются.
 * - Если memrd == CLEAR, включаются все светодиоды.
 * - В остальных случаях все светодиоды выключаются.
 */
void switch_led(void) {
  switch (memrd) {
    case LED1:
      LED1_ON;   // Включение LED1
      LED2_OFF;  // Выключение LED2
      LED3_OFF;  // Выключение LED3
      break;
    case LED2:
      LED1_OFF;  // Выключение LED1
      LED2_ON;   // Включение LED2
      LED3_OFF;  // Выключение LED3
      break;
    case LED3:
      LED1_OFF;  // Выключение LED1
      LED2_OFF;  // Выключение LED2
      LED3_ON;   // Включение LED3
      break;
    case CLEAR:
      ALL_LEDS_ON; // Включение всех светодиодов
      break;
    default:
      ALL_LEDS_OFF; // Выключение всех светодиодов
      break;
  }
}