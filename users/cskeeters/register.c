#include "register.h"

// KB_REGISTER_BUFFER_MAX needs to be defined in config.h

// Single linked list of register data
Register *register_head = NULL;

// when data comes in, it will be associated with this keycode in a free register or replace the oldest
char kb_register_next_keycode;

// This is where we write register data until F message is received
char kb_register_buffer[KB_REGISTER_BUFFER_MAX];
int  kb_register_buffer_offset;

int rgui_pressed = 0; // true or false depending on if RGUI is pressed


void register_init(char next_keycode)
{
    kb_register_next_keycode = next_keycode;
}

// returns true when further handling should not occur
bool register_handle_event(uint16_t keycode, keyrecord_t *record)
{
    // the pressing and releasing of RGUI itself needs to be handled for get_mod to work ok
    if (keycode == KC_RGUI) {
        rgui_pressed = record->event.pressed;

        // ignore all events associated with the right gui key itself.
        return true;
    }

    if (rgui_pressed) {

        if (!record->event.pressed) {

            if ((get_mods() & MOD_BIT(KC_RSFT)) != 0) {
                kb_register_next_keycode = keycode;
                dprintf("Set kb_register_next_keycode to %04X\n", kb_register_next_keycode);
            } else {
                Register *node = get_register(keycode);
                if (node == NULL) {
                    dprintf("No register found for keycode %02X.\n", keycode);
                    register_code(KC_LEFT_GUI);
                    tap_code(keycode);
                    unregister_code(KC_LEFT_GUI);
                } else {
                    dprintf("Sending data for register for keycode %02X.\n", keycode);
                    dprintf("Sending: %s\n", node->data);

                    bool esc = 0;
                    for (uint8_t *p = node->data; *p != 0; ++p) {
                        dprintf("Processing %c\n", (char) *p);

                        if (esc == 1) {
                            esc = 0;
                            if (*p == '\\') {
                                tap_code(KC_BACKSLASH);
                            }
                            if (*p == 'd') {
                                wait_ms(100);
                            }
                            if (*p == 'n') {
                                tap_code(KC_ENTER);
                            }
                            if (*p == 't') {
                                tap_code(KC_TAB);
                            }
                            if (*p == 'g') {
                                dprintf("reg KC_LEFT_GUI\n");
                                register_code(KC_LEFT_GUI);
                            }
                            if (*p == 'G') {
                                dprintf("unreg KC_LEFT_GUI\n");
                                unregister_code(KC_LEFT_GUI);
                            }
                            if (*p == 'a') {
                                register_code(KC_LEFT_ALT);
                            }
                            if (*p == 'A') {
                                unregister_code(KC_LEFT_ALT);
                            }
                            if (*p == 's') {
                                register_code(KC_LEFT_SHIFT);
                            }
                            if (*p == 'S') {
                                unregister_code(KC_LEFT_SHIFT);
                            }
                        } else {
                            // Escape character enables user to trigger modifers
                            if (*p == '\\') {
                                esc = 1;
                            } else {
                                dprintf("tap_code %c\n", (char)*p);
                                send_char((char) *p);
                            }
                        }
                    }
                }
            }
        }
        // ignore all events when right gui is currently pressed
        return true;
    }

    return false;
}

// Create a new node, append it into the linked list, and initialize node->next to NULL.
Register *init_new_register(void)
{
    Register *node = (Register *) malloc(sizeof(Register));
    if (node == NULL) {
        dprintf("Error allocating memory for register\n");
        return NULL;
    }
    dprintf("Allocated memory for node\n");

    // Make sure the new node's next member doesn't point to anything
    node->next = NULL;

    if (register_head == NULL) {
        register_head = node;
        return node;
    }

    Register *prev = register_head;
    Register *cur = register_head->next;
    while (cur != NULL) {
        prev = cur;
        cur = cur->next;
    }
    prev->next = node;
    return node;
}

Register *get_register(uint16_t keycode)
{
    Register *r = register_head;
    while (r != NULL) {
        if (r->keycode == keycode) {
            return r;
        }
        r = r->next;
    }
    // Did not find register for this keycode
    return NULL;
}

void send_raw_hid_response(char *msg, uint8_t length)
{
    uint8_t response[length];
    strcpy((char *)response, msg);
    raw_hid_send(response, length);
}

void raw_hid_receive(uint8_t *data, uint8_t length)
{
    dprintf("DEBUG: raw_hid_receive message: %c\n", data[0]);

    if (data[0] == 'K') { // Set Key
        // use QMK's LUT to translate ASCII to keycode
        uint8_t keycode = pgm_read_byte(&ascii_to_keycode_lut[(uint8_t)data[1]]);
        kb_register_next_keycode = keycode;
        dprintf("Set kb_register_next_keycode to %04X\n", kb_register_next_keycode);
        send_raw_hid_response("OK", length);
        return;

    } else if (data[0] == 'S') { // Initial set
        // Reinitialize kb_register to wipe out all appended data (past length)
        memset(kb_register_buffer, 0, KB_REGISTER_BUFFER_MAX);
        kb_register_buffer_offset = 0;

        // Copy over data, incrementing kb_register_buffer_offset for each byte written
        for (int i=1; i<length; i++) {
            if (data[i] == 0) {
                break;
            }
            kb_register_buffer[kb_register_buffer_offset++] = data[i];
        }

        send_raw_hid_response("OK", length);
        return;

    } else if (data[0] == 'A') { // Append

        // Copy over data, incrementing kb_register_buffer_offset for each byte written
        for (int i=1; i<length; i++) {
            if (kb_register_buffer_offset < KB_REGISTER_BUFFER_MAX) {
                if (data[i] == 0) {
                    break;
                }
                kb_register_buffer[kb_register_buffer_offset++] = data[i];
            } else {
                send_raw_hid_response("Overflow", length);
                return;
            }
        }

        send_raw_hid_response("OK", length);
        return;

    } else if (data[0] == 'F') { // Finish (Store written register)
        data = malloc(kb_register_buffer_offset+1); // add for one zero
        if (data == NULL) {
            send_raw_hid_response("Out of Memory", length);
            return;
        }
        dprintf("Allocated memory for text\n");

        Register *node = get_register(kb_register_next_keycode);
        if (node == NULL) {
            node = init_new_register();
            if (node == NULL) {
                send_raw_hid_response("Out of Memory", length);
                return;
            }
        } else {
            // Free existing node's data
            free(node->data);
        }

        node->keycode = kb_register_next_keycode;
        node->data = data;
        // Next initialized by init_new_register

        // Copy data with one zero
        memcpy(node->data, kb_register_buffer, kb_register_buffer_offset+1);

        send_raw_hid_response("OK", length);
        return;
    }

    dprintf("Unknown instruction: 0x%02x", data[0]);
}
