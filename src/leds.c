#include "osapi.h"
#include "pwm.h"

#include "leds.h"
#include "utils.h"

unsigned int led_brightness;
unsigned int led_color;

static const int led_big_lut[] ICACHE_RODATA_ATTR = {
    9,   9,   10,  10,  11,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,
    16,  16,  17,  17,  18,  18,  19,  20,  20,  21,  21,  22,  22,  23,  24,
    24,  25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,  33,  33,  34,
    35,  36,  36,  37,  38,  39,  39,  40,  41,  42,  43,  44,  44,  45,  46,
    47,  48,  49,  50,  51,  52,  53,  54,  54,  55,  56,  57,  58,  59,  60,
    61,  62,  63,  64,  65,  66,  68,  69,  70,  71,  72,  73,  74,  75,  76,
    77,  78,  80,  81,  82,  83,  84,  85,  87,  88,  89,  90,  92,  93,  94,
    95,  97,  98,  99,  100, 102, 103, 104, 106, 107, 108, 110, 111, 112, 114,
    115, 116, 118, 119, 121, 122, 124, 125, 126, 128, 129, 131, 132, 134, 135,
    137, 138, 140, 141, 143, 144, 146, 147, 149, 151, 152, 154, 155, 157, 159,
    160, 162, 164, 165, 167, 169, 170, 172, 174, 175, 177, 179, 180, 182, 184,
    186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 204, 205, 207, 209, 211,
    213, 215, 217, 219, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240,
    242, 244, 246, 248, 250, 252, 254, 256, 258, 260, 262, 265, 267, 269, 271,
    273, 275, 277, 279, 282, 284, 286, 288, 290, 292, 295, 297, 299, 301, 304,
    306, 308, 310, 313, 315, 317, 320, 322, 324, 326, 329, 331, 333, 336, 338,
    341, 343, 345, 348, 350, 353, 355, 357, 360, 362, 365, 367, 370, 372, 375,
    377, 380, 382, 385, 387, 390, 392, 395, 397, 400};

void ICACHE_FLASH_ATTR leds_off(void) {
  pwm_set_duty(0, 0);
  pwm_set_duty(0, 1);
  pwm_start();
}

void ICACHE_FLASH_ATTR leds_on(void) {
  int brightness = COUNT_OF(led_big_lut) - 1;

  // set brighness level
  brightness = (brightness * led_brightness) / LED_BRIGHTNESS_MAX;

  int brightness_0;
  int brightness_1;

  // power partitioning to LEDs for color change (warm or cold)
  if (BRIGHTNESS_BOOST) {
    // maximize power to LEDs
    // not all color modes have the same brightness
    unsigned int led_color_0 = led_color;
    unsigned int led_color_1 = LED_COLOR_MAX - led_color;
    brightness_0 = brightness * led_color_0;
    brightness_1 = brightness * led_color_1;

    // normalization factor
    int n = led_color_0 > led_color_1 ? led_color_0 : led_color_1;

    // limit total power to 80% of the maximum (power supply limit)
    int p = LED_COLOR_MAX * 100 / n / 2;
    if (p > 80) {
      n = n * p / 80;
    }

    // normalize
    brightness_0 /= n;
    brightness_1 /= n;
  } else {
    // all color modes have the same total brightness
    // e.g. 50% + 50% = 100% or 70% + 30% = 100%
    brightness_0 = (brightness * led_color) / LED_COLOR_MAX;
    brightness_1 = brightness - brightness_0;
  }

  // set pwm according to lookup table
  int max_lut_val = led_big_lut[COUNT_OF(led_big_lut) - 1];
  uint32_t pwm0 = (LED_PWM_DUTY_MAX * led_big_lut[brightness_0]) / max_lut_val;
  uint32_t pwm1 = (LED_PWM_DUTY_MAX * led_big_lut[brightness_1]) / max_lut_val;
  pwm_set_duty(pwm0, 0);
  pwm_set_duty(pwm1, 1);

  pwm_start();
}

void ICACHE_FLASH_ATTR leds_update_brightness(int delta) {
  if (delta < 0 && led_brightness < (unsigned int)-delta + LED_BRIGHTNESS_MIN) {
    led_brightness = LED_BRIGHTNESS_MIN;
  } else {
    led_brightness += delta;
    if (led_brightness > LED_BRIGHTNESS_MAX) {
      led_brightness = LED_BRIGHTNESS_MAX;
    }
  }
  leds_on();
}

void ICACHE_FLASH_ATTR leds_update_color(int delta) {
  if (delta < 0 && led_color < (unsigned int)-delta) {
    led_color = 0;
  } else {
    led_color += delta;
    if (led_color > LED_COLOR_MAX) {
      led_color = LED_COLOR_MAX;
    }
  }
  leds_on();
}
