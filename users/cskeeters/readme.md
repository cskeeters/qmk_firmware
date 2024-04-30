Chad's Custom Keymap

# Utilizing Layer States

In QMK, each layer is given a number as it is added to the enum.

```c
enum layer_names
{
    BASE_LAYER, // 0 (state 0x01 or 0b0001)
    MAC_LAYER,  // 1 (state 0x02 or 0b0010)
    VIM_LAYER,  // 2 (state 0x04 or 0b0100)
    ...
};
```

Each layer number has an associated *state* (layer_state_t) value, that the documentation refers to as a *mask*.  The state values associate with each layer number are bit-wise mutually exclusive.  This means that a single state integer identify *multiple* active layers.  For example, a state of 5 (or 0b0101) has two bits set.  One bit indicates layer 0 is active and the other indicates that layer 2 is active.

ASIDE: I like to call it *state* since *mask* implies a bit-wise operation may be occurring under the hood when methods like [layer_state_set(layer_mask)](https://docs.qmk.fm/#/feature_layers?id=functions) do not perform any bit-wise operation.

QMK stores active layers in two integer variables, `default_layer_state` and `layer_state`.  When a key is pressed, these two variables get bit-wise or'ed and searched.  Once QMK finds an active layer with a key mapped to something other than, `KC_TRANSPARENT`, that key will be used.

## GUI Key and the Number Pad

In my configuration, I configure a **BASE** layer that complies with a standard 104-key layout for Linux/Windows.  When I'm on mac, I can toggle on a layer that swaps the `OS` and `ALT` key.

Since I use vim, I like to map `CAPSLOCK` to `ESCAPE`.  I defined the layer **VIM** for this purpose.

I also created a layer to change the keys on my right hand into a number pad.  This enables the entry of multiple numbers with tabs and returns supported.  Sometimes I just want to enter in one number and then exit the number pad, so I have another layer that maps the enter key to `EXIT_NUMPAD`.  When I exit the number pad, I need to turn of both of these layers without turning off the **MAC** or **VIM** layers *if they are enabled*.

Understanding QMK has a default layer state and (not a default layer) helps all of this work.  When **MAC** or **VIM** are turned on, they are set in `default_layer_state` instead of `layer_state` so that I can turn off all of the temporary layers but retain as set of base layers.

This could all work with a single base layer, but then I'd have to define layers for every combination base layers: **WINDOWS**, **WINDOWS_VIM**, **MAC**, **MAC_VIM**.  As I add more base layers, the number of layers needed grows exponentially.

## Manipulating the Default Layer State

QMK provides `default_layer_set(layer_mask)` to set the active layer(s) in `default_layer_state`.  In order to use this, you must pass in the *state* of the layer instead of the layer *number*.  To convert a layer *number* to a layer *state*, I define the macro `TO_STATE`.

```c++
#define TO_STATE(layer) (layer_state_t)1 << layer
```

To set a layer as the only default layer, the call would look like this:

```c++
default_layer_set(TO_STATE(BASE_LAYER));
```

Since I like multiple layers set by default, I call `default_layer_state` in `keyboard_post_init_user` and set multiple layer active with one call.

```c++
void keyboard_post_init_user(void)
{
    default_layer_set(TO_STATE(BASE_LAYER) |
                      TO_STATE(MAC_LAYER) |
                      TO_STATE(VIM_LAYER));
}
```

QMK provides `default_layer_or(layer_mask)` to add layer(s) to `default_layer_state`.  In order to use this, you must pass in the *state* of the layer instead of the layer *number*.

```c++
default_layer_or(TO_STATE(MAC_LAYER));
```

QMK provides default_layer_and(layer_mask) to remove a layer from `default_layer_state`.  The argument to this method needs to be an inversion of the layer's *state*.  To get this value, I defined a `ALL_BUT_STATE` macro.

```c++
#define ALL_LAYERS_STATE ~((layer_state_t)0) // All ones (0b11111111)
#define ALL_BUT_STATE(layer) ALL_LAYERS_STATE ^ TO_STATE(layer)
```

With these defined, you can remove a layer like this:

```c++
default_layer_and(ALL_LAYERS_STATE(MAC_LAYER));
```

The layer_state variable can be similarly manipulated via `layer_or`, `layer_and`, `layer_clear`.  The API also provides other methods like `layer_on` and `layer_off` that take numbers instead of state too.  These work, but can only add/remove one layer at a time.

## Debugging layers

The keyboard enters debug mode when QK_DEBUG_TOGGLE is issued.  If `qmk
console` is running and connects to they keyboard, and debug mode is entered,
then strings like this will be output from qmk:

    Pizzakeyboards:Slice65:1: ---- action_exec: start -----
    Pizzakeyboards:Slice65:1: EVENT: 0801u(6233)
    Pizzakeyboards:Slice65:1: Turning on MAC_LAYER in default_layer_state
    Pizzakeyboards:Slice65:1: default_layer_state: 00000005(2) to 00000007(2)
    Pizzakeyboards:Slice65:1:                     BASE_LAYER:  0 (0x01)[D ]
    Pizzakeyboards:Slice65:1:                      MAC_LAYER:  1 (0x02)[D ]
    Pizzakeyboards:Slice65:1:                      VIM_LAYER:  2 (0x04)[D ]
    Pizzakeyboards:Slice65:1:                   NUMPAD_LAYER:  3 (0x08)[  ]
    Pizzakeyboards:Slice65:1:                      EOE_LAYER:  4 (0x10)[  ]
    Pizzakeyboards:Slice65:1:                  CONTROL_LAYER:  5 (0x20)[ L]
    Pizzakeyboards:Slice65:1: processed: 0801u(6233):0

"Turning on MAC_LAYER in default_layer_state" was issued due to a `dprintf` command in method `process_record_user` inside `keymap.c`.  The (slick) iteration of the layers as well as their activity in `default_layer_state` (indicated by `[D ]`) and `layer_state` (indicated by `[ L]`) is output by the methods `dprint_layers`, `dprint_layer_info`, and the macro `DPRINT_LAYER`.


# Layer Definition and Readability

`keymap.c` needs to be written in a way such that it will be easy for users to assign [Keycodes](https://github.com/qmk/qmk_firmware/blob/master/docs/keycodes.md) to keys.  This is tricky since in QMK, the keys themselves are mapped to a particular location in the 3-dimensional array `keymaps` in a non-intuitive way.

In the *default* keymap, the base layer is defined using the `LAYOUT` definition (which works like a compile-time function).  `LAYOUT` returns a 2-dimensional array, but despite the keys on the keyboard having rows and columns, keys on the same row of the keyboard don't get assigned to locations in the same row of the returned array.  The `LAYOUT` definition handles the rearrangement that must occur so that users can specify a layout that looks like the keyboard layout of their keyboard (see keymaps/default/keymap.c and notice that the `_BASE` has keycodes arranged in the 65% layout of the keyboard).  This enables users to intuitively modify the keycodes assigned to keys of their keyboard.

```c
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT( \
        KC_ESC,  KC_1,    KC_2,    KC_3, KC_4, KC_5, KC_6,   KC_7, KC_8, KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME, \
        KC_TAB,  KC_Q,    KC_W,    KC_E, KC_R, KC_T, KC_Y,   KC_U, KC_I, KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_END,  \
        KC_CAPS, KC_A,    KC_S,    KC_D, KC_F, KC_G, KC_H,   KC_J, KC_K, KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP, \
        KC_LSFT,          KC_Z,    KC_X, KC_C, KC_V, KC_B,   KC_N, KC_M, KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN, \
        KC_LCTL, KC_LWIN, KC_LALT,                   KC_SPC,                      KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT \
    ),
};
```

When I tried to use this, I found it difficult to keep the Keycodes aligned with each other in the call to `LAYOUT`.  Here's an example with just the escape key modified.  A few keys like this and the 65% layout either has to be *very* wide, or will be unreadable.

```c
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT( \
        LT(CONTROL_LAYER, KC_GRAVE),  KC_1,    KC_2,    KC_3, KC_4, KC_5, KC_6,   KC_7, KC_8, KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME, \
        KC_TAB,  KC_Q,    KC_W,    KC_E, KC_R, KC_T, KC_Y,   KC_U, KC_I, KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_END,  \
        KC_CAPS, KC_A,    KC_S,    KC_D, KC_F, KC_G, KC_H,   KC_J, KC_K, KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP, \
        KC_LSFT,          KC_Z,    KC_X, KC_C, KC_V, KC_B,   KC_N, KC_M, KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN, \
        KC_LCTL, KC_LWIN, KC_LALT,                   KC_SPC,                      KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT \
    ),
};
```

...so I developed another way and I think it's easier to maintain.  Instead of passing all of the keycodes to the function-like macro, you simply pass a layer prefix and the updated macro (`slice65`) will obtain the keycodes from variable-like macros.  For each layer, I created a header file to be included that looks like the following.

```c
#define BASE_ESC  LT(CONTROL_LAYER, KC_GRAVE) // KC_ESC
#define BASE_1    KC_1
#define BASE_2    KC_2
#define BASE_3    KC_3
#define BASE_4    KC_4
```

It's easy to see that when the user presses the `1` key in the base layer, `KC_1` will be issued.  Escape does something more complex, but it's not hard to see that that complex thing runs when escape is pressed.  This is because the `slice65` macro identifies the top left key as escape with the `_ESC` suffix.  All of the keys have suffixes.

When creating a new layer, you have to change all of the layer prefixes, but with Vim or Neovim, this is easy with VERTICAL BLOCK, substitute, etc.

The `slice65` macro is defined in `slice.h` and used to return the requisite two-dimensional array to associate with the layer in `keymaps/cskeeters/keymap.c` as follows.

```c
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE_LAYER]    = slice65(BASE),
    [MAC_LAYER]     = slice65(MAC),
    [VIM_LAYER]     = slice65(VIM),
    [CONTROL_LAYER] = slice65(CONTROL),
    [NUMPAD_LAYER]  = slice65(NUMPAD),
    [EOE_LAYER]     = slice65(EOE),
};
```

The `slice65` definition takes `LP` (for layer prefix).  When `BASE` is passed as `LP`, the keycode assigned to BASE_ESC is set where `LP##_ESC` is set in the array.

```c
// LP is the Layer Prefix for key definitions
#define slice65(LP) \
{ \
    {LP##_ESC,   LP##_2,  LP##_4,   LP##_6,  LP##_8,   LP##_0,  LP##_EQ,  LP##_HOM},\
    {LP##_1,     LP##_3,  LP##_5,   LP##_7,  LP##_9,   LP##_MI, LP##_BSP, KC_NO},\
    {LP##_TAB,   LP##_W,  LP##_R,   LP##_Y,  LP##_I,   LP##_P,  LP##_RB,  LP##_END},\
    {LP##_Q,     LP##_E,  LP##_T,   LP##_U,  LP##_O,   LP##_LB, LP##_BSL, KC_NO},\
    {LP##_CAP,   LP##_S,  LP##_F,   LP##_H,  LP##_K,   LP##_SC, KC_NO,  LP##_PUP},\
    {LP##_A,     LP##_D,  LP##_G,   LP##_J,  LP##_L,   LP##_QT, LP##_ENT,   KC_NO},\
    {LP##_LSH,   LP##_Z,  LP##_C,   LP##_B,  LP##_M,   LP##_DT, LP##_RSH,   LP##_PDN},\
    {KC_NO,    LP##_X,  LP##_V,   LP##_N,  LP##_CM,  LP##_SL,  LP##_AUP,   KC_NO},\
    {LP##_LCTL,  LP##_LALT,  KC_NO,  LP##_SP,  KC_NO,  LP##_RALT,  LP##_AL,   LP##_AR},\
    {LP##_LGUI,  KC_NO, KC_NO,  KC_NO, KC_NO,  LP##_RCTL, LP##_ADN,  KC_NO}\
}
```

NOTE: ## is the [concatenate preprocessor operator](https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html).

Creating this macro is a bit tricky but only has to be done once per keyboard.  It can be accomplished by looking at the default keymap.c and the `LAYOUT` macro.  One must observe how `KC_ESC` in `keymaps/default/keymap.c` is assigned to `K1_1` and then note the position that variable took in the final array, then repeat for each key.

## Pros/Cons

This layout scheme has the following pros:

1. If I assign the O keycode to the W key, this mapping is very obvious when reading the code
2. I don't have to worry about maintaining spacing of the keycodes so that they match the 65% layout of the board
3. I can use the non-abbreviated key names KC_LOCKING_SCROLL_LOCK in my mappings
4. It creates some separation of concerns, as you an develop a layer without worrying about what layer number will be associated with it.
5. It's easier to modify the mappings for use in a different keyboard with a different mapping.  The header files could be modified fairly easily to support other layouts by adding lines for he addional keys.  If the key is not used in the layout, it doesn't hurt to define it, but they can be removed also.
6. The code is easier to changes (for users with advanced editors like vim).

## Files / Layers

I have developed a few layers in separate files.

* empty.h (EMPTY) - This is just checked in for making layers for special functions
* base.h (BASE) - This is the layer I'd use if I was on a windows computer
* mac.h (MAC) - This is the layer I'd use if I was on a mac computer with LGUI next to the space bar
* vim.h (VIM) - This is a modification of the MAC layer with caps lock sending escape to the computer and escape sending grave/tilde
* control.h (CONTROL) - This is the same as *empty*, but will be customized as a control layer for me
* numpad.h (NUMPAD) - This enables the use of a number pad in the right hand
* eoe.h (EOE) - This can be enabled with the NUMPAD so that enter disables NUMPAD.  Good for single number entry.
