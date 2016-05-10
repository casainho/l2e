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
  GPIO_SetBits (DIR_PORT, DIR_PIN);

  // keep stepping while endstop if off
  while (GPIO_ReadInputDataBit (ENDSTOP_PORT, ENDSTOP_PIN))
  {
    // do the step
    GPIO_SetBits (STEP_PORT, STEP_PIN);
    delay_ms (0);
    GPIO_ResetBits (STEP_PORT, STEP_PIN);
    delay_ms (2);
  }

#define ZERO_POINT_STEPS 2
  unsigned int steps = 0;

  // go to zero point, by moving ZERO_POINT_STEPS
  while (steps <= ZERO_POINT_STEPS)
  {
    // do the step
    GPIO_SetBits (STEP_PORT, STEP_PIN);
    delay_ms (2);
    GPIO_ResetBits (STEP_PORT, STEP_PIN);
    delay_ms (2);

    steps++;
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
  float potentiomer_value = 0;
  static float value = 0;

  potentiomer_value = (float) (adc_get_potentiometer_value () * 1.7);

  // IIR filter
  value = value + (0.005 * (potentiomer_value - value));

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
    if (delta_position < 175) continue;

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
