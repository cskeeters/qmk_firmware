#pragma once

// 1 for each bit (layer), usefull for building a mask
#define ALL_LAYERS_STATE ~((layer_state_t)0)

// Converts layer number to later_state_t (Mask)
#define TO_STATE(layer) (layer_state_t)1 << layer

// returns layer_state_t with all 1s except the bit for the provided layer
#define ALL_BUT_STATE(layer) ALL_LAYERS_STATE ^ TO_STATE(layer)

// Layers at the bottom will override layers at the top regardless of if
// they are set in layer_state or default_layer_state
enum layer_names
{
    BASE_LAYER,
    MAC_LAYER,
    VIM_LAYER,
    NUMPAD_LAYER,
    EOE_LAYER, // (Keeps me from getting confused after I forget to press Escape)

    // Keep CONTROL_LAYER last so that it takes priority over NUMPAD
    CONTROL_LAYER,
};

// Output layer information to console
void dprint_layer_info(int layer, const char *name);

// Output all layer information to console
void dprint_layers(void);
