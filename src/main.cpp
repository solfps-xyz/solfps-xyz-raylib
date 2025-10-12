#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "privy_bridge.h"
#include "player.h"
#include "map.h"
#include "gun.h"
#include "ui.h"

int main() {
    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "solfps.xyz - Cyberpunk Arena FPS");
    
    // Initialize Privy Bridge
    PrivyBridge::init();
    
    // Wallet state
    bool walletConnected = false;
    std::string walletAddress = "";
    double solBalance = 0.0;
    
    // Game objects
    Player player;
    Map map;
    Gun gun;
    
    // Load cyberpunk arena map
    map.loadCyberpunkArena();
    
    // Player stats
    float playerHealth = 100.0f;
    float maxHealth = 100.0f;
    float playerRadius = 0.4f;
    
    DisableCursor();
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update
        //----------------------------------------------------------------------------------
        
        // Update player
        player.update(deltaTime);
        
        // Collision detection
        Vector3 correction;
        if (map.checkCollision(player.camera.position, playerRadius, correction)) {
            player.camera.position = Vector3Add(player.camera.position, correction);
        }
        
        // Update gun
        bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);
        gun.update(deltaTime, isMoving, player.isShooting);
        
        // Check wallet connection status
        walletConnected = PrivyBridge::isWalletConnected();
        
        // Update wallet info if connected
        if (walletConnected) {
            walletAddress = PrivyBridge::getWalletAddress();
            solBalance = PrivyBridge::getSolanaBalance();
        }
        
        // Press C to connect wallet
        if (IsKeyPressed(KEY_C) && !walletConnected) {
            PrivyBridge::requestConnectWallet();
        }
        
        // Press X to disconnect wallet
        if (IsKeyPressed(KEY_X) && walletConnected) {
            PrivyBridge::requestDisconnectWallet();
        }
        
        // Toggle cursor lock with ESC key
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground((Color){ 10, 10, 15, 255 }); // Dark cyberpunk background
            
            BeginMode3D(player.camera);
                // Draw map
                map.draw();
                
                // TODO: Draw enemies, particles, effects
                
            EndMode3D();
            
            // Draw gun viewmodel (after 3D scene, before UI)
            gun.draw(player.camera);
            
            // Draw HUD
            UI::drawCrosshair(screenWidth, screenHeight);
            UI::drawGunHUD(player.ammo, player.maxAmmo, screenWidth, screenHeight);
            UI::drawHealthBar(playerHealth, maxHealth, screenWidth, screenHeight);
            UI::drawWalletInfo(walletConnected, walletAddress, solBalance);
            UI::drawControls();
            
            DrawFPS(screenWidth - 100, 10);
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    CloseWindow();
    
    return 0;
}