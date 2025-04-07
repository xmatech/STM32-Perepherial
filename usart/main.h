#ifndef MAIN_H
#define MAIN_H

#include <stm32f4xx.h>

// Макросы для управления светодиодами
#define LED1_ON     GPIOE->ODR  &= ~GPIO_ODR_OD13
#define LED1_OFF    GPIOE->ODR  |= GPIO_ODR_OD13

#define LED2_ON     GPIOE->ODR  &= ~GPIO_ODR_OD14
#define LED2_OFF    GPIOE->ODR  |= GPIO_ODR_OD14

#define LED3_ON     GPIOE->ODR  &= ~GPIO_ODR_OD15
#define LED3_OFF    GPIOE->ODR  |= GPIO_ODR_OD15

// Макрос для отключения всех светодиодов
#define ALL_LEDS_OFF GPIOE->ODR |= (GPIO_ODR_OD13 | GPIO_ODR_OD14 | GPIO_ODR_OD15)

// Макрос для включения всех светодиодов
#define ALL_LEDS_ON GPIOE->ODR &= ~(GPIO_ODR_OD13 | GPIO_ODR_OD14 | GPIO_ODR_OD15)

// Конфигурационные опции
// раскоментируйте по одному
//#define OPTION1 1
  #define OPTION2 2

// Прототипы функций
void RCC_Init(void);
void EXTI_Init(void);
void USART1_Init(void);
void GPIO_Init(void);
void sendStringUSART(char* str);

#endif // MAIN_H





