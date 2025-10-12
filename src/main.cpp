#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <iostream>
#include <vector>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #include <GLES2/gl2.h>
#endif

#include "privy_bridge.h"
#include "player.h"
#include "map.h"
#include "gun.h"
#include "ui.h"

// Particle structures for effects
struct BulletTracer {
    Vector3 start;
    Vector3 end;
    float lifetime;
    Color color;
};

struct ImpactParticle {
    Vector3 position;
    Vector3 velocity;
    float lifetime;
    float maxLifetime;
    Color color;
};

int main() {
    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "solfps.xyz - Cyberpunk Arena FPS");
    
    // Initialize audio device
    InitAudioDevice();
    
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
    
    // Effects system
    std::vector<BulletTracer> bulletTracers;
    std::vector<ImpactParticle> impactParticles;
    bool lastShooting = false;
    
    DisableCursor();
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update
        //----------------------------------------------------------------------------------
        
        // Update player (this will set isShooting if mouse is clicked)
        player.update(deltaTime);
        
        // Collision detection
        Vector3 correction;
        if (map.checkCollision(player.camera.position, playerRadius, correction)) {
            player.camera.position = Vector3Add(player.camera.position, correction);
        }
        
        // Update gun with movement and sprint state
        bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);
        gun.update(deltaTime, isMoving, player.isShooting, player.isSprinting);
        
        // Create bullet tracer when shooting
        if (player.isShooting && !lastShooting) {
            // Calculate gun barrel position (where muzzle flash appears)
            Vector3 forward = Vector3Normalize(Vector3Subtract(player.camera.target, player.camera.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, player.camera.up));
            Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));
            
            // Gun barrel position (matches gun rendering position)
            Vector3 gunPos = player.camera.position;
            gunPos = Vector3Add(gunPos, Vector3Scale(right, 0.25f));      // Right
            gunPos = Vector3Add(gunPos, Vector3Scale(up, -0.15f));         // Down
            gunPos = Vector3Add(gunPos, Vector3Scale(forward, 0.4f));      // Forward
            
            // Barrel tip (add barrel length)
            Vector3 barrelPos = Vector3Add(gunPos, Vector3Scale(forward, 0.2f));
            Vector3 start = Vector3Add(barrelPos, Vector3Scale(forward, 0.08f)); // Muzzle tip
            Vector3 end = Vector3Add(start, Vector3Scale(forward, 300.0f)); // 300 units range
            
            // Simple collision check with walls
            bool hitWall = false;
            Vector3 hitPoint = end;
            
            // Check collision with each wall in the map
            for (const auto& wall : map.walls) {
                Ray ray = { start, forward };
                BoundingBox box = {
                    { wall.position.x - wall.size.x/2, wall.position.y - wall.size.y/2, wall.position.z - wall.size.z/2 },
                    { wall.position.x + wall.size.x/2, wall.position.y + wall.size.y/2, wall.position.z + wall.size.z/2 }
                };
                RayCollision collision = GetRayCollisionBox(ray, box);
                
                if (collision.hit) {
                    float dist = Vector3Distance(start, collision.point);
                    float currentDist = Vector3Distance(start, hitPoint);
                    if (dist < currentDist) {
                        hitPoint = collision.point;
                        hitWall = true;
                    }
                }
            }
            
            if (hitWall) {
                end = hitPoint;
                
                // Create more visible impact particles
                for (int i = 0; i < 15; i++) {
                    ImpactParticle p;
                    p.position = hitPoint;
                    p.velocity = (Vector3){
                        (float)(GetRandomValue(-200, 200)) / 100.0f,
                        (float)(GetRandomValue(50, 200)) / 100.0f,
                        (float)(GetRandomValue(-200, 200)) / 100.0f
                    };
                    p.lifetime = 0.8f;
                    p.maxLifetime = 0.8f;
                    
                    // Mix of cyan and orange sparks
                    if (i % 2 == 0) {
                        p.color = (Color){ 0, 255, 255, 255 }; // Cyan sparks
                    } else {
                        p.color = (Color){ 255, 150, 0, 255 }; // Orange sparks
                    }
                    impactParticles.push_back(p);
                }
            }
            
            // Create brighter, longer-lasting bullet tracer
            BulletTracer tracer;
            tracer.start = start;
            tracer.end = end;
            tracer.lifetime = 0.15f; // Longer duration
            tracer.color = (Color){ 255, 255, 0, 255 }; // Bright yellow
            bulletTracers.push_back(tracer);
        }
        
        // Update lastShooting and reset the flag for next frame
        lastShooting = player.isShooting;
        player.isShooting = false;
        
        // Update bullet tracers
        for (auto it = bulletTracers.begin(); it != bulletTracers.end();) {
            it->lifetime -= deltaTime;
            if (it->lifetime <= 0.0f) {
                it = bulletTracers.erase(it);
            } else {
                ++it;
            }
        }
        
        // Update impact particles
        for (auto it = impactParticles.begin(); it != impactParticles.end();) {
            it->lifetime -= deltaTime;
            it->position = Vector3Add(it->position, Vector3Scale(it->velocity, deltaTime));
            it->velocity.y -= 9.8f * deltaTime; // Gravity
            
            if (it->lifetime <= 0.0f) {
                it = impactParticles.erase(it);
            } else {
                ++it;
            }
        }
        
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
            ClearBackground((Color){ 5, 5, 10, 255 }); // Darker cyberpunk background
            
            // Draw 3D scene
            BeginMode3D(player.camera);
                // Setup lighting for better depth perception
                // Directional light from above-front for ambient occlusion feel
                Vector3 lightPos = { player.camera.position.x, player.camera.position.y + 20.0f, player.camera.position.z + 10.0f };
                
                // Draw map with fog effect
                map.draw();
                
                // Draw bullet tracers
                for (const auto& tracer : bulletTracers) {
                    float alpha = (tracer.lifetime / 0.15f) * 255.0f;
                    
                    // Draw thick tracer line
                    DrawCylinderEx(tracer.start, tracer.end, 0.02f, 0.02f, 4,
                                  (Color){ tracer.color.r, tracer.color.g, tracer.color.b, (unsigned char)alpha });
                    
                    // Draw bright core
                    DrawLine3D(tracer.start, tracer.end, 
                              (Color){ 255, 255, 255, (unsigned char)alpha });
                    
                    // Draw glow sphere at start (muzzle)
                    DrawSphere(tracer.start, 0.05f, 
                              (Color){ 255, 200, 0, (unsigned char)alpha });
                }
                
                // Draw impact particles
                for (const auto& particle : impactParticles) {
                    float alpha = (particle.lifetime / particle.maxLifetime) * 255.0f;
                    float size = 0.03f + (1.0f - particle.lifetime / particle.maxLifetime) * 0.05f;
                    
                    // Draw particle cube
                    DrawCube(particle.position, size, size, size,
                            (Color){ particle.color.r, particle.color.g, particle.color.b, (unsigned char)alpha });
                    
                    // Draw glow
                    DrawSphere(particle.position, size * 0.5f,
                              (Color){ particle.color.r, particle.color.g, particle.color.b, (unsigned char)(alpha * 0.5f) });
                }
                
                // Muzzle flash dynamic lighting - light up the area when shooting
                if (gun.isRecoiling && gun.recoilAngle > 0.5f) {
                    // Calculate muzzle position
                    Vector3 forward = Vector3Normalize(Vector3Subtract(player.camera.target, player.camera.position));
                    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, player.camera.up));
                    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));
                    
                    Vector3 muzzlePos = player.camera.position;
                    muzzlePos = Vector3Add(muzzlePos, Vector3Scale(right, 0.25f));
                    muzzlePos = Vector3Add(muzzlePos, Vector3Scale(up, -0.15f));
                    muzzlePos = Vector3Add(muzzlePos, Vector3Scale(forward, 0.68f)); // At barrel tip
                    
                    // Bright spherical light source
                    float lightIntensity = gun.recoilAngle / 2.5f; // Fades with recoil
                    float lightRadius = 8.0f + lightIntensity * 4.0f; // Dynamic size
                    
                    // Draw multiple light spheres for volumetric effect
                    DrawSphere(muzzlePos, lightRadius, (Color){ 255, 200, 100, 15 });
                    DrawSphere(muzzlePos, lightRadius * 0.7f, (Color){ 255, 220, 150, 25 });
                    DrawSphere(muzzlePos, lightRadius * 0.4f, (Color){ 255, 240, 200, 40 });
                    
                    // Cast light rays in forward direction
                    Vector3 lightEnd = Vector3Add(muzzlePos, Vector3Scale(forward, 15.0f));
                    DrawCylinderEx(muzzlePos, lightEnd, lightRadius * 0.6f, 0.1f, 8,
                                  (Color){ 255, 230, 180, (unsigned char)(20 * lightIntensity) });
                }
                
            EndMode3D();
            
            // Clear depth buffer for gun rendering (so it appears on top)
            #if defined(PLATFORM_WEB)
                glClear(GL_DEPTH_BUFFER_BIT);
            #endif
            
            // Draw gun in its own 3D context with cleared depth
            BeginMode3D(player.camera);
                gun.drawSimple(player.camera);
            EndMode3D();
            
            // Muzzle flash screen overlay (brightens entire screen slightly)
            if (gun.isRecoiling && gun.recoilAngle > 1.0f) {
                float flashIntensity = (gun.recoilAngle / 2.5f) * 40.0f;
                DrawRectangle(0, 0, screenWidth, screenHeight,
                             (Color){ 255, 220, 150, (unsigned char)flashIntensity });
            }
            
            // NOW clear depth buffer for UI rendering
            rlDrawRenderBatchActive();
            #if defined(PLATFORM_WEB)
                glClear(GL_DEPTH_BUFFER_BIT);
            #else
                rlgl.State.framebufferWidth = 0; // Force depth clear
            #endif
            
            // Post-processing effects
            // Vignette effect - darken edges of screen
            int vignetteSize = 300; // How far the vignette extends inward
            int steps = 12; // Gradient smoothness
            
            for (int i = 0; i < steps; i++) {
                float t = (float)i / (float)steps;
                float alpha = 35.0f * t * t; // Quadratic falloff for smooth gradient
                float inset = vignetteSize * (1.0f - t);
                
                // Top bar
                DrawRectangle(0, (int)inset, screenWidth, 1, (Color){ 0, 0, 0, (unsigned char)alpha });
                // Bottom bar
                DrawRectangle(0, screenHeight - (int)inset, screenWidth, 1, (Color){ 0, 0, 0, (unsigned char)alpha });
                // Left bar
                DrawRectangle((int)inset, 0, 1, screenHeight, (Color){ 0, 0, 0, (unsigned char)alpha });
                // Right bar
                DrawRectangle(screenWidth - (int)inset, 0, 1, screenHeight, (Color){ 0, 0, 0, (unsigned char)alpha });
            }
            
            // Subtle chromatic aberration/glow around screen edges
            DrawRectangleLinesEx((Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                                2.0f, (Color){ 0, 120, 180, 20 });
            DrawRectangleLinesEx((Rectangle){ 3, 3, screenWidth - 6.0f, screenHeight - 6.0f },
                                1.0f, (Color){ 120, 50, 180, 15 });
            
            // Draw HUD (direct 2D draw, no modes)
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
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}