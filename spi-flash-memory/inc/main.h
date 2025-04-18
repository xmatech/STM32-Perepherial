/**
---------------------------------------------------------------------------------------------------------------------------------------------
 * @file        : main.h
 * @brief       : Заголовочный файл для основного модуля программы.
 * @author      : xmatech
 * @date        : 13.01.2025
 * @board       : JZ-F407VET6
 * @MCU         : STM32F407VET6
 * @IDE         : Segger Embedded Studio
 * @Description : Этот файл содержит объявления и определения, используемые в основном модуле программы.
 *                - Объявление глобальной переменной `memrd` для хранения данных, считанных из памяти W25Q64.
 *                - Подключение заголовочного файла stm32f4xx.h для работы с регистрами микроконтроллера.
 * @warning     : Глобальная переменная `memrd` должна использоваться с осторожностью, чтобы избежать конфликтов при многопоточном доступе.
 * @note        : Для корректной работы программы убедитесь, что все модули (GPIO, SPI, W25Q64) инициализированы.
 * @dependencies: Требуется stm32f4xx.h для работы с регистрами микроконтроллера.
---------------------------------------------------------------------------------------------------------------------------------------------
*/

#include <stm32f4xx.h>

/**
 * @brief Глобальная переменная для хранения данных, считанных из памяти W25Q64.
 */
extern uint8_t memrd;