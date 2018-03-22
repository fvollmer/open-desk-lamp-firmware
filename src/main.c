#include "ets_sys.h"
#include "gpio.h"
#include "os_type.h"
#include "osapi.h"
#include "pwm.h"
#include "user_interface.h"

#include "encoder.h"
#include "leds.h"
#include "utils.h"

#define DEBUG 0

static enum state { INIT, OFF, ON, BUTTON_DOWN, CHANGE_COLOR } state = INIT;

static os_timer_t main_fun_timer;
static os_timer_t encoder_timer;

static void ICACHE_FLASH_ATTR main_fun(void *arg) {
  (void)arg;
  int button_state = GPIO_INPUT_GET(PIN_ENCODER_SW);
  static int last_button_state;

  // very basic state machine
  switch (state) {
  case INIT:
    debug_print("State: INIT\r\n");
    // start with maximum brightness, neutral color
    led_brightness = LED_BRIGHTNESS_MAX;
    led_color = LED_COLOR_MAX / 2;
    state = OFF;
    break;
  case OFF:
    debug_print("State: OFF\r\n");
    if (button_state && !last_button_state) {
      // go into on state if button is released
      leds_on();
      state = ON;
    } else {
      // ignore rotary encoder
      encoder_reset_delta();
    }
    break;
  case ON:
    debug_print("State: ON\r\n");
    if (!button_state && last_button_state) {
      state = BUTTON_DOWN;
    } else {
      // change brightness
      int delta = encoder_get_delta();
      if (delta != 0) {
        leds_update_brightness(delta);
        encoder_reset_delta();
      }
    }
    break;
  case BUTTON_DOWN:
    debug_print("State: BUTTON_DOWN\r\n");
    if (encoder_get_delta()) {
      state = CHANGE_COLOR;
    } else if (button_state && !last_button_state) {
      leds_off();
      state = OFF;
    } else {
      // do nothing
    }
    break;
  case CHANGE_COLOR:
    debug_print("State: CHANGE_COLOR\r\n");
    if (button_state && !last_button_state) {
      // go back into on state if button is released
      state = ON;
    } else {
      // change color temperature
      int delta = encoder_get_delta();
      if (delta != 0) {
        leds_update_color(delta);
        encoder_reset_delta();
      }
    }
    break;
  default:
    // impossible state
    break;
  }

  debug_print("Brightness: %u, Color: %u, encoder_delta: %d\r\n",
              led_brightness, led_color, encoder_get_delta());

  last_button_state = button_state;
}

static void ICACHE_FLASH_ATTR encoder_fun(void *arg) {
  (void)arg;
  encoder_process_input();
}

void ICACHE_FLASH_ATTR user_init() {
  // setup gpios
  gpio_init();
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
  gpio_output_set(0, 0, 0,
                  (1 << PIN_ENCODER_SW) | (1 << PIN_ENCODER_A) |
                      (1 << PIN_ENCODER_B));

  // setup pwm
  uint32 io_info[][3] = {
      {PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, PIN_LED1},
      {PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5, PIN_LED2},
  };
  uint32_t duty_init[] = {0, 0};
  pwm_init(LED_PWM_Period, duty_init, COUNT_OF(io_info), io_info);

  encoder_init();

  // setup timer
  os_timer_setfn(&main_fun_timer, main_fun, NULL);
  os_timer_arm(&main_fun_timer, 100, 1); // 100 ms, repeating

  os_timer_setfn(&encoder_timer, encoder_fun, NULL);
  os_timer_arm(&encoder_timer, 10, 1); // 10 ms, repeating

  // setup uart (baudrate 115200)
  uart_div_modify(0, UART_CLK_FREQ / 115200);
}
