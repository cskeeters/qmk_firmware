#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "layer.h"
#include "raw_hid.h"

#include QMK_KEYBOARD_H

struct Register
{
    uint16_t keycode;
    uint8_t *data;
    struct Register *next;
};

typedef struct Register Register;

void register_init(char next_keycode);
bool register_handle_event(uint16_t keycode, keyrecord_t *record);

Register *get_register(uint16_t keycode);
void send_raw_hid_response(char *msg, uint8_t length);
void raw_hid_receive(uint8_t *data, uint8_t length);
