#ifndef PRIVY_BRIDGE_H
#define PRIVY_BRIDGE_H

#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

class PrivyBridge {
public:
    // Initialize the bridge - call this once at startup
    static void init() {
        EM_ASM({
            // Initialize wallet state tracking
            Module.privyWalletConnected = false;
            Module.privyWalletAddress = "";
            Module.privySolanaBalance = 0.0;
            
            // Poll wallet status periodically
            setInterval(function() {
                if (window.PrivyBridge && window.PrivyBridge.isWalletConnected) {
                    window.PrivyBridge.isWalletConnected().then(function(connected) {
                        Module.privyWalletConnected = connected;
                        
                        if (connected && window.PrivyBridge.getSolanaAddress) {
                            window.PrivyBridge.getSolanaAddress().then(function(address) {
                                if (address) {
                                    Module.privyWalletAddress = address;
                                }
                            });
                            
                            if (window.PrivyBridge.getSolanaBalance) {
                                window.PrivyBridge.getSolanaBalance().then(function(balance) {
                                    if (balance !== null) {
                                        Module.privySolanaBalance = balance;
                                    }
                                });
                            }
                        }
                    }).catch(function(err) {
                        console.error("Error checking wallet connection:", err);
                    });
                }
            }, 1000); // Check every second
        });
    }
    
    // Synchronous getters (safe to call from game loop)
    static bool isWalletConnected() {
        return EM_ASM_INT({
            return Module.privyWalletConnected || false;
        });
    }
    
    static std::string getWalletAddress() {
        char* result = (char*)EM_ASM_PTR({
            var addr = Module.privyWalletAddress || "";
            var len = lengthBytesUTF8(addr) + 1;
            var ptr = _malloc(len);
            stringToUTF8(addr, ptr, len);
            return ptr;
        });
        std::string addr(result);
        free(result);
        return addr;
    }
    
    static double getSolanaBalance() {
        return EM_ASM_DOUBLE({
            return Module.privySolanaBalance || 0.0;
        });
    }
    
    // Async actions (non-blocking)
    static void requestConnectWallet() {
        EM_ASM({
            if (window.PrivyBridge && window.PrivyBridge.connectWallet) {
                window.PrivyBridge.connectWallet()
                    .then(function() {
                        console.log("Wallet connected successfully");
                        Module.privyWalletConnected = true;
                    })
                    .catch(function(err) {
                        console.error("Failed to connect wallet:", err);
                    });
            } else {
                console.error("PrivyBridge not available");
            }
        });
    }
    
    static void requestDisconnectWallet() {
        EM_ASM({
            if (window.PrivyBridge && window.PrivyBridge.disconnectWallet) {
                window.PrivyBridge.disconnectWallet()
                    .then(function() {
                        console.log("Wallet disconnected successfully");
                        Module.privyWalletConnected = false;
                        Module.privyWalletAddress = "";
                        Module.privySolanaBalance = 0.0;
                    })
                    .catch(function(err) {
                        console.error("Failed to disconnect wallet:", err);
                    });
            }
        });
    }
    
    static void requestSignMessage(const char* message) {
        EM_ASM({
            var msg = UTF8ToString($0);
            if (window.PrivyBridge && window.PrivyBridge.signSolanaMessage) {
                window.PrivyBridge.signSolanaMessage(msg)
                    .then(function(signature) {
                        console.log("Message signed:", signature);
                        Module.privyLastSignature = signature;
                    })
                    .catch(function(err) {
                        console.error("Failed to sign message:", err);
                    });
            }
        }, message);
    }
    
    static void requestSendTransaction(const char* recipientAddress, int amountLamports) {
        EM_ASM({
            var recipient = UTF8ToString($0);
            var amount = $1;
            if (window.PrivyBridge && window.PrivyBridge.sendSolanaTransaction) {
                window.PrivyBridge.sendSolanaTransaction(recipient, amount)
                    .then(function(txHash) {
                        console.log("Transaction sent:", txHash);
                        Module.privyLastTxHash = txHash;
                    })
                    .catch(function(err) {
                        console.error("Failed to send transaction:", err);
                    });
            }
        }, recipientAddress, amountLamports);
    }
    
    static std::string getUserId() {
        char* result = (char*)EM_ASM_PTR({
            if (window.PrivyBridge && window.PrivyBridge.getUserId) {
                var promise = window.PrivyBridge.getUserId();
                // Note: This won't work synchronously, returning empty for now
                return 0;
            }
            return 0;
        });
        return "";
    }
    
    static std::string getUserEmail() {
        char* result = (char*)EM_ASM_PTR({
            if (window.PrivyBridge && window.PrivyBridge.getUserEmail) {
                var promise = window.PrivyBridge.getUserEmail();
                // Note: This won't work synchronously, returning empty for now
                return 0;
            }
            return 0;
        });
        return "";
    }
};

#else
// Stub implementation for desktop builds
class PrivyBridge {
public:
    static void init() {}
    static bool isWalletConnected() { return false; }
    static std::string getWalletAddress() { return ""; }
    static double getSolanaBalance() { return 0.0; }
    static void requestConnectWallet() {}
    static void requestDisconnectWallet() {}
    static void requestSignMessage(const char* message) {}
    static void requestSendTransaction(const char* recipientAddress, int amountLamports) {}
    static std::string getUserId() { return ""; }
    static std::string getUserEmail() { return ""; }
};
#endif

#endif // PRIVY_BRIDGE_H