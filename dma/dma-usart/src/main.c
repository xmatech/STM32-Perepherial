/**
 * @file        main.c
 * @brief       Программа демонстрации работы DMA на STM32F407VET6
 *              - Копирует строку из массива bufferOUT в bufferIN через DMA (режим память-память)
 *              - Периодически отправляет строку по USART1 через DMA (режим память-периферия)
 *              - Передача осуществляется 1 раз в секунду с использованием SysTick для отсчёта времени
 *
 * @author      xmatech
 * @date        2023
 * @board       JZ-F407VET6
 * @MCU         STM32F407VET6 
 * @IDE         Segger Embedded Studio
 */

#include "main.h"

#define BUF_SIZE 14

// Массив, содержащий строку для передачи, расположен в секции ".fast"
uint8_t bufferOUT[BUF_SIZE] __attribute__((section(".fast"))) = "USART-DMA OK!\r\n";
// Массив для копирования строки, расположен в секции ".fast"
uint8_t bufferIN[BUF_SIZE] __attribute__((section(".fast")));

// Флаги и переменные для контроля передачи данных
volatile uint8_t transfer_complete = 1;   // Флаг завершения передачи DMA по USART1
volatile uint32_t systick_counter = 0;    // Счётчик для отсчёта времени SysTick



/**
 * @brief Основная функция программы. Инициализирует систему, выполняет копирование данных и настраивает периферию.
 */
int main(void) {
    SystemInit();                // Инициализация системы микроконтроллера
    rcc_init();                  // Включение тактирования необходимых периферийных устройств
    DMA2_Stream0_MEM2MEM_Init(); // Копирование данных: bufferOUT -> bufferIN через DMA (режим память-память)
    usart1_init();               // Инициализация USART1 для работы с DMA
    DMA2_Stream7_USART1_Init();  // Настройка DMA для передачи данных по USART1 (режим память-периферия)

 
    while (1) {

        DMA2_Stream7->CR |= DMA_SxCR_EN;   // Запуск потока DMA для передачи данных
        delay_ms(1000);                    // задержка в 1 секунду
    }
}



/**
 * @brief Инициализация USART1.
 *
 * Настраивает пины PA9 (TX) и PA10 (RX) для альтернативной функции USART1,
 * устанавливает скорость передачи 115200 бод.
 */
void usart1_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;                                        // Включение тактирования USART1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;                                         // Включение тактирования порта A

    // Настройка скорости передачи. В данном примере используется значение 0x2D9
    USART1->BRR = 0x2D9; // 115200 бод (при SystemCoreClock = 16 МГц)
    // Включение передатчика и самого USART1
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
    USART1->CR2 &= ~USART_CR2_STOP;                                              // Настройка на 1 стоп-бит
    USART1->CR3 |= USART_CR3_DMAT;                                               // Разрешение работы DMA для передачи данных

    // Настройка пинов PA9 (TX) и PA10 (RX) в альтернативный режим (AF7 для USART1)
    GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;                    // Режим альтернативной функции
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos); // AF7 для USART1
}

/**
 * @brief Инициализация DMA для копирования данных из памяти в память.
 *
 * Копирует содержимое массива bufferOUT в bufferIN.
 * Используется канал DMA2, Stream0 в режиме память-память.
 */
void DMA2_Stream0_MEM2MEM_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;                                // Включаем тактирование DMA2
    
    // Настройка DMA потока: направление, инкремент адресов и количество данных
    DMA2_Stream0->CR = DMA_SxCR_DIR_1 | DMA_SxCR_PINC | DMA_SxCR_MINC; // Режим память-память
    DMA2_Stream0->NDTR = BUF_SIZE;                                     // Размер буфера
    DMA2_Stream0->PAR = (uint32_t)bufferOUT;                           // Адрес источника (bufferOUT)
    DMA2_Stream0->M0AR = (uint32_t)bufferIN;                           // Адрес назначения (bufferIN)
    
    DMA2_Stream0->CR |= DMA_SxCR_EN;                                   // Запуск DMA
    // Ожидание завершения копирования
    while (!(DMA2->LISR & DMA_LISR_TCIF0));                            // Ожидаем флага завершения передачи
    // Сброс флага завершения передачи
    DMA2->LIFCR |= DMA_LIFCR_CTCIF0;                                   // Очистка флага завершения
}

/**
 * @brief Инициализация DMA для передачи данных по USART1.
 *
 * Используется DMA2 Stream7 для передачи данных из bufferIN в регистр данных USART1.
 * Канал настроен для режима "из памяти в периферию" с включенным прерыванием по завершению передачи.
 */
void DMA2_Stream7_USART1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;  // Включаем тактирование DMA2
    
    // Настройка DMA потока: выбор канала, направление, инкремент адресов, приоритет и прерывания
    DMA2_Stream7->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PL | DMA_SxCR_TCIE;
    DMA2_Stream7->NDTR = BUF_SIZE;             // Размер буфера для передачи
    DMA2_Stream7->PAR = (uint32_t)&USART1->DR; // Адрес регистра данных USART1 (периферия)
    DMA2_Stream7->M0AR = (uint32_t)bufferIN;   // Адрес источника данных (bufferIN)
    
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);         // Разрешаем прерывание по завершению передачи DMA
}

/**
 * @brief Обработчик прерывания DMA2 Stream7.
 *
 * При завершении передачи данных по DMA для USART1 сбрасывает флаг завершения передачи (transfer_complete).
 */
void DMA2_Stream7_IRQHandler(void) {
    if (DMA2->HISR & DMA_HISR_TCIF7) {    // Проверяем флаг завершения передачи
        DMA2->HIFCR |= DMA_HIFCR_CTCIF7;  // Сброс флага завершения передачи
        transfer_complete = 1;            // Устанавливаем флаг готовности к следующей передаче
    }
}



void delay_ms(uint32_t ms) {
  while (ms)
  {
    SysTick -> LOAD = 84000 - 1;                                             // Загрузка значения для 1 мс
    SysTick -> VAL  = 0;                                                     // Сброс счетчика
    SysTick -> CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;  // Запуск таймера
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);               // Ожидание переполнения
    ms--;
  }
}