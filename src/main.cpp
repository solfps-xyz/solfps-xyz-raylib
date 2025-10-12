#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "privy_bridge.h"

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "solfps.xyz - First Person Controller");
    
    // Initialize Privy Bridge
    PrivyBridge::init();
    
    // Wallet state
    bool walletConnected = false;
    std::string walletAddress = "";
    double solBalance = 0.0;
    
    // Define the camera to look into our 3D world (first person)
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    
    // Player movement settings
    float moveSpeed = 0.1f;
    float mouseSensitivity = 0.003f;
    float yaw = -90.0f;   // Horizontal rotation
    float pitch = 0.0f;   // Vertical rotation
    
    // Ground plane
    Vector3 groundPosition = { 0.0f, 0.0f, 0.0f };
    
    // Create some cubes for the scene
    Vector3 cubePositions[] = {
        { -2.0f, 0.5f, 0.0f },
        { 2.0f, 0.5f, 0.0f },
        { 0.0f, 0.5f, -2.0f },
        { 0.0f, 0.5f, 2.0f },
        { -3.0f, 0.5f, -3.0f },
        { 3.0f, 0.5f, -3.0f },
        { -3.0f, 0.5f, 3.0f },
        { 3.0f, 0.5f, 3.0f }
    };
    
    DisableCursor(); // Lock cursor to the window
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        
        // Mouse look
        Vector2 mouseDelta = GetMouseDelta();
        yaw += mouseDelta.x * mouseSensitivity;
        pitch -= mouseDelta.y * mouseSensitivity;
        
        // Clamp pitch to prevent camera flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        
        // Calculate camera direction from yaw and pitch
        Vector3 direction;
        direction.x = cosf(yaw) * cosf(pitch);
        direction.y = sinf(pitch);
        direction.z = sinf(yaw) * cosf(pitch);
        direction = Vector3Normalize(direction);
        
        // Update camera target based on position and direction
        camera.target = Vector3Add(camera.position, direction);
        
        // Movement
        Vector3 forward = { direction.x, 0.0f, direction.z };
        forward = Vector3Normalize(forward);
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        
        // WASD movement
        if (IsKeyDown(KEY_W)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, moveSpeed));
        }
        if (IsKeyDown(KEY_S)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, moveSpeed));
        }
        if (IsKeyDown(KEY_A)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(right, moveSpeed));
        }
        if (IsKeyDown(KEY_D)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(right, moveSpeed));
        }
        
        // Space to jump/go up, Left Shift to go down
        if (IsKeyDown(KEY_SPACE)) {
            camera.position.y += moveSpeed;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            camera.position.y -= moveSpeed;
        }
        
        // Update camera target
        camera.target = Vector3Add(camera.position, direction);
        
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
            ClearBackground(SKYBLUE);
            
            BeginMode3D(camera);
                // Draw ground
                DrawPlane(groundPosition, (Vector2){ 20.0f, 20.0f }, DARKGRAY);
                DrawGrid(20, 1.0f);
                
                // Draw cubes
                for (int i = 0; i < 8; i++) {
                    Color cubeColor = (i % 2 == 0) ? RED : BLUE;
                    DrawCube(cubePositions[i], 1.0f, 1.0f, 1.0f, cubeColor);
                    DrawCubeWires(cubePositions[i], 1.0f, 1.0f, 1.0f, MAROON);
                }
                
                // Draw a reference cube at origin
                DrawCube((Vector3){ 0.0f, 0.5f, 0.0f }, 1.0f, 1.0f, 1.0f, YELLOW);
                DrawCubeWires((Vector3){ 0.0f, 0.5f, 0.0f }, 1.0f, 1.0f, 1.0f, ORANGE);
                
            EndMode3D();
            
            // Draw UI - Controls
            DrawRectangle(10, 10, 320, 123, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 10, 320, 123, BLUE);
            DrawText("First Person Controller", 20, 20, 10, BLACK);
            DrawText("WASD - Move", 20, 40, 10, DARKGRAY);
            DrawText("MOUSE - Look around", 20, 55, 10, DARKGRAY);
            DrawText("SPACE/SHIFT - Up/Down", 20, 70, 10, DARKGRAY);
            DrawText("ESC - Toggle cursor", 20, 85, 10, DARKGRAY);
            DrawText("C - Connect Wallet", 20, 100, 10, DARKGRAY);
            DrawText("X - Disconnect Wallet", 20, 115, 10, DARKGRAY);
            
            // Draw Wallet Status
            int walletUIY = 145;
            DrawRectangle(10, walletUIY, 400, 95, Fade(DARKGREEN, 0.5f));
            DrawRectangleLines(10, walletUIY, 400, 95, GREEN);
            DrawText("Wallet Status", 20, walletUIY + 10, 10, WHITE);
            
            if (walletConnected) {
                DrawText("Status: Connected", 20, walletUIY + 30, 10, LIME);
                
                // Show wallet address (truncated)
                if (!walletAddress.empty()) {
                    std::string truncatedAddress = walletAddress.length() > 20 
                        ? walletAddress.substr(0, 8) + "..." + walletAddress.substr(walletAddress.length() - 8)
                        : walletAddress;
                    DrawText(TextFormat("Address: %s", truncatedAddress.c_str()), 20, walletUIY + 50, 10, WHITE);
                }
                
                // Show SOL balance
                DrawText(TextFormat("Balance: %.4f SOL", solBalance / 1000000000.0), 20, walletUIY + 70, 10, WHITE);
            } else {
                DrawText("Status: Not Connected", 20, walletUIY + 30, 10, RED);
                DrawText("Press C to connect", 20, walletUIY + 55, 10, YELLOW);
            }
            
            // Draw crosshair
            DrawLine(screenWidth / 2 - 10, screenHeight / 2, screenWidth / 2 + 10, screenHeight / 2, BLACK);
            DrawLine(screenWidth / 2, screenHeight / 2 - 10, screenWidth / 2, screenHeight / 2 + 10, BLACK);
            
            DrawFPS(10, screenHeight - 30);
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    CloseWindow();
    
    return 0;
}