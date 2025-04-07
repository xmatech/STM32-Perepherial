/**
---------------------------------------------------------------------------------------------------------------
 * @file        : spi2_init.c
 * @brief       : Инициализация SPI2 для работы с внешней памятью W25Q64.
 * @author      : xmatech
 * @date        : 13.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 * @Description : Функция spi2_init() настраивает SPI2 для взаимодействия с внешней памятью W25Q64.
 *                - Настраиваются выводы GPIO для SPI2: PC3 (MOSI), PC2 (MISO), PB10 (SCK), PE3 (CS).
 *                - SPI2 настраивается в режиме Master с частотой тактирования 1,32 МГц (делитель 32).
 *                - Выполняется инициализация памяти W25Q64: сброс, разрешение записи, стирание сектора.
 *                - После инициализации SPI2 готов к использованию для чтения/записи данных.
 ---------------------------------------------------------------------------------------------------------------
 */

#include "w25q64.h"

/**
 * @brief Инициализация SPI2 для работы с памятью W25Q64.
 * 
 * Функция выполняет следующие шаги:
 * 1. Настраивает GPIO для работы с SPI2:
 *    - PC3 (MOSI), PC2 (MISO), PB10 (SCK) в альтернативном режиме (AF5).
 *    - PE3 (CS) в режиме вывода.
 * 2. Включает тактирование SPI2.
 * 3. Настраивает SPI2:
 *    - Режим Master.
 *    - Делитель частоты 32 (частота SPI2 = 1,32 МГц).
 *    - 8-битный формат данных.
 *    - Программное управление сигналом NSS.
 * 4. Инициализирует память W25Q64:
 *    - Сброс памяти (команды EN_RST и RST).
 *    - Разрешение записи (команда WR_EN).
 *    - Стирание сектора (команда SECT_ER).
 *    - Ожидание завершения операций записи/стирания.
 */
void spi2_init(void) {
  // Включение тактирования GPIOB, GPIOC, GPIOE
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN;

  // Настройка PC3 (MOSI) и PC2 (MISO) в альтернативном режиме (AF5)
  GPIOC->MODER  |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;                   //Альт. режим работы  
  GPIOC->AFR[0] |= (5 << GPIO_AFRL_AFSEL2_Pos) | (5 << GPIO_AFRL_AFSEL3_Pos); // AF5 для PC2,PC3  
  GPIOC->PUPDR  |= GPIO_PUPDR_PUPD3_1 | GPIO_PUPDR_PUPD2_1;                   // PC3,PC2 pull-down

  // Настройка PB10 (SCK) в альтернативном режиме (AF5)
  GPIOB->MODER  |= GPIO_MODER_MODE10_1;                                       // Альт. режим работы PB10
  GPIOB->AFR[1] |= GPIO_AFRH_AFRH2_2 | GPIO_AFRH_AFRH2_0;                     // AF5 для PB10
  GPIOB->PUPDR  |= GPIO_PUPDR_PUPD10_1;                                       // PB10 pull-down
                                                                              
  // Настройка PE3 (CS) в режиме вывода                                       
  GPIOE->MODER |= GPIO_MODER_MODE3_0;                                         // Режим работы на выход PE3
  GPIOE->OTYPER &= ~(GPIO_OTYPER_OT3);                                        // Режим Push-Pull
  GPIOE->PUPDR |= GPIO_PUPDR_PUPD3_1;                                         // Подтяжка вниз
  GPIOE->BSRR |= GPIO_BSRR_BS3;                                               // Установка CS в неактивное состояние (CS = 1)

  // Включение тактирования SPI2
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  // Настройка SPI2
  SPI2->CR1 |= SPI_CR1_MSTR | // Режим Master
               SPI_CR1_BR_2 | // Делитель частоты 32 (частота SPI2 = 1,32 МГц)
               SPI_CR1_SSM |  // Программное управление NSS
               SPI_CR1_SSI;   // Уровень NSS = 1
  SPI2->CR1 &= ~(SPI_CR1_DFF); // 8-битный формат данных

  // Включение SPI2
  SPI2->CR1 |= SPI_CR1_SPE;

  // Инициализация памяти W25Q64
  CSLOW;
  w25send(EN_RST); // Команда включения сброса (0x66)
  CSHIGH;

  CSLOW;
  w25send(RST); // Команда сброса (0x99)
  CSHIGH;

  CSLOW;
  w25send(WR_EN); // Команда разрешения записи (0x06)
  CSHIGH;

  CSLOW;
  w25send(SECT_ER); // Команда стирания сектора (0x20)
  w25send((ADDR >> 16) & 0xFF); // Адрес сектора (старший байт)
  w25send((ADDR >> 8) & 0xFF);  // Адрес сектора (средний байт)
  w25send(ADDR & 0xFF);         // Адрес сектора (младший байт)
  CSHIGH;

  // Ожидание завершения стирания сектора
  CSLOW;
  w25send(RD_SR1); // Чтение статусного регистра 1 (0x05)
  while ((w25send(0x00) & 0x01) == 1); // Ожидание завершения операции
  CSHIGH;

  // Повторное разрешение записи
  CSLOW;
  w25send(WR_EN); // Команда разрешения записи (0x06)
  CSHIGH;
}