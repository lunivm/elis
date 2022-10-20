/*
 * rgb_led.c
 *
 *  Created on: Oct 20, 2022
 *      Author: mykolaluniv
 */

#include <stdbool.h>

#include "driver/ledc.h"
#include "rgb_led.h"

/**
 * Initialize RGB LED settings for each channel
 */
static void rgb_led_pwn_init(void)
{
	int rgb_ch;

	// Red
	ledc_ch[0].channel		= LEDC_CHANNEL_0;
	ledc_ch[0].gpio   		= RGB_LED_RED_GPIO;
	ledc_ch[0].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[0].channel		= LEDC_TIMER_0;

	// Green
	ledc_ch[1].channel		= LEDC_CHANNEL_1;
	ledc_ch[1].gpio   		= RGB_LED_GREEN_GPIO;
	ledc_ch[1].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].channel		= LEDC_TIMER_0;

	// Blue
	ledc_ch[1].channel		= LEDC_CHANNEL_1;
	ledc_ch[1].gpio   		= RGB_LED_BLUE_GPIO;
	ledc_ch[1].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].channel		= LEDC_TIMER_0;
}
