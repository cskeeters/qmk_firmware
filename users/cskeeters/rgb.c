#include "rgb.h"

#include QMK_KEYBOARD_H

#define LED_INDEX(KEY) \
        g_led_config.matrix_co[LED_LAYOUT_65_ansi_blocker_##KEY##_ROW] \
                              [LED_LAYOUT_65_ansi_blocker_##KEY##_COL]

#define VIM_COLOR 0x30, 0xFF, 0x30
#define GUI_COLOR 0x30, 0xFF, 0x30

#define FNK_COLOR 0xFF, 0xFF, 0x60 // Function
#define LAY_COLOR 0x30, 0x30, 0xFF // Layer
#define SET_COLOR 0x30, 0xFF, 0x00 // Setting
#define CMD_COLOR 0xFF, 0x60, 0xFF // Command
#define BTL_COLOR 0xFF, 0x00, 0x00 // Trigger Bootloader
#define NOR_COLOR 0x60, 0x60, 0x60 // Normal
#define DEL_COLOR 0xFF, 0x60, 0x60 // Delete

#define NUM_COLOR 0x80, 0x80, 0x80
#define DOT_COLOR 0xFF, 0xFF, 0xFF
#define ADD_COLOR 0x20, 0xA0, 0x20
#define SUB_COLOR 0xA0, 0x20, 0x20
#define MUL_COLOR 0x00, 0xFF, 0x00
#define DIV_COLOR 0xFF, 0x00, 0x00
#define AUX_COLOR 0x00, 0x00, 0xFF

void rgb_default(void) {
    rgb_matrix_set_color_all(0x60, 0x60, 0x60);

    if (default_layer_state & TO_STATE(VIM_LAYER)) {
        rgb_matrix_set_color(LED_INDEX(CAP),  VIM_COLOR);
    }

    if (default_layer_state & TO_STATE(MAC_LAYER)) {
        rgb_matrix_set_color(LED_INDEX(LALT), GUI_COLOR);
    } else {
        rgb_matrix_set_color(LED_INDEX(LGUI), GUI_COLOR);
    }
}

void rgb_control(void) {
    //Turn off all
    rgb_matrix_set_color_all(0, 0, 0);

    // Row 1
    rgb_matrix_set_color(LED_INDEX(1),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(2),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(3),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(4),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(5),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(6),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(7),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(8),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(9),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(0),    FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(MI),   FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(EQ),   FNK_COLOR);
    rgb_matrix_set_color(LED_INDEX(BSP),  DEL_COLOR);

    // Row 2
    rgb_matrix_set_color(LED_INDEX(Q),    SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(E),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(Y),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(U),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(I),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(O),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(LB),   SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(RB),   SET_COLOR);

    // Row 3
    rgb_matrix_set_color(LED_INDEX(CAP),  SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(S),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(D),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(J),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(K),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(ENT),  LAY_COLOR);

    // Row 4
    rgb_matrix_set_color(LED_INDEX(LSH),  LAY_COLOR); // not technically a layer
    rgb_matrix_set_color(LED_INDEX(V),    LAY_COLOR);
    rgb_matrix_set_color(LED_INDEX(N),    LAY_COLOR);
    rgb_matrix_set_color(LED_INDEX(M),    CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(RSH),  CMD_COLOR);

    // Row 5
    rgb_matrix_set_color(LED_INDEX(LCTL), CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(LGUI), LAY_COLOR);
    rgb_matrix_set_color(LED_INDEX(LALT), LAY_COLOR);
    rgb_matrix_set_color(LED_INDEX(SP),   CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(RALT), BTL_COLOR);

    // Arrows
    rgb_matrix_set_color(LED_INDEX(AUP),  SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(AL),   SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(ADN),  SET_COLOR);
    rgb_matrix_set_color(LED_INDEX(AR),   SET_COLOR);

    // Special
    rgb_matrix_set_color(LED_INDEX(HOM),  CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(END),  CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(PUP),  CMD_COLOR);
    rgb_matrix_set_color(LED_INDEX(PDN),  CMD_COLOR);
}

void rgb_numpad(void) {
    rgb_matrix_set_color_all(0x00, 0x00, 0x00);

    rgb_matrix_set_color(LED_INDEX(U),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(I),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(O),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(J),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(J),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(K),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(L),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(M),    NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(CM),   NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(DT),   NUM_COLOR);
    rgb_matrix_set_color(LED_INDEX(SP),   NUM_COLOR);

    rgb_matrix_set_color(LED_INDEX(RALT), DOT_COLOR);

    rgb_matrix_set_color(LED_INDEX(P),    ADD_COLOR);
    rgb_matrix_set_color(LED_INDEX(SC),   ADD_COLOR);

    rgb_matrix_set_color(LED_INDEX(0),    SUB_COLOR);
    rgb_matrix_set_color(LED_INDEX(MI),   SUB_COLOR);

    rgb_matrix_set_color(LED_INDEX(9),    MUL_COLOR);

    rgb_matrix_set_color(LED_INDEX(8),    DIV_COLOR);

    rgb_matrix_set_color(LED_INDEX(TAB),  AUX_COLOR);
    rgb_matrix_set_color(LED_INDEX(CAP),  AUX_COLOR);

    rgb_matrix_set_color(LED_INDEX(ENT),  AUX_COLOR);

    rgb_matrix_set_color(LED_INDEX(SL),   AUX_COLOR);
    rgb_matrix_set_color(LED_INDEX(RCTL), AUX_COLOR);
}

bool rgb_mode_custom(effect_params_t* params)
{
    // LEDs are not animated, so we only need to do this once
    if (!params->init) {
        if (layer_state & TO_STATE(NUMPAD_LAYER)) {
            rgb_numpad();
        } else {
            if (layer_state & TO_STATE(CONTROL_LAYER)) {
                rgb_control();
            } else {
                rgb_default();
            }
        }
    }

    return false; // return true if still rendering
}
