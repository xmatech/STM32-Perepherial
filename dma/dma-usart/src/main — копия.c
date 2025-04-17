/**
 * @file        main.c
 * @brief       Программа демонстрации работы DMA на STM32F407VET6

 * @author      xmatech
 * @date        2023
 * @board       JZ-F407VET6
 * @MCU         STM32F407VET6 
 * @IDE         Segger Embedded Studio


 * @Description Программа копирует строку из одного массива в другой с помощью DMA2 (режим память-память),
 *              затем периодически отправляет эту строку по USART1 с использованием DMA2 (режим память-периферия).
 *              Отправка происходит 1 раз в секунду с использованием SysTick для отсчета времени.
 */


#include "main.h"




#define BUF_SIZE 14

uint8_t bufferOUT[BUF_SIZE] __attribute__((section(".fast"))) = "USART-DMA OK!\r\n";
uint8_t bufferIN[BUF_SIZE] __attribute__((section(".fast")));

volatile uint8_t transfer_complete = 1;
volatile uint32_t systick_counter = 0;

int main(void) {
    SystemInit();
    rcc_init();  
    DMA2_Stream0_MEM2MEM_Init();
    usart1_init();
    DMA2_Stream7_USART1_Init();
   SysTick_Config(SystemCoreClock / 2);
 
    while (1) {}
}

void SysTick_Handler(void) {
    if (++systick_counter >= 10) {
        systick_counter = 0;
        if (transfer_complete) {
            transfer_complete = 0;
            DMA2_Stream7->CR |= DMA_SxCR_EN;
        }
    }
}

void usart1_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    USART1->BRR = 0x2D9;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
    USART1->CR2 &= ~USART_CR2_STOP;
    USART1->CR3 |= USART_CR3_DMAT;
    
    GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos);
}

void DMA2_Stream0_MEM2MEM_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    
    DMA2_Stream0->CR = DMA_SxCR_DIR_1 | DMA_SxCR_PINC | DMA_SxCR_MINC;
    DMA2_Stream0->NDTR = BUF_SIZE;
    DMA2_Stream0->PAR = (uint32_t)bufferOUT;
    DMA2_Stream0->M0AR = (uint32_t)bufferIN;
    
    DMA2_Stream0->CR |= DMA_SxCR_EN;
    while (!(DMA2->LISR & DMA_LISR_TCIF0));
    DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
}

void DMA2_Stream7_USART1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    
    DMA2_Stream7->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PL | DMA_SxCR_TCIE;
    DMA2_Stream7->NDTR = BUF_SIZE;
    DMA2_Stream7->PAR = (uint32_t)&USART1->DR;
    DMA2_Stream7->M0AR = (uint32_t)bufferIN;
    
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

void DMA2_Stream7_IRQHandler(void) {
    if (DMA2->HISR & DMA_HISR_TCIF7) {
        DMA2->HIFCR |= DMA_HIFCR_CTCIF7;
        transfer_complete = 1;
    }
}
