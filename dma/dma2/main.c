/************************************************************************************************
                     Модуль прямого доступа к памяти DMA                                                      
1) Напишите программу, в которой выполняется копирование массива bufferOUT в                                  
массив bufferIN с помощью режима модуля DMA2 «из памяти в память» и отправка                                  
массива bufferIN по USART1 с помощью того же модуля DMA2 и режима «из памяти                                  
в периферию». При этом:                                                                                       
  • массив bufferOUT содержит строку “USART-DMA OK!” и имеет достаточный                                      
    для нее размер;                                                                                           
  • копирование массивов производится до основного цикла программы;                                           
  • USART1 настроен на скорость 115200 бод и на работу с ПДП;                                                 
  • отправка данных происходит 1 раз в секунду.                                                               
2) Проверьте работу программы, подключив USB-UART преобразователь к плате и                                   
запустив терминал.
                                                                                            
Замечания:                                                                                                    
Для отсчета времени примените таймер SysTick.                                                                 
Учитывайте размер данных, которые будут передаваться по USART1 при настройке                                  
потоков DMA2.                                                                                                 
Не забудьте указать область памяти, где будут расположены массивы, для корректной                             
работы прямого доступа к памяти.     
                                                                         
Советы:                                                                                                        
Присвоение значения массива производится после attribute ((section(".fast"))) в                               
фигурных скобках – {“Строка”}.                                                                                
В конец строки можно добавить строковые литералы возврата каретки \r и конца                                  
строки \n. При этом значения в терминале будут отображаться в столбик и без лишних символов.                                                                                                                 
*************************************************************************************************/ 

/* */
/*  ***  */
/*** • ***/

#include <stm32f4xx.h>
#include <stdio.h>
#include <stdlib.h>


#define i 14

uint8_t bufferOUT[i] __attribute__((section(".fast"))) = {"USART-DMA OK!\r\n"};
uint8_t bufferIN[i] __attribute__((section(".fast")));





/* Прототипы функций */ 
void RCC_Init(void);
void USART1_Init(void);
void DMA2_Stream0_MEM2MEM_Init(void);
void DMA2_Stream7_USART1_Init(void);
void SysTick_Init(void);



volatile uint8_t transfer_complete = 0; // Флаг завершения передачи DMA
volatile uint8_t dma_mem_transfer_complete = 0; // Флаг завершения DMA memory-to-memory
volatile uint8_t test_counter[7] = {0, 0, 0, 0, 0, 0, 0};  // Массив для хранения значений счетчика-проверки
volatile uint32_t systick_counter = 0; // счетчик SysTick




 int main(void) {

  SystemInit();                 // Инициализация системы
  RCC_Init();                   // Устаовка тактирования на 84 МГц  
  DMA2_Stream0_MEM2MEM_Init();
  USART1_Init();
  DMA2_Stream7_USART1_Init(); 
  SysTick_Init();               // Инициализация SysTick 

              

while (1) 
  {
    test_counter[0]++;
  }
}




// --- Инициализация SysTick ---
void SysTick_Init(void) {
    SysTick_Config(SystemCoreClock/1);    // Прерывание каждую секунду, а вышло 200 мс
}


// --- Обработчик прерывания SysTick ---
void SysTick_Handler(void) {
systick_counter++;

    if (systick_counter >= 5) 
    {   // По заданию прерывание 1 раз/сек, почему-то прерываение происходит 5 раз в секунду. добавил счет до 5
        systick_counter = 0;              // Сбрасываем счетчик

        // Код для периодической отправки по USART
        if (transfer_complete) 
        {       
            DMA2_Stream7 -> CR |= DMA_SxCR_EN;  // Запускаем DMA
            transfer_complete = 0;            // Сбрасываем флаг завершения
            SystemCoreClockUpdate();          // Обновляем SystemCoreClock
            test_counter[1]++;
        }   
    }
}


/*** DMA2_USART1 - Из памяти в перефию ***/
 void USART1_Init(void) {

    RCC -> APB2ENR  |= RCC_APB2ENR_USART1EN;
    RCC -> AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;

                             
    USART1 -> BRR    = 0x2D9;                                                        /* 84MHz / (115200 * 16) = 45.57 // M=45 (0x2D), F=0.57*16=(0x09)*/ 
                                                                                    //где 84MHz - частота шины APB2, 115200 - бодрейт. 

    USART1 -> CR1   |= USART_CR1_TE;                                               //  Вкл. передатчик
    USART1 -> CR1   &= ~(USART_CR1_M) | ~(USART_CR1_PCE);                          // 8-бит, без контроля четности 
    USART1 -> CR2   &= ~(USART_CR2_STOP);                                          // 1 стоповый бит


    GPIOA -> MODER  |=  (GPIO_MODER_MODE9_1 |  GPIO_MODER_MODE10_1);               //  Альт. режим работы 
    GPIOA -> AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos); // AF7 для A9, A10

    USART1 -> CR3   |= USART_CR3_DMAT;                                             // DMA Trancieve
                                                                                    
    USART1 -> CR1   |= USART_CR1_UE;                                               // Включение USART1

 }


