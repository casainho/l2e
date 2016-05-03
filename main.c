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

static int _ms = 0;

unsigned int value;

void delay_ms (int ms)
{
  _ms = 1;
  while (ms >= _ms) ;
}

void SysTick_Handler(void) // runs every 1ms
{
  // for delay_ms ()
  _ms++;
}

void step (void)
{
  GPIO_SetBits (STEP_PORT, STEP_PIN);
  delay_ms (value);
  GPIO_ResetBits (STEP_PORT, STEP_PIN);
  delay_ms (2);
}

void direction (unsigned int dir)
{
  if (dir == 0) GPIO_ResetBits (DIR_PORT, DIR_PIN);
  else GPIO_SetBits (DIR_PORT, DIR_PIN);

  delay_ms (2);
}

void initialize (void)
{
  gpio_init ();
  adc_init ();

  /* Setup SysTick Timer for 1ms, also enables Systick and Systick-Interrupt */
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */
    while (1);
  }
}

int main (void)
{
  //static unsigned int value;

  initialize ();

  direction (1);

  while (1)
  {

      value = (adc_get_potentiometer_value () >> 3);

    step ();
  }

  // should never arrive here
  return 0;
}
