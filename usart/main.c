/*-----------------------------------------------------------------------------------------------------------
   @file        : main.c
   @brief       : Конфигурация GPIO, EXTI и USART для управления светодиодами и обработки нажатий кнопок
   @author      : xmatech
   @date        : 01.09.2024
   @board       : JZ-F407VET6
   @MCU         : STM32F407VET6 
   @IDE         : Segger Embedded Studio

   @Description : Программа содержит конфигурацию GPIO, EXTI и USART для управления светодиодами и обработки
                  нажатий кнопок на плате JZ-F407VET6 с использованием микроконтроллера STM32F407VET6.
                  
                  OPTION1: При использовании этой опции, обработчик прерываний EXTI отправляет цифры '1', '2', '3'
                  через USART1 при нажатии соответствующих кнопок S1, S2 и S3.
                  
                  OPTION2: При использовании этой опции, обработчик прерываний EXTI отправляет строки "Button S1\n",
                  "Button S2\n" и "Button S3\n" через USART1 при нажатии соответствующих кнопок S1, S2 и S3.

                  USART также обрабатывает входящие данные для включения и выключения светодиодов в зависимости 
                  от полученного символа ('0' выключает все светодиоды, '1', '2', '3' включают соответствующие 
                  светодиоды PE13, PE14, PE15).
                  
                  Для выбора OPTION1 или OPTION 2 необходимо их раскомментировать по одному в файле main.h
                  

   
   
   
--------------------------------------------------------------------------------------------------------------*/

#include "main.h"



int main(void) {

    SystemInit();   // Инициализация системы 
    RCC_Init();     // Настройка тактирования системы на 84 МГц (включая HSI/PLL)
    EXTI_Init();    // Инициализация EXTI для обработки прерываний от кнопок
    GPIO_Init();    // Настройка GPIO для управления светодиодами и кнопками
    USART1_Init();  // Инициализация USART1 для обмена данными по UART

    while (1) {}
}

/* Инициализация EXTI */
void EXTI_Init(void) {

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;           // включение тактирования SysCFG

    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE;  // Настройка EXTI10 на порт E (PE10)
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PE;  // Настройка EXTI11 на порт E (PE11)
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PE;  // Настройка EXTI12 на порт E (PE12)

    EXTI->PR   |= EXTI_PR_PR10;     // Сброс флага внешнего прерывания для канала EXTI10
    EXTI->FTSR |= EXTI_FTSR_TR10;   // Выбор срабатывания прерывания только по спаду
    EXTI->IMR  |= EXTI_IMR_IM10;    // Разрешение внешнего прерывания для канала EXTI10

    EXTI->PR   |= EXTI_PR_PR11;     // Сброс флага внешнего прерывания для канала EXTI11
    EXTI->FTSR |= EXTI_FTSR_TR11;   // Выбор срабатывания прерывания только по спаду
    EXTI->IMR  |= EXTI_IMR_IM11;    // Разрешение внешнего прерывания для канала EXTI11

    EXTI->PR   |= EXTI_PR_PR12;     // Сброс флага внешнего прерывания для канала EXTI12
    EXTI->FTSR |= EXTI_FTSR_TR12;   // Выбор срабатывания прерывания только по спаду
    EXTI->IMR  |= EXTI_IMR_IM12;    // Разрешение внешнего прерывания для канала EXTI12

    __enable_irq();                 // Включение глобальных прерываний 
    NVIC_EnableIRQ(EXTI15_10_IRQn); // Разрешение прерываний в контроллере NVIC для каналов с EXTI10 по EXTI15
}

/* Инициализация USART1 */
void USART1_Init(void) {

    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;                                        // вкл. тактирования GPIOA

    GPIOA->MODER  |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;                   // Альт. режим работы 
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos); // AF7 для A9, A10

    RCC->APB2ENR  |= RCC_APB2ENR_USART1EN;                                       // Включение тактирования USART1
    // 84MHz / 115200bod / 16 = 45,57  M=45 (0x2D) F=0,57*16=9 (0x09)
    USART1->BRR    = 0x02D9;                                                     // Boudrate = 115200
                   
    USART1->CR1   |= USART_CR1_RXNEIE;                                           // Прерывание по приему 
    USART1->CR1   |= USART_CR1_TE | USART_CR1_RE;                                //  Вкл. передатчик и приемник
                  
    USART1->CR1   &= ~(USART_CR1_M) | ~(USART_CR1_PCE);                          // 8-бит, без контроля четности  
    USART1->CR2   &= ~(USART_CR2_STOP);                                          // 1 стоповый бит                                                   
                                                                                 
    NVIC_EnableIRQ(USART1_IRQn);                                                 // Разрешение прерывания NVIC
                                                                                 
    USART1->CR1   |= USART_CR1_UE;                                               // Включение USART1
}

