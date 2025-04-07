/**
 * @file        : main.c
 * @brief       : Управление ШИМ сигналом с использованием АЦП на STM32F407VET6
 * @author      : xmatech
 * @date        : 25.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @description : Проект демонстрирует использование АЦП для считывания аналогового сигнала
 *                и управления коэффициентом заполнения ШИМ-сигнала на таймере TIM1.
 *                ШИМ используется для управления яркостью светодиода LED1,
 *                подключенного к выводу PE14 (TIM1_CH4).
 */

#include "main.h"




 int main(void) {

  SystemInit();   // Инициализация системы
  rcc_init();     // Настройка тактирования 
  tim1_init();    // Инициализация таймера TIM1
  adc1_init();    // Инициализация АЦП
  
 

while (1) {

  }
}


/**
 * @brief Инициализация таймера TIM1 для генерации ШИМ 
 */
void tim1_init(void) {

    // Включение тактирования TIM1 и GPIOE
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;         // вкл. тактирования TIM1 
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOEEN;        // вкл. тактирования GPIOE

    /* Настройка PE14 как альтернативной функции TIM1_CH4  */ 
    GPIOE -> MODER   |= GPIO_MODER_MODE14_1;      // Альт. функция для PE14
    GPIOE -> OSPEEDR |=  GPIO_OSPEEDR_OSPEED14_1; // Высокая скорость для PE14
    GPIOE -> AFR[1]  |= GPIO_AFRH_AFRH6_0;        // AF1 для PE14 

    /* Настройка TIM1*/ 
    TIM1 -> PSC    = 84 - 1;                      // Предделитель для частоты 1 МГц (84 МГц / 84 = 1 Мгц) 
    TIM1 -> ARR    = 1000 - 1;                    // Период ШИМ (1 кГц)  
    TIM1 -> CCR4   = 1;                           // Начальный КФ заполнения
    TIM1 -> CR1   |= TIM_CR1_CMS;                 // Выравнивание по центру 
    TIM1 -> CCMR2 |= TIM_CCMR2_OC4M;              // Режим PWM Mode 2
    TIM1 -> CCMR2 &= ~TIM_CCMR2_CC4S;             //  Выходной режим (OC4)
    TIM1 -> CCER  |= TIM_CCER_CC4E;               // Включение канала 4 
    TIM1 -> BDTR  |= TIM_BDTR_MOE;                // Включение выхода в блоке dead-time
                                                  
    TIM1 -> CR1   |= TIM_CR1_CEN;                 // Запуск таймера  
    TIM1 -> EGR   |= TIM_EGR_UG;                  // Обновление регистров s
}   


/**
 * @brief Инициализация АЦП1 (ADC1)
 */
void  adc1_init(void) {
    
    RCC   -> APB2ENR |= RCC_APB2ENR_ADC1EN;       // вкл. тактирования ADC1
    RCC   -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;      // вкл. тактирования GPIOA
                                                  
    GPIOA -> MODER   |= GPIO_MODER_MODE5;         // Настройка PA5 как аналогового входа (ADC1_IN5)
                                                  
    // Настройка АЦП                              
    ADC1 -> SMPR2 |= ADC_SMPR2_SMP5_0;            // Длительность выборки: 15 + 12 тактов
    ADC1 -> SQR1  &= ~ADC_SQR1_L;                 // Последовательность конверсии: 1 канал
    ADC1 -> SQR3  |= 5 << ADC_SQR3_SQ1_Pos;       // Первый канал конверсии: IN5
    ADC1 -> CR1   |= ADC_CR1_EOCIE;               // Включение прерывания по окончанию конверсии
    NVIC_EnableIRQ(ADC_IRQn);                     // Разрешение прерывания в NVIC
                                                  
    ADC1 -> CR2   |= ADC_CR2_CONT | ADC_CR2_ADON; // Непрерывный режим, включение АЦП
    ADC1 -> CR2   |= ADC_CR2_SWSTART;             // Запуск преобразования
}



/**
 * @brief Обработчик прерывания АЦП1
 * @details В регистр сравнения таймера записывается значение из регистра данных АЦП,
    но т.к  ARR = 999, нам необходимо привести значение из регистра ADC1->DR к 1000 
 */
void ADC_IRQHandler(void) {

    /* Обновление коэффициента заполнения ШИМ на основе данных АЦП */
    TIM1 -> CCR4 = (ADC1 -> DR * 1000)/4096; // Вычисление значения и запись его в таймер

 NVIC_ClearPendingIRQ(ADC_IRQn); // Cброс запроса прерывания
}

