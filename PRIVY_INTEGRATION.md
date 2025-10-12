# Privy Bridge Integration Guide

## Overview
The Privy Bridge allows your C++ Raylib game to interact with the Privy wallet JavaScript API. The bridge automatically polls wallet status and caches values so they can be accessed synchronously from the game loop.

## How It Works

### 1. **Automatic Polling**
When you call `PrivyBridge::init()`, it sets up a JavaScript interval that checks wallet status every second and stores the results in Module variables that C++ can access synchronously.

### 2. **Synchronous Getters (Safe for Game Loop)**
```cpp
bool isWalletConnected()     // Check if wallet is connected
std::string getWalletAddress() // Get the Solana wallet address
double getSolanaBalance()      // Get balance in lamports
```

These methods are **safe to call every frame** because they return cached values updated by the polling system.

### 3. **Async Actions (Non-blocking)**
```cpp
void requestConnectWallet()       // Initiate wallet connection
void requestDisconnectWallet()    // Disconnect wallet
void requestSignMessage(const char* message)  // Sign a message
void requestSendTransaction(const char* recipient, int lamports) // Send SOL
```

These methods trigger JavaScript promises but **don't block** your game. They return immediately.

## Usage Example

### In main.cpp:
```cpp
#include "privy_bridge.h"

int main() {
    InitWindow(800, 600, "Game");
    
    // Initialize the bridge once at startup
    PrivyBridge::init();
    
    bool walletConnected = false;
    std::string walletAddress = "";
    double solBalance = 0.0;
    
    while (!WindowShouldClose()) {
        // Check wallet status (updates from polling)
        walletConnected = PrivyBridge::isWalletConnected();
        
        if (walletConnected) {
            walletAddress = PrivyBridge::getWalletAddress();
            solBalance = PrivyBridge::getSolanaBalance();
        }
        
        // Connect wallet on key press
        if (IsKeyPressed(KEY_C) && !walletConnected) {
            PrivyBridge::requestConnectWallet();
        }
        
        // Drawing code...
    }
    
    CloseWindow();
    return 0;
}
```

## JavaScript Side Setup

### Your HTML needs to provide the PrivyBridge object:

```javascript
window.PrivyBridge = {
    async isWalletConnected() {
        // Return true/false
    },
    
    async getWalletAddress() {
        // Return wallet address string or null
    },
    
    async getSolanaAddress() {
        // Return Solana-specific address
    },
    
    async getSolanaBalance() {
        // Return balance in lamports (number)
    },
    
    async connectWallet() {
        // Initiate connection flow
    },
    
    async disconnectWallet() {
        // Disconnect wallet
    },
    
    async getUserId() {
        // Return user ID or null
    },
    
    async getUserEmail() {
        // Return user email or null
    },
    
    async signSolanaMessage(message) {
        // Sign message, return signature
    },
    
    async sendSolanaTransaction(recipientAddress, amountLamports) {
        // Send transaction, return tx hash
    }
};
```

## Testing

A test file `test-privy.html` is included with a mock PrivyBridge implementation. To test:

```sh
cd webbuild
npx serve
```

Then open `http://localhost:3000/test-privy.html`

## Converting Balance

The bridge returns balance in **lamports** (smallest SOL unit).
To convert to SOL:
```cpp
double solBalance = PrivyBridge::getSolanaBalance();
double balanceInSOL = solBalance / 1000000000.0;  // 1 SOL = 1 billion lamports
```

## Important Notes

1. **Always call `PrivyBridge::init()` once at startup** - This starts the polling mechanism
2. **Polling happens every 1 second** - Wallet status updates may have up to 1 second delay
3. **Action requests are non-blocking** - They return immediately, results appear in next poll
4. **Desktop builds have stub implementations** - All methods return safe default values

## Available Methods

### Getters (Synchronous - Safe for game loop)
- `bool isWalletConnected()`
- `std::string getWalletAddress()`
- `double getSolanaBalance()`
- `std::string getUserId()` (may return empty, async only)
- `std::string getUserEmail()` (may return empty, async only)

### Actions (Async - Non-blocking)
- `void init()` - Initialize polling (call once at startup)
- `void requestConnectWallet()`
- `void requestDisconnectWallet()`
- `void requestSignMessage(const char* message)`
- `void requestSendTransaction(const char* recipientAddress, int amountLamports)`
