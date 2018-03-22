#include "ets_sys.h"
#include "gpio.h"

#include "encoder.h"

static int encoder_delta;

int ICACHE_FLASH_ATTR encoder_get_delta(void) {
  // encoder latching points seem to have half the total resolution
  int ret = encoder_delta / 2;
  return ret;
}

void ICACHE_FLASH_ATTR encoder_reset_delta(void) {
  // TODO this might not be safe
  encoder_delta -= encoder_delta / 2 * 2;
}

void ICACHE_FLASH_ATTR encoder_init(void) {
  encoder_process_input();
  encoder_delta = 0;
}

void ICACHE_FLASH_ATTR encoder_process_input(void) {
  int enc_state = 0;
  static int last_enc_state = 0;

  if (GPIO_INPUT_GET(PIN_ENCODER_A)) {
    enc_state |= 0b11;
  }
  if (GPIO_INPUT_GET(PIN_ENCODER_B)) {
    enc_state ^= 0b01;
  }

  int enc_state_difference = last_enc_state - enc_state;
  if (enc_state_difference & 0b01) {
    last_enc_state = enc_state;
    if (enc_state_difference & 0b10) {
      encoder_delta++;
    } else {
      encoder_delta--;
    }
  }
}
