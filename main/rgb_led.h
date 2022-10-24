#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

// RGB LED GPIOs
#define RGB_LED_RED_GPIO     26
#define RGB_LED_GREEN_GPIO   25
#define RGB_LED_BLUE_GPIO    17

// RGB LED color mix channels
#define RGB_LED_CHANNEL_NUM  3

// RGB LED configuration
typedef struct
{
	int channel;
	int gpio;
	int mode;
	int timer_index;
} ledc_info_t;
ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

/**
 * Message IDs for the WiFi application task
 */
typedef enum rgb_status_message
{
	RGB_STATUS_MSG_NONE = 0,
	RGB_STATUS_MSG_START_APP,
	RGB_STATUS_MSG_START_WIFI_APP,
	RGB_STATUS_MSG_START_HTTP_SERVER,
} rgb_status_message_e;

// Indicate main app start
void rgb_send_status_message(rgb_status_message_e message);

#endif /* MAIN_RGB_LED_H_ */
