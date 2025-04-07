/**
-------------------------------------------------------------------------------------------------------------------------------
 * @file        : gpio.h
 * @brief       : Заголовочный файл для управления GPIO и светодиодами.
 * @author      : xmatech
 * @date        : 14.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : Данный файл содержит прототипы функций и макросы для управления GPIO, 
 *                включая инициализацию GPIO и управление светодиодами LED1 и LED2.
 *                LED1 управляется через пин PE13, а LED2 использует ШИМ на TIM1_CH4.
 -------------------------------------------------------------------------------------------------------------------------------
 */



// Прототип функции
void gpio_init(void);  // Инициализация GPIO


/*
/**
 * @brief Макрос для включения LED1 (PE13).
 */

// Макросы для управления светодиодом LED1 на пине PE13
#define LED1_ON     GPIOE->ODR &= ~GPIO_ODR_OD13;  // Включение LED1 (PE13)
#define LED1_OFF    GPIOE->ODR |=  GPIO_ODR_OD13;  // Выключение LED1 (PE13)



// Прототипы функций для управления светодиодами
void led1_toggle(void);
void led2_updateBrightness(void);







