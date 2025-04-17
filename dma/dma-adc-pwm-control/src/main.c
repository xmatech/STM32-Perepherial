/**
 * @file        : main.c
 * @brief       : Управление яркостью светодиода с помощью потенциометра через АЦП, DMA и ШИМ.
 * @author      : xmatech
 * @date        : 14.03.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : АЦП измеряет значение на аналоговом входе PA5 (напряжение на потенциометре), затем записывает эти данные в память
 *                с помощью модуля DMA. В прерывании по окончанию передачи DMA данные отправляются в таймер, который
 *                работает в режиме ШИМ и управляет яркостью светодиода PE14.
 *                Таким образом, регулируется яркость светодиода с помощью потенциометра посредством передачи данных в память.
 */



#include "main.h"

#include <stm32f4xx.h>

uint16_t buffer [8] __attribute__ ((section(".fast"))); // Буфер для хранения данных АЦП

 int main(void) {

  SystemInit();        // Инициализация системы
  rcc_init();          // Устаовка тактирования на 84 МГц  
  tim1_init();         // Инициализация TIM2
  adc1_init();         // Инициализация ADC1
  DMA2_Stream0_Init(); // Инициализация DMA2 Stream 0

  while (1) {
        // Основной цикл
    }
}

/**
    @brief Инициализация таймера TIM1.
    @details Настраивает TIM1 для работы в режиме ШИМ с частотой 1 кГц и управляет яркостью светодиода на PE14.
*/
void tim1_init(void) {

    RCC -> APB2ENR   |= RCC_APB2ENR_TIM1EN;          // вкл. тактирования TIM1 (APB2 - 84 МГц)
    RCC -> AHB1ENR   |= RCC_AHB1ENR_GPIOEEN;         // вкл. тактирования GPIOE
                                                   
    /* Настройка PE13 (TIM1_CH3) в режиме альтернативной функции  */                        
    GPIOE -> MODER   |=  GPIO_MODER_MODE13_1;      // Альт. фунуция для PE13
    GPIOE -> OSPEEDR |= GPIO_OSPEEDR_OSPEED13_1;   // Высокая скорость
    GPIOE -> AFR[1]  |= GPIO_AFRH_AFRH5_0;         // AF1 для PE13  (5-й набор бит в регистре)  

    /* Настройка TIM1*/ 
    TIM1 -> PSC       = 84 - 1;                       // Частота тактирования - 1МГц  ( APB2(84 МГц)/PSC(83+1) )
    TIM1 -> CR1      |= TIM_CR1_CMS;                  // Выравнивание по центру 
    TIM1 -> ARR       = 1000 - 1;                          // Частота ШИМ - 1кГц   (APB2(84)/PSC/ARR = TIMfreq) 
    TIM1 -> CCR3      = 1;                            // Начальное значение ШИМ  
    TIM1 -> CCMR2    |= TIM_CCMR2_OC3M;               // PWM Mode 2
    TIM1 -> CCMR2    &= ~(TIM_CCMR2_CC3S);            // Режим работы - выход (OC3)
    TIM1 -> CCER     |= TIM_CCER_CC3E;                // Включение канала сравнения 3
    TIM1 -> BDTR     |= TIM_BDTR_MOE;                 // Включение выхода в блоке dead-time
                                                                             
    TIM1 -> CR1      |= TIM_CR1_CEN;                  // Включение счетчика
    TIM1 -> EGR      |= TIM_EGR_UG;                   // Обновление регистров (событие UEV)

}   


