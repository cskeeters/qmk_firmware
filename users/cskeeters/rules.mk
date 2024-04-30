INTROSPECTION_KEYMAP_C = cskeeters.c
SRC += layer.c                   # Separates out the keymap into layer helpers

# $(info $$CONSOLE_ENABLE is [${CONSOLE_ENABLE}])

LTO_ENABLE ?= yes                # Link time Optimization Size: -2k
BOOTMAGIC_ENABLE ?= yes          # Enable Bootmagic Lite
EXTRAKEY_ENABLE ?= yes           # Audio control and System control
NKRO_ENABLE ?= yes               # Enable N-Key Rollover   - Size:   358
DYNAMIC_MACRO_ENABLE ?= yes      # Enabled rec/playback    - Size: 1,162
CONSOLE_ENABLE ?= yes            # Console for debug       - Size: 5,332
CAPS_WORD_ENABLE ?= yes          # BOTH_SHIFTS_TURNS_ON_CAPS_WORD in config.h

RGB_MATRIX_ENABLE ?= no          # Use RGB matrix Size: 4696k to 8680k depending on animations
ifeq ($(strip $(RGB_MATRIX_ENABLE)), yes)
  SRC += rgb.c
endif

REGISTER_ENABLE ?= yes
ifeq ($(strip $(REGISTER_ENABLE)), yes)
  OPT_DEFS += -DREGISTER_ENABLE
  RAW_ENABLE = yes
  SRC += register.c
endif
