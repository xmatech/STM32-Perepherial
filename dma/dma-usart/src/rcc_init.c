/**
-------------------------------------------------------------------------------------------------------------------------------------------------------
 * @file        : rcc_init.c
 * @brief       : Инициализация тактирования системы с использованием HSE и PLL для получения частоты 84 МГц.
 * @author      : xmatech
 * @date        : 23.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 *
 * @Description : Функция rcc_init() настраивает тактирование микроконтроллера с использованием внешнего кварцевого резонатора (HSE) и PLL.
 *                - HSE (High Speed External) используется в качестве источника тактирования.
 *                - PLL (Phase-Locked Loop) настраивается для получения частоты 84 МГц для SYSCLK.
 *                - Частоты шин APB1 и APB2 настраиваются на 42 МГц и 84 МГц соответственно.
 *                - Также выполняется настройка регистров FLASH-памяти для обеспечения стабильной работы на высокой частоте.
 -------------------------------------------------------------------------------------------------------------------------------------------------------
 */
#include <stm32f4xx.h>

/**
 * @brief Инициализация тактирования системы.
 * 
 * Функция выполняет следующие шаги:
 * 1. Включает HSE и ожидает его готовности.
 * 2. Отключает PLL перед настройкой.
 * 3. Настраивает PLL для получения частоты 84 МГц:
 *    - HSE_CLK = 25 МГц (внешний кварцевый резонатор).
 *    - VCO_CLK = 168 МГц (HSE_CLK / M * N, где M = 25, N = 168).
 *    - PLL_CLK = 84 МГц (VCO_CLK / 2).
 * 4. Настраивает делители для шин:
 *    - SYSCLK               = 84 МГц.
 *    - APB1_CLK             = 42 МГц.
 *    - APB2_CLK             = 84 МГц.
 *    - AHB1 = AHB2 = SYSCLK = 84 МГц
 * 5. Включает PLL и ожидает его готовности.
 * 6. Настраивает FLASH-память для работы на высокой частоте.
 * 7. Переключает SYSCLK на PLL и ожидает завершения переключения.
 */
void rcc_init(void) {
  // Включение HSE и ожидание его готовности
  RCC->CR |= RCC_CR_HSEON;
  while ((RCC->CR & RCC_CR_HSERDY) == 0) {}

  // Отключение PLL перед настройкой
  RCC->CR &= ~(RCC_CR_PLLON);


  // Настройка PLL: выбор HSE в качестве источника тактирования
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

  // Настройка предделителя (M) и множителя (N) для PLL
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLM_3 | RCC_PLLCFGR_PLLM_0; // M = 25
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_3; // N = 168
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); // P = 2 (PLLCLK = VCO_CLK / 2 = 84 МГц)

  // Настройка делителей для шин
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;

  // Включение PLL и ожидание его готовности
  RCC->CR |= RCC_CR_PLLON;
  while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}

  // Настройка FLASH-памяти для работы на высокой частоте
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS | FLASH_ACR_PRFTEN;

  // Переключение SYSCLK на PLL и ожидание завершения переключения
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}