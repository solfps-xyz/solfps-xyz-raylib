#ifndef SHOOT_H
#define SHOOT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ShootCallback)(bool success, const char* error);

// Shoot with primary (1) or secondary (2) weapon
static inline void Shoot(uint8_t weaponSlot, ShootCallback callback) {
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
        
        window.SolanaGameBridge.shoot(weaponSlot)
            .then(() => {
                Module.dynCall_vii(callback, 1, 0);
            })
            .catch(err => {
                const errorMsg = err.message || "Shoot failed";
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

#endif // SHOOT_H
