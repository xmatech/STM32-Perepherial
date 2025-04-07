/**
---------------------------------------------------------------------------------------------------------------------------------------------
 * @file        : main.h
 * @brief       : Заголовочный файл для основного модуля программы.
 * @author      : xmatech
 * @date        : 14.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : Данный файл содержит объявления глобальных переменных для отслеживания времени,
 *                используемых в программе для управления светодиодами LED1 и LED2.
 *                Также подключается библиотека STM32F4 для работы с периферией микроконтроллера.
---------------------------------------------------------------------------------------------------------------------------------------------
*/

#include <stm32f4xx.h>


// Объявление глобальных переменных для отслеживания времени
extern  volatile uint32_t time_ms ;      // Счетчик времени в миллисекундах
extern  volatile uint32_t delay_500ms;   // Задержка для мигания LED1 каждые 500 мс
extern  volatile uint32_t delay_10ms;    // Задержка для изменения яркости LED2 каждые 10 мс