/**
    @brief Инициализация ADC1.
    @details Настраивает ADC1 для измерения напряжения на PA5 (ADC1_CH5) и передачи данных через DMA.
*/
void adc1_init(void) {
    
    RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN;          // Включение тактирования ADC1
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;         // Включение тактирования GPIOA
                                                                                 
    GPIOA -> MODER |= GPIO_MODER_MODE5;            // Аналоговый режим работы PA5 (ADC1_CH5)
                                                    
    ADC -> CCR     |= ADC_CCR_ADCPRE;              // PCLK / 8
    ADC1 -> SMPR2  |= ADC_SMPR2_SMP5_2;            // 84 + 12 циклов (конвертация произайдет за 84+12 = 96 циклов тактирования АЦП)

    ADC1->SQR1     &= ~ADC_SQR1_L;                 // Длина последовательности конвертации = 1
    ADC1->SQR3     |= 5 << ADC_SQR3_SQ1_Pos;       // Первая конвертация на 5-ом канале

    ADC1 -> CR2    |=  ADC_CR2_CONT;               // Непрерывный режим 
    ADC1 -> CR2    |= ADC_CR2_DMA | ADC_CR2_DDS;   // Включение непрерывных запросов DMA

    ADC1 -> CR2    |= ADC_CR2_ADON;                // Вкл. АЦП
    ADC1 -> CR2    |= ADC_CR2_SWSTART;             // Запуск конвертации

}






/**
    @brief Инициализация DMA2 Stream 0.
    @details Настраивает DMA2 Stream 0 для передачи данных из ADC1 в буфер.
*/
void DMA2_Stream0_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;           // Включение тактирования DMA2

    // Настройка DMA2 Stream 0 Channel 0 для ADC1
    DMA2_Stream0 -> PAR  =  (uint32_t)&(ADC1->DR);                 // Адрес регистра данных ADC1
    DMA2_Stream0 -> M0AR =  (uint32_t)buffer;                      // Адрес буфера для хранения данных
    DMA2_Stream0 -> NDTR = 8;                                     // Количество передаваемых данных
    DMA2_Stream0 -> FCR &= ~(DMA_SxFCR_DMDIS);                    // Прямой режим без FIFO
    DMA2_Stream0 -> CR  &= ~(DMA_SxCR_CHSEL);                     // Установка канала 0
    DMA2_Stream0 -> CR  &= ~(DMA_SxCR_MBURST | DMA_SxCR_PBURST);  // Одиночная передача
    DMA2_Stream0 -> CR  &= ~(DMA_SxCR_DBM);                       // Режим двойного буфера выключен
    DMA2_Stream0 -> CR  |=   DMA_SxCR_PL;                         // Высокий приоритет
    DMA2_Stream0 -> CR  |=   DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; // Размер данных памяти и переферии - полуслово (16 бит)
    DMA2_Stream0 -> CR  |=   DMA_SxCR_MINC;                       // Инкремент адреса памяти
    DMA2_Stream0 -> CR  &= ~(DMA_SxCR_PINC);                      // Без инкремента адреса периферии
    DMA2_Stream0 -> CR  |=   DMA_SxCR_CIRC;                       // Циклический режим
    DMA2_Stream0 -> CR  &= ~(DMA_SxCR_DIR);                       // Передача из периферии в память
    DMA2_Stream0 -> CR  |=   DMA_SxCR_TCIE;                       // Прерывание по завершению передачи
                                                               
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);                          // Разрешение прерывания DMA2 Stream 0
    DMA2_Stream0->CR    |= DMA_SxCR_EN;                           // Включение DMA2 Stream 0
}




/**
    @brief Обработчик прерывания DMA2 Stream 0.
    @details Обрабатывает данные, полученные от ADC1, и обновляет значение ШИМ для управления яркостью светодиода.
*/
void DMA2_Stream0_IRQHandler(void) {

    uint8_t i = 0;     // перемення для цикла for
    uint16_t ovr = 0;  //  переменная, которая названа по операции оверсемплинга, когда мы берем 
                       // несколько значений из АЦП и усредненное значение отпрвляем в TIM

   /* Усреднение значений из буфера  */
    for (i = 0; i < 8; i++) {
      ovr = ovr + buffer[i];
    }
     ovr /= 8;

    // Обновление значения ШИМ                          
    TIM1 -> CCR3 = (ovr * 1000) / 4096; // Вычисление значения и запись его в таймер  
                                        // 1000 - ARR, 4096 - разрядность АЦП

    DMA2 -> LIFCR |= DMA_LIFCR_CTCIF0; // Очистка флага прерывания
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
}
