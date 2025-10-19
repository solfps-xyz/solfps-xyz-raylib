#ifndef APPLY_DAMAGE_H
#define APPLY_DAMAGE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ApplyDamageCallback)(bool success, const char* error);

// Apply damage to victim
// victimAddress: base58 string
// weaponType: 1=primary, 2=secondary
// isHeadshot: 0=body, 1=headshot
// distance: float distance to target
static inline void ApplyDamage(
    const char* victimAddress,
    uint8_t weaponType,
    uint8_t isHeadshot,
    float distance,
    ApplyDamageCallback callback
) {
#ifdef PLATFORM_WEB
    EM_ASM({
        const victimAddr = UTF8ToString($0);
        const weaponType = $1;
        const isHeadshot = $2;
        const distance = $3;
        const callback = $4;
        
        if (!window.SolanaGameBridge) {
            const errorMsg = "SolanaGameBridge not initialized";
            const errorPtr = allocateUTF8(errorMsg);
            Module.dynCall_vii(callback, 0, errorPtr);
            _free(errorPtr);
            return;
        }
        
        window.SolanaGameBridge.applyDamage(
            victimAddr,
            weaponType,
            isHeadshot,
            distance
        )
            .then(() => {
                Module.dynCall_vii(callback, 1, 0);
            })
            .catch(err => {
                const errorMsg = err.message || "Apply damage failed";
                const errorPtr = allocateUTF8(errorMsg);
                Module.dynCall_vii(callback, 0, errorPtr);
                _free(errorPtr);
            });
    }, victimAddress, weaponType, isHeadshot, distance, callback);
#else
    callback(false, "Web platform only");
#endif
}

#ifdef __cplusplus
}
#endif

#endif // APPLY_DAMAGE_H
