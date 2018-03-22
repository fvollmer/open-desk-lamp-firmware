#pragma once

#define BRIGHTNESS_BOOST 1

#define PIN_LED1 4
#define PIN_LED2 5

#define LED_BRIGHTNESS_MIN 1
#define LED_BRIGHTNESS_MAX 10

#define LED_COLOR_MAX 20

#define LED_PWM_Period 1000
#define LED_PWM_DUTY_MAX (LED_PWM_Period * 1000 / 45)

extern unsigned int led_brightness;
extern unsigned int led_color;

void leds_off(void);
void leds_on(void);

void leds_update_brightness(int delta);
void leds_update_color(int delta);
