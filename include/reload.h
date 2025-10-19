#ifndef RELOAD_H
#define RELOAD_H

#include <stdbool.h>
#include <stdint.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ReloadCallback)(bool success, const char* error);

// Reload weapon (1=primary, 2=secondary)
static inline void Reload(uint8_t weaponSlot, ReloadCallback callback) {
#ifdef PLATFORM_WEB
    EM_ASM({
        const weaponSlot = $0;
        const callback = $1;
        
        if (!window.SolanaGameBridge) {
            const errorMsg = "SolanaGameBridge not initialized";
            const errorPtr = allocateUTF8(errorMsg);
            Module.dynCall_vii(callback, 0, errorPtr);
            _free(errorPtr);
            return;
        }
        
        window.SolanaGameBridge.reload(weaponSlot)
            .then(() => {
                Module.dynCall_vii(callback, 1, 0);
            })
            .catch(err => {
                const errorMsg = err.message || "Reload failed";
                const errorPtr = allocateUTF8(errorMsg);
                Module.dynCall_vii(callback, 0, errorPtr);
                _free(errorPtr);
            });
    }, weaponSlot, callback);
#else
    callback(false, "Web platform only");
#endif
}

#ifdef __cplusplus
}
#endif

#endif // RELOAD_H
