/*
 * l2e
 *
 * Copyright (C) Jorge Pinto aka Casainho, 2016.
 *
 * Released under the GPL License, Version 3
 */


/* Connetions:
 *
 * PA0  (ADC1_IN1)      -- potentiometer
 * PA1  (ADC1_IN2)      -- magnetic sensor
 * PB10 (GPIO)          -- DIR pin
 * PB13 (GPIO)          -- STEP pin
 */

#define DIR_PIN			GPIO_Pin_10
#define DIR_PORT		GPIOB
#define STEP_PIN		GPIO_Pin_13
#define STEP_PORT		GPIOB
#define POTENTIMETER_PIN	GPIO_Pin_0
#define POTENTIMETER_PORT	GPIOA

void gpio_init (void);
