#ifdef ENABLE_RGB_MATRIX_CALLBACK
RGB_MATRIX_EFFECT(CALLBACK)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool CALLBACK(effect_params_t* params) {
    if (rgb_mode_callback != NULL) {
        return rgb_mode_callback(params);
    }
    return true; // report finished if no callback installed
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // ENABLE_RGB_MATRIX_CALLBACK