/*** DMA2 - Из памяти в память ***/
 void DMA2_Stream0_MEM2MEM_Init(void) {

    RCC -> AHB1ENR      |= RCC_AHB1ENR_DMA2EN; 

    DMA2_Stream0 -> CR  |= DMA_SxCR_DIR_1;                      // 1 0 - MEM2MEM // из памяти в память
    DMA2_Stream0 -> CR  &= ~( DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); // Memory data size | Peripheral data size = 8 bit
    DMA2_Stream0 -> CR  |= DMA_SxCR_CIRC;                       // Циклический режим 
    DMA2_Stream0 -> CR  |= DMA_SxCR_PINC;                       // Инкрементирование адреса периферии(source)
    DMA2_Stream0 -> CR  |= DMA_SxCR_MINC;                       // Инкрементирование адреса памяти (destination)
    
    DMA2_Stream0->NDTR   = i;           // Количество передаваемых данных

    DMA2_Stream0->PAR    = (uint32_t)&bufferOUT;                //Адрес периферии (source) // note: прописывать &
    DMA2_Stream0->M0AR   = (uint32_t)&bufferIN;                 // Адрес памяти (destination) // note!!! : адрес 32-битный, задаем как uint32_t

    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    DMA2_Stream0->CR    |= DMA_SxCR_EN; // Вкл. DMA2_Stream0

 }    


 void DMA2_Stream7_USART1_Init(void) {

     RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN; // Тактирование DMA2
 
    // DMA2_Stream7, Channel 4 для USART1_TX
    DMA2_Stream7 ->FCR &= ~(DMA_SxFCR_DMDIS);                   // Прямой режим без FIFO
    DMA2_Stream7 -> CR &= ~(DMA_SxCR_MBURST | DMA_SxCR_PBURST); // Одиночная передача (не пакетная)
    DMA2_Stream7 -> CR &= ~(DMA_SxCR_DBM);                      // Режим двойного буфера ВЫКЛ 
    DMA2_Stream7 -> CR &= ~(DMA_SxCR_PINC);                     // Без инкрементирования адреса периферии
    
    DMA2_Stream7 -> CR |= DMA_SxCR_CHSEL_2 ;                    // 100: channel 4 selected
    DMA2_Stream7 -> CR |= DMA_SxCR_MINC;                        // Инкремент адреса памяти MINC: Memory increment mode 1: Memory address pointer is incremented after each data transfer (increment is done according to MSIZE)
    DMA2_Stream7 -> CR |= DMA_SxCR_DIR_0;                       // 01: Memory-to-peripheral. DIR[1:0]: Data transfer direction
    DMA2_Stream7 -> CR |= DMA_SxCR_PL;                          // Самый высокий приоритет
    DMA2_Stream0 -> CR &= ~( DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);  // Memory data size | Peripheral data size = 8 bit
    DMA2_Stream7 -> CR |= DMA_SxCR_TCIE;                        // Прерывание по завершению
    //DMA2_Stream7->CR |= DMA_SxCR_CIRC;                        // Циклический режим (не ставить цикл. режим, т.к передача будет очень быстрой)

    DMA2_Stream7->NDTR  = i;            // Количество передаваемых данных

    DMA2_Stream7->PAR   = (uint32_t)&USART1->DR; // Адрес периферии 
    DMA2_Stream7->M0AR  = (uint32_t)&bufferIN;   // Адрес памяти


    NVIC_EnableIRQ(DMA2_Stream7_IRQn);   // Разрешение прерывания в NVIC
    DMA2_Stream7->CR   |= DMA_SxCR_EN;   //Вкл. DMA2_Stream7
    

   // Запуск DMA2, Stream7 происходит в обработчике прерывания SysTick
}




void DMA2_Stream7_IRQHandler(void) {
    test_counter[4]++;
    if (DMA2->HISR & DMA_HISR_TCIF7)         // Проверка, что прерывание вызвано завершением передачи
    {    

        DMA2->HIFCR |= DMA_HIFCR_CTCIF7;     // Сбрасываем флаг завершения передачи // Writing 1 to this bit clears the corresponding TCIFx flag in the DMA_HISR register
        transfer_complete = 1;               // Устанавливаем флаг для следующей передачи
        test_counter[5]++;
    }
    test_counter[6]++;

    NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
}



void DMA2_Stream0_IRQHandler(void) {
    
    test_counter[2]++;
    if (DMA2->LISR & DMA_LISR_TCIF0)         // Проверка флага завершения передачи для Stream 0
    {                                         
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;     // Сброс флага завершения передачи
        dma_mem_transfer_complete = 1;       // Установка флага завершения
        test_counter[3]++;
    }
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
}

