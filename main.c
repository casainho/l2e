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
static unsigned int direction = 0;

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
  // verify endstop and don't step if we hit the endstop and we are trying to go on the wrong direction
  if ((direction == 1) && (!GPIO_ReadInputDataBit (ENDSTOP_PORT, ENDSTOP_PIN))) return;

  // do the step
  GPIO_SetBits (STEP_PORT, STEP_PIN);
  delay_ms (0);
  GPIO_ResetBits (STEP_PORT, STEP_PIN);
  delay_ms (2);
}

void set_direction (unsigned int dir)
{
  if (dir == 0)
  {
    GPIO_ResetBits (DIR_PORT, DIR_PIN);
    direction = 0;
  }
  else
  {
    GPIO_SetBits (DIR_PORT, DIR_PIN);
    direction = 1;
  }

  delay_ms (2);
}

void calibration (void)
{
  // set first the direction
  GPIO_ResetBits (DIR_PORT, DIR_PIN);

  // keep stepping while endstop if off
  while (GPIO_ReadInputDataBit (ENDSTOP_PORT, ENDSTOP_PIN))
  {
    // do the step
    GPIO_SetBits (STEP_PORT, STEP_PIN);
    delay_ms (2);
    GPIO_ResetBits (STEP_PORT, STEP_PIN);
    delay_ms (2);
  }
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

// returns [0 <-> 1000]
float get_potentiometer_value (void)
{
  static float x0 = 0, x1 = 0, x2 = 0, x3 = 0, x4 = 0;
  float value = 0;

  x0 = adc_get_potentiometer_value () / 4.097;
  value = x0*0.2 + x1*0.2 + x2*0.2 + x3*0.2 + x4*0.2;
  x4 = x3;
  x3 = x2;
  x2 = x1;
  x1 = x0;

  return value;
}

int main (void)
{
  static int position = 0;
  static int target_position = 0;
  static int delta_position = 0;

  // initialize the system
  initialize ();

  // initial calibration of the motor position
  calibration ();

  while (1)
  {

home:
    // calc the new target position based on potentiometer value
    target_position = (int) get_potentiometer_value ();
    delta_position = target_position - position;

    // define the motor direction
    if (delta_position > 0)
    {
      set_direction (1);
    }
    else
    {
      set_direction (0);
      delta_position *= -1;
    }

    // filtering
    if (delta_position < 50)
    {
      delta_position = 0;
      goto home;
    }

    // keep stepping until get on the target position
    while (delta_position != 0)
    {
      // do the step
      step ();
      delta_position--;
    }

    position = target_position;
  }
}
