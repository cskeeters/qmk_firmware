/* Copyright 2024 Chad Skeeters
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include "action.h"
#include "action_layer.h"
#include "keycodes.h"
#include "layer.h"
#include "quantum_keycodes.h"

#ifdef RGB_MATRIX_ENABLE
#include "rgb.h"
#endif

#ifdef REGISTER_ENABLE
#include "register.h"
#endif

#include QMK_KEYBOARD_H

// Layer mappings are defined in these header files
#include "base.h"
#include "mac.h"
#include "vim.h"
#include "control.h"
#include "number_pad.h"
#include "eoe.h"

enum custom_keycodes
{
  // https://github.com/qmk/qmk_firmware/blob/develop/data/constants/keycodes/keycodes_0.0.2_basic.hjson
  DEBUG_DATA = SAFE_RANGE, // Show layers and their states

  // Can't be used because we need to toggle this in default_layer_state, not layer_state as TG does
  ENABLE_MAC,
  DISABLE_MAC,
  TOGGLE_VIM,

  // Don't use TG so that we don't accidently turn it off
  ENABLE_NUMPAD,

  // This toggles two layers on
  NUMPAD_TIL_ENTER,

  // This toggles off both NUMPAD_LAYER and EOE_LAYER
  EXIT_NUMPAD,
  EXIT_NUMPAD_ENTER,

  // Convenient Macros
  LAUNCH_YOUTUBE,
  LAUNCH_YOUTUBE_MUSIC
};

void keyboard_post_init_user(void)
{
    default_layer_set(TO_STATE(BASE_LAYER) |
                      TO_STATE(MAC_LAYER) |
                      TO_STATE(VIM_LAYER));

#ifdef REGISTER_ENABLE
    register_init(KC_P);
#endif

#ifdef RGB_MATRIX_ENABLE
    //Register the callback that will be called on render
    rgb_mode_callback = rgb_mode_custom;
    rgb_matrix_mode_noeeprom(RGB_MATRIX_CALLBACK);
#endif
}

// Save space when not using magic keycodes (Saves 576 bytes)
uint16_t keycode_config(uint16_t keycode) {
    return keycode;
}

// Save space when not using magic keycodes
uint8_t mod_config(uint8_t mod) {
    return mod;
}

// keycode can either be:
//  * ENUMS from custom_keycodes
//  * Keys like KC_ESCAPE (quantum/keycodes.h)
//  * Macros like LT(CONTROL_LAYER, KC_GRAVE) - which would equal 0x4535 (quantum/quantum_keycodes.h)
//
// NOTE: further processing of this key is skipped when this function returns false

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{

#ifdef REGISTER_ENABLE
    if (register_handle_event(keycode, record)) {
        return false;
    }
#endif

    switch (keycode) {
        case DEBUG_DATA:
            if (!record->event.pressed) {
                dprint_layers();
            }
            return false;

        case DISABLE_MAC:
            if (!record->event.pressed) {
                dprintf("Turning off MAC_LAYER in default_layer_state\n");
                default_layer_and(ALL_BUT_STATE(MAC_LAYER));
                dprint_layers();
            }
            return false;

        case ENABLE_MAC:
            if (!record->event.pressed) {
                dprintf("Turning on MAC_LAYER in default_layer_state\n");
                default_layer_or(TO_STATE(MAC_LAYER));
                dprint_layers();
            }
            return false;

        case TOGGLE_VIM:
            if (!record->event.pressed) {
                // default_layer_state is global from action_layer.c
                if (default_layer_state & TO_STATE(VIM_LAYER)) {
                    dprintf("Turning off VIM_LAYER in default_layer_state\n");
                    default_layer_and(ALL_BUT_STATE(VIM_LAYER));
                } else {
                    dprintf("Turning on VIM_LAYER in default_layer_state\n");
                    default_layer_or(TO_STATE(VIM_LAYER));
                }
                dprint_layers();
            }
            return false;
        case ENABLE_NUMPAD:
            if (!record->event.pressed) {
                dprintf("Turning on NUMPAD_LAYER in layer_state\n");
                layer_on(NUMPAD_LAYER);
                dprint_layers();
            }
            return false;

        case NUMPAD_TIL_ENTER:
            if (!record->event.pressed) {
                dprintf("Turning on NUMPAD_LAYER in layer_state\n");
                layer_on(NUMPAD_LAYER);
                dprintf("Turning on EOE_LAYER in layer_state\n");
                layer_on(EOE_LAYER);
                dprint_layers();
            }
            return false;

        case EXIT_NUMPAD:
            if (!record->event.pressed) {
                dprintf("Clearing layer_state\n");
                layer_clear();
                dprint_layers();
            }
            return false;

        case EXIT_NUMPAD_ENTER: //exited via enter key
            if (!record->event.pressed) {
                dprintf("Clearing layer_state\n");
                layer_clear();
                SEND_STRING(SS_TAP(X_ENTER));
                dprint_layers();
            }
            return true; // Process enter key

        case LAUNCH_YOUTUBE:
            // Currently, this is mac only
            if (!record->event.pressed) {
                // work only on release
                SEND_STRING(SS_LGUI(" ")); // Activate Alfred
                SEND_STRING(SS_DELAY(100)); // Wait for alfred to load
                SEND_STRING("http://youtube.com" SS_TAP(X_ENTER)); // Open chrome
                layer_off(CONTROL_LAYER);
            }
            return false;

        case LAUNCH_YOUTUBE_MUSIC:
            // Currently, this is mac only
            if (!record->event.pressed) {
                // work only on release
                SEND_STRING(SS_LGUI(" ")); // Activate Alfred
                SEND_STRING(SS_DELAY(100)); // Wait for alfred to load
                SEND_STRING("YouTube Music" SS_TAP(X_ENTER)); // Open YouTube Music Safari App
                layer_off(CONTROL_LAYER);
            }
            return false;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE_LAYER]    = MATRIX_LAYOUT_65_ansi_blocker(BASE),
    [MAC_LAYER]     = MATRIX_LAYOUT_65_ansi_blocker(MAC),
    [VIM_LAYER]     = MATRIX_LAYOUT_65_ansi_blocker(VIM),
    [CONTROL_LAYER] = MATRIX_LAYOUT_65_ansi_blocker(CONTROL),
    [NUMPAD_LAYER]  = MATRIX_LAYOUT_65_ansi_blocker(NUMPAD),
    [EOE_LAYER]     = MATRIX_LAYOUT_65_ansi_blocker(EOE),
};
