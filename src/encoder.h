#pragma once

#define PIN_ENCODER_SW 2
#define PIN_ENCODER_A 12
#define PIN_ENCODER_B 13

int encoder_get_delta(void);

void encoder_reset_delta(void);

void encoder_init(void);

void encoder_process_input(void);
