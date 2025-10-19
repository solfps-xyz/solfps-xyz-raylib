#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MovementCallback)(bool success, const char* error);

// Update player movement
// x, y, z: position coordinates
// rotation: player rotation
// velocity: movement velocity
static inline void UpdateMovement(
    float x,
    float y, 
    float z,
    float rotation,
    float velocityX,
    float velocityY,
    float velocityZ,
    MovementCallback callback
) {
#ifdef PLATFORM_WEB
    EM_ASM({
        const x = $0;
        const y = $1;
        const z = $2;
        const rotation = $3;
        const velocityX = $4;
        const velocityY = $5;
        const velocityZ = $6;
        const callback = $7;
        
        if (!window.SolanaGameBridge) {
            const errorMsg = "SolanaGameBridge not initialized";
            const errorPtr = allocateUTF8(errorMsg);
            Module.dynCall_vii(callback, 0, errorPtr);
            _free(errorPtr);
            return;
        }
        
        window.SolanaGameBridge.updateMovement(
            x, y, z, rotation, velocityX, velocityY, velocityZ
        )
            .then(() => {
                Module.dynCall_vii(callback, 1, 0);
            })
            .catch(err => {
                const errorMsg = err.message || "Update movement failed";
                const errorPtr = allocateUTF8(errorMsg);
                Module.dynCall_vii(callback, 0, errorPtr);
                _free(errorPtr);
            });
    }, x, y, z, rotation, velocityX, velocityY, velocityZ, callback);
#else
    callback(false, "Web platform only");
#endif
}

#ifdef __cplusplus
}
#endif

#endif // MOVEMENT_H
