#include "layer.h"
#include "quantum.h"

void dprint_layer_info(int layer, const char *name)
{
    dprintf("%30s: ", name);
    dprintf("%2d (0x%02x)", layer, TO_STATE(layer));

    dprintf("[");
    if (default_layer_state & TO_STATE(layer)) {
        dprintf("D");
    } else {
        dprintf(" ");
    }
    if (layer_state & TO_STATE(layer)) {
        dprintf("L");
    } else {
        dprintf(" ");
    }
    dprintf("]\n");
}

#define DPRINT_LAYER(layer) dprint_layer_info(layer, #layer)

void dprint_layers(void)
{
    DPRINT_LAYER(BASE_LAYER);
    DPRINT_LAYER(MAC_LAYER);
    DPRINT_LAYER(VIM_LAYER);
    DPRINT_LAYER(NUMPAD_LAYER);
    DPRINT_LAYER(EOE_LAYER);
    DPRINT_LAYER(CONTROL_LAYER);
}

