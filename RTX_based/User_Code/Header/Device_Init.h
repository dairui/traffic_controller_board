#ifndef __DEVICEINIT_H
#define __DEVICEINTI_H

void DeviceInit(void);
void RCC_configuration(void);
void SRAM_Init(void);
void GPIO_configuration(void);
void Exti_Configuration(void);
void USART_configuration(void);
void NVIC_configuration(void);
void SPI3_configuration(void);
void TIM2_configuration(u8 second);
void TIM3_configuration(u8 second);
void TIM4_configuration(u8 second);
void TIM5_configuration(u8 second);
void GPIO_PinReverse(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#endif