/* Инициализация GPIOE */
void GPIO_Init(void) {

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;                                             // вкл. тактирования GPIOE
                                                           
    GPIOE->MODER |= GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0; // Работа E13,E14,15 на выход (LED1,LED2,LED3) 
    GPIOE->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);    // работа E10,E11,E12 на вход (S1,S2,S3) 

    ALL_LEDS_OFF;                                                                    // Предварительное отключение LED по дефолту
}

/* Функция отправки строки через USART */
void sendStringUSART(char* str) {
    while (*str) {                            // Пока не достигнут конец строки (символ '\0')
        while (!(USART1->SR & USART_SR_TXE)); // Ожидание готовности передатчика (TXE = 1)
        USART1->DR = *str++;                  // Запись символа в регистр данных USART и переход к следующему символу
    }
}


/* Обработчик прерывания EXTI15_10 для OPTION1 */
#if defined(OPTION1)
void EXTI15_10_IRQHandler(void) {

    if (EXTI->PR & EXTI_PR_PR10) {  // Проверка флага прерывания для EXTI10
        USART1->DR = '1';           // Отправка символа '1' через USART1
        EXTI->PR |= EXTI_PR_PR10;   // Сброс флага прерывания для EXTI10
    }

    if (EXTI->PR & EXTI_PR_PR11) {  // Проверка флага прерывания для EXTI11
        USART1->DR = '2';           // Отправка символа '2' через USART1
        EXTI->PR |= EXTI_PR_PR11;   // Сброс флага прерывания для EXTI11
    }

    if (EXTI->PR & EXTI_PR_PR12) {  // Проверка флага прерывания для EXTI12
        USART1->DR = '3';           // Отправка символа '3' через USART1
        EXTI->PR |= EXTI_PR_PR12;   // Сброс флага прерывания для EXTI12
    }
}


/* Обработчик прерывания EXTI15_10 для OPTION2 */
#elif defined(OPTION2)
void EXTI15_10_IRQHandler(void) {

    if (EXTI->PR & EXTI_PR_PR10) {        // Проверка флага прерывания для EXTI10
        sendStringUSART("Button S1\n\n"); // Отправка строки "Button S1\n\n" через USART1 при нажатии S1
        EXTI->PR |= EXTI_PR_PR10;         // Сброс флага прерывания для EXTI10
    }
    if (EXTI->PR & EXTI_PR_PR11) {        // Проверка флага прерывания для EXTI11
        sendStringUSART("Button S2\n");   // Отправка строки "Button S2\n" через USART1 при нажатии S2
        EXTI->PR |= EXTI_PR_PR11;         // Сброс флага прерывания для EXTI11
    }                                     
    if (EXTI->PR & EXTI_PR_PR12) {        // Проверка флага прерывания для EXTI12
        sendStringUSART("Button S3\n");   // Отправка строки "Button S3\n" через USART1 при нажатии S3
        EXTI->PR |= EXTI_PR_PR12;         // Сброс флага прерывания для EXTI12
    }
}
#endif


/* Обработчик прерывания USART1 */
void USART1_IRQHandler(void) {
    if ((USART1->SR & USART_SR_RXNE) != 0) {  // Проверка флага приема данных (RXNE = 1)
        uint16_t RXc = (uint16_t)(USART1->DR & (uint16_t)0x01FF);  // Чтение принятого символа из регистра данных

        switch (RXc) {
            case '0':
                ALL_LEDS_OFF;  // Выключение всех светодиодов
                break;
            case '1':
                LED1_ON;  // Включение светодиода LED1 (PE13)
                break;
            case '2':
                LED2_ON;  // Включение светодиода LED2 (PE14)
                break;
            case '3':
                LED3_ON;  // Включение светодиода LED3 (PE15)
                break;
            default:
                break;  // Игнорирование других символов
        }
    }

    NVIC_ClearPendingIRQ(USART1_IRQn);  // Сброс запроса прерывания USART1
}