/*
 * l2e
 *
 * Copyright (C) Jorge Pinto aka Casainho, 2016.
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "core_cm3.h"
#include "gpio.h"
#include "adc.h"

volatile unsigned int _ms10;

void delay_ms10 (unsigned int ms10)
{
  _ms10 = 1;
  while (ms10 >= _ms10) ;
}

void SysTick_Handler(void) // runs every 10ms
{

}

void initialize (void)
{
  gpio_init ();
  adc_init ();

  /* Setup SysTick Timer for 10 millisecond interrupts, also enables Systick and Systick-Interrupt */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }
}

int main (void)
{
  initialize ();

   while (1)
  {

  }

  // should never arrive here
  return 0;
}
