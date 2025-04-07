/*-----------------------------------------------------------------------------------------------------------
   @file        : main.c
   @brief       : GPIO and EXTI 
   @author      : xmatech
   @date        : 01.09.2024
   @board       : STM32F407VET6 
   @IDE         : Segger Studio

   @Description : Программа предназначена для работы с отладочной платой STM32F407VET6.
                  Программа использует GPIO и внешние прерывания (EXTI) для управления тремя
                  светодиодами (LED1, LED2, LED3), подключенными к порту GPIOE микроконтроллера(E13,E14,E15),
                  и тремя кнопками (S1, S2, S3), подключенными к выводам E10, E11, E12.

                  Функциональность программы:
                  1) При нажатии кнопки S1 включается светодиод LED1.
                  2) При нажатии кнопки S2 включается светодиод LED2.
                  3) При нажатии кнопки S3 включается светодиод LED3.
                  
                  Для обработки нажатий кнопок используются внешние прерывания.
                  В обработчике прерываний определяется, какая кнопка была нажата, и в зависимости от
                  этого переключается состояние соответствующего светодиода.
-------------------------------------------------------------------------------------------------------------*/


#include <stm32f4xx.h>


/* Прототипы функций */
void RCC_Init(void);  // Прототип функции инициализации тактирования
void EXTI_Init(void); // Прототип функции инициализации внешних прерываний
void GPIO_Init(void); // Прототип функции инициализации GPIO



 int main(void) {

  SystemInit();
  RCC_Init();
  GPIO_Init();
  EXTI_Init();


while (1) {} 
}



/* Инициализация внешних прерываний (EXTI) */
void EXTI_Init(void) {

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Включение тактирования SYSCFG  
  
  //-----------------------------------------------
  // EXTICR[2] соответствует регистру SYSCFG_EXTICR3
  // EXTICR[3] соответствует регистру SYSCFG_EXTICR4
  //-----------------------------------------------
  
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE; // Порт E на линии 10
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PE; // Порт E на линии 11
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PE; // Порт E на линии 12
  
  EXTI->PR   |= EXTI_PR_PR10;                    // Сброс флага внешнего прерывания для канала EXTI10 
  EXTI->FTSR |= EXTI_FTSR_TR10;                  // Выбор срабатывания прерывания только по спаду
  EXTI->IMR  |= EXTI_IMR_IM10;                   // Разрешение внешнего прерывания для канала EXTI10
                                                 
  EXTI->PR   |= EXTI_PR_PR11;                    // Сброс флага внешнего прерывания для канала EXTI11
  EXTI->FTSR |= EXTI_FTSR_TR11;                  // Выбор срабатывания прерывания только по спаду
  EXTI->IMR  |= EXTI_IMR_IM11;                   // Разрешение внешнего прерывания для канала EXTI11
                                                 
  EXTI->PR   |= EXTI_PR_PR12;                    // Сброс флага внешнего прерывания для канала EXTI12
  EXTI->FTSR |= EXTI_FTSR_TR12;                  // Выбор срабатывания прерывания только по спаду
  EXTI->IMR  |= EXTI_IMR_IM12;                   // Разрешение внешнего прерывания для канала EXTI12
                                                 
  __enable_irq();                                // Включение глобальных прерываний 
  NVIC_EnableIRQ(EXTI15_10_IRQn);                // Разрешение прерываний в NVIC для каналов EXTI10-EXTI15

}

/* Инициализация GPIO */
void GPIO_Init(void) {

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Включение тактирования GPIOD
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Включение тактирования GPIOE
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Включение тактирования GPIOB
  
  // Настройка GPIOE: E13, E14, E15 на выход
  GPIOE->MODER |= GPIO_MODER_MODE13_0; // E13 на выход 
  GPIOE->MODER |= GPIO_MODER_MODE14_0; // E14 на выход 
  GPIOE->MODER |= GPIO_MODER_MODE15_0; // E15 на выход
  
  // Предварительное отключение LED по дефолту
  GPIOE-> ODR  |= GPIO_ODR_OD13;       // Выкл. LED 13
  GPIOE-> ODR  |= GPIO_ODR_OD14;       // Выкл. LED 14
  GPIOE-> ODR  |= GPIO_ODR_OD15;       // Выкл. LED 15
  
  // Настройка GPIOE: E10, E11, E12 на вход
  GPIOE->MODER &= ~GPIO_MODER_MODE10;  // E10 на вход (S1)
  GPIOE->MODER &= ~GPIO_MODER_MODE11;  // E11 на вход (S2)
  GPIOE->MODER &= ~GPIO_MODER_MODE12;  // E12 на вход (S3)

}



/* Обработчик прерывания кнопок */
void EXTI15_10_IRQHandler(void){

  switch (EXTI->PR)
  {    
       case EXTI_PR_PR10: 
            EXTI->PR  |= EXTI_PR_PR10;    // Сброс флага прерывания
            GPIOE->ODR ^= GPIO_ODR_OD13;  // Переключение состояния LED1
            break;
       
       case EXTI_PR_PR11: 
            EXTI->PR  |= EXTI_PR_PR11;    // Сброс флага прерывания
            GPIOE->ODR ^= GPIO_ODR_OD14;  // Переключение состояния LED2
            break;
       
       case EXTI_PR_PR12: 
            EXTI->PR  |= EXTI_PR_PR12;    // Сброс флага прерывания
            GPIOE->ODR ^= GPIO_ODR_OD15;  // Переключение состояния LED3
            break;
       
   default: EXTI->PR  |= EXTI_BASE;       // Сброс всех прерываний
  }
}




