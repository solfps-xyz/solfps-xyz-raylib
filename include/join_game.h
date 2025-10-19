#ifndef JOIN_GAME_H
#define JOIN_GAME_H

#include <stdbool.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*JoinGameCallback)(bool success, const char* error);

static inline void JoinGame(const char* gameAddress, JoinGameCallback callback) {
#ifdef PLATFORM_WEB
    EM_ASM({
        const gameAddr = UTF8ToString($0);
        const callback = $1;
        
        if (!window.SolanaGameBridge) {
            const errorMsg = "SolanaGameBridge not initialized";
            const errorPtr = allocateUTF8(errorMsg);
            Module.dynCall_vii(callback, 0, errorPtr);
            _free(errorPtr);
            return;
        }
        
        window.SolanaGameBridge.joinGame(gameAddr)
            .then(() => {
                Module.dynCall_vii(callback, 1, 0);
            })
            .catch(err => {
                const errorMsg = err.message || "Join game failed";
                const errorPtr = allocateUTF8(errorMsg);
                Module.dynCall_vii(callback, 0, errorPtr);
                _free(errorPtr);
            });
    }, gameAddress, callback);
#else
    callback(false, "Web platform only");
#endif
}

#ifdef __cplusplus
}
#endif

#endif // JOIN_GAME_H
