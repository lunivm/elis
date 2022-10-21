/*
 * rgb_led.c
 *
 *  Created on: Oct 20, 2022
 *      Author: mykolaluniv
 */

#include <stdbool.h>

#include "driver/ledc.h"
#include "rgb_led.h"

// flag to indicate g_pwm_init_handle call
bool g_pwm_init_handle = false;

/**
 * Initialize RGB LED settings for each channel
 */
static void rgb_led_pwm_init(void)
{
	int rgb_ch;

	// Red
	ledc_ch[0].channel		= LEDC_CHANNEL_0;
	ledc_ch[0].gpio   		= RGB_LED_RED_GPIO;
	ledc_ch[0].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[0].timer_index		= LEDC_TIMER_0;

	// Green
	ledc_ch[1].channel		= LEDC_CHANNEL_1;
	ledc_ch[1].gpio   		= RGB_LED_GREEN_GPIO;
	ledc_ch[1].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].timer_index		= LEDC_TIMER_0;

	// Blue
	ledc_ch[2].channel		= LEDC_CHANNEL_2;
	ledc_ch[2].gpio   		= RGB_LED_BLUE_GPIO;
	ledc_ch[2].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[2].timer_index		= LEDC_TIMER_0;

	ledc_timer_config_t ledc_timer_conf =
	{
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_8_BIT,
			.timer_num = LEDC_TIMER_0,
			.freq_hz = 100
	};
	ledc_timer_config(&ledc_timer_conf);

	// Configure channels
	for (rgb_ch = 0; rgb_ch < RGB_LED_CHANNEL_NUM; rgb_ch++)
	{
		ledc_channel_config_t ledc_channel =
		{
			.channel	= ledc_ch[rgb_ch].channel,
			.duty		= 0,
			.hpoint		= 0,
			.gpio_num	= ledc_ch[rgb_ch].gpio,
			.intr_type	= LEDC_INTR_DISABLE,
			.speed_mode = ledc_ch[rgb_ch].mode,
			.timer_sel	= ledc_ch[rgb_ch].timer_index,
		};
		ledc_channel_config(&ledc_channel);
	}

	g_pwm_init_handle = true;
}

static void safe_rgb_led_pwm_init(void) {
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}
}

/**
 * Sets the RGB color.
 */
static void rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	// reduce led brightness
	red = red / 100;
	green = green / 100;
	blue = blue / 100;
	
	safe_rgb_led_pwm_init();

	// Value should be 0 - 255 for 8 bit number
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}

// app
void rgb_main_app_start(void) {
	rgb_led_set_color(240, 136, 43);
}

// wifi
void rgb_led_wifi_app_start(void)
{
	rgb_led_set_color(240, 227, 43);
}

void rgb_led_wifi_connected(void)
{
	rgb_led_set_color(0, 255, 0);
}

void rgb_led_wifi_ap_child_connected(void)
{
	rgb_led_set_color(250, 0, 0);
}

// http
void rgb_led_http_server_started(void)
{
	rgb_led_set_color(0, 0, 250);
}

// none
void rgb_led_none(void)
{
	safe_rgb_led_pwm_init();
	rgb_led_set_color(0, 0, 0);
}
