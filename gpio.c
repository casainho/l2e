/*
 * l2e
 *
 * Copyright (C) Jorge Pinto aka Casainho, 2016.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/* Connetions:
 *
 * PA0  (ADC1_IN1)      -- potentiometer
 * PA1  (ADC1_IN2)      -- magnetic sensor
 * PB5  (GPIO)          -- LED debug
 */

void gpio_init (void)
{
  /* Enable GPIOB clock. */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  |
                          RCC_APB2Periph_GPIOA |
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_ADC1
                          , ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC
                          , ENABLE);

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
     connected to the DAC converter. In order to avoid parasitic consumption,
     the GPIO pin should be configured in analog */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Turn off port bits */
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}
