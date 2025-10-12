#include "gun.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <cmath>
#include <iostream>

Gun::Gun() {
    position = (Vector3){ 0.3f, -0.2f, 0.5f }; // Right side of screen
    rotation = (Vector3){ 0.0f, 0.0f, 0.0f };
    bobOffset = 0.0f;
    bobSpeed = 0.0f;
    recoilAngle = 0.0f;
    isRecoiling = false;
    sprintTilt = 0.0f;
    currentSoundIndex = 0;
    
    // Load gunshot sound - wrap in try-catch for web builds
    #if defined(PLATFORM_WEB)
        // For web, the file will be preloaded
        if (FileExists("assets/gun/audio/submachinegun-gunshot.mp3")) {
            shootSound = LoadSound("assets/gun/audio/submachinegun-gunshot.mp3");
            SetSoundVolume(shootSound, 0.5f);
            
            // Create multiple instances for overlapping sounds
            for (int i = 0; i < MAX_SOUND_INSTANCES; i++) {
                shootSoundInstances[i] = LoadSoundAlias(shootSound);
            }
        }
    #else
        shootSound = LoadSound("assets/gun/audio/submachinegun-gunshot.mp3");
        SetSoundVolume(shootSound, 0.5f);
        
        // Create multiple instances for overlapping sounds
        for (int i = 0; i < MAX_SOUND_INSTANCES; i++) {
            shootSoundInstances[i] = LoadSoundAlias(shootSound);
        }
    #endif
}

Gun::~Gun() {
    // Cleanup sound instances
    for (int i = 0; i < MAX_SOUND_INSTANCES; i++) {
        UnloadSoundAlias(shootSoundInstances[i]);
    }
    // Cleanup main sound
    UnloadSound(shootSound);
}

void Gun::update(float deltaTime, bool isMoving, bool isShooting, Vector3 playerVelocity) {
    // Weapon bob while moving
    if (isMoving) {
        bobSpeed += deltaTime * 10.0f;
        bobOffset = sinf(bobSpeed) * 0.02f;
    } else {
        bobSpeed = 0.0f;
        bobOffset *= 0.9f; // Smooth return
    }
    
    // Sprint tilt - tilt gun horizontally based on movement speed
    float horizontalSpeed = sqrtf(playerVelocity.x * playerVelocity.x + playerVelocity.z * playerVelocity.z);
    float targetTilt = 0.0f;
    
    if (horizontalSpeed > 0.5f) {
        // Tilt gun 25-35 degrees when running (classic FPS style)
        targetTilt = 30.0f * (horizontalSpeed / 10.0f); // Scale with speed
        targetTilt = fminf(targetTilt, 35.0f); // Cap at 35 degrees
    }
    
    // Smooth interpolation
    sprintTilt += (targetTilt - sprintTilt) * deltaTime * 8.0f;
    
    // Recoil
    if (isShooting) {
        applyRecoil();
    }
    
    if (isRecoiling) {
        recoilAngle -= deltaTime * 12.0f; // Faster recoil recovery (was 8.0f)
        if (recoilAngle <= 0.0f) {
            recoilAngle = 0.0f;
            isRecoiling = false;
        }
    }
}

void Gun::applyRecoil() {
    recoilAngle = 2.5f; // Lighter kick (was 5.0f)
    isRecoiling = true;
    
    // Play gunshot sound using round-robin to allow overlapping
    PlaySound(shootSoundInstances[currentSoundIndex]);
    currentSoundIndex = (currentSoundIndex + 1) % MAX_SOUND_INSTANCES;
}

void Gun::drawSimple(Camera3D camera) {
    // Gun is now drawn in its own BeginMode3D with cleared depth buffer
    // No need to manually disable depth test
    
    // Calculate gun position relative to camera orientation
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));
    
    // Calculate gun orientation angle (yaw around Y axis)
    float yaw = atan2f(forward.x, forward.z) * RAD2DEG;
    // Calculate pitch
    float pitch = asinf(forward.y) * RAD2DEG;
    
    // Gun base position (right side, slightly down, forward)
    Vector3 gunPos = camera.position;
    gunPos = Vector3Add(gunPos, Vector3Scale(right, 0.25f)); // Right
    gunPos = Vector3Add(gunPos, Vector3Scale(up, -0.15f + bobOffset)); // Down + bob
    gunPos = Vector3Add(gunPos, Vector3Scale(forward, 0.4f)); // Forward
    
    // Apply recoil with moderate kickback
    gunPos = Vector3Add(gunPos, Vector3Scale(up, -recoilAngle * 0.03f));    // Kick up (was 0.05f)
    gunPos = Vector3Add(gunPos, Vector3Scale(forward, -recoilAngle * 0.02f)); // Pull back (was 0.03f)
    
    // Draw simple gun body (rectangular prism)
    Color gunBody = (Color){ 40, 40, 45, 255 };
    Color gunAccent = (Color){ 0, 200, 255, 255 }; // Neon cyan
    
    // Main body - oriented along forward direction with rotation
    rlPushMatrix();
        rlTranslatef(gunPos.x, gunPos.y, gunPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        rlRotatef(sprintTilt, 0, 0, 1); // Sprint tilt (roll) - horizontal gun when running
        DrawCube((Vector3){0, 0, 0}, 0.08f, 0.08f, 0.3f, gunBody);
        DrawCubeWires((Vector3){0, 0, 0}, 0.08f, 0.08f, 0.3f, Fade(gunAccent, 0.3f));
    rlPopMatrix();
    
    // Barrel
    Vector3 barrelPos = Vector3Add(gunPos, Vector3Scale(forward, 0.2f));
    rlPushMatrix();
        rlTranslatef(barrelPos.x, barrelPos.y, barrelPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        rlRotatef(sprintTilt, 0, 0, 1); // Sprint tilt
        DrawCube((Vector3){0, 0, 0}, 0.02f, 0.02f, 0.15f, (Color){ 60, 60, 65, 255 });
    rlPopMatrix();
    
    // Magazine
    Vector3 magPos = Vector3Add(gunPos, Vector3Scale(up, -0.06f));
    rlPushMatrix();
        rlTranslatef(magPos.x, magPos.y, magPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        rlRotatef(sprintTilt, 0, 0, 1); // Sprint tilt
        DrawCube((Vector3){0, 0, 0}, 0.04f, 0.08f, 0.12f, gunBody);
    rlPopMatrix();
    
    // Neon sight
    Vector3 sightPos = Vector3Add(gunPos, Vector3Scale(up, 0.05f));
    sightPos = Vector3Add(sightPos, Vector3Scale(forward, 0.05f));
    rlPushMatrix();
        rlTranslatef(sightPos.x, sightPos.y, sightPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        rlRotatef(sprintTilt, 0, 0, 1); // Sprint tilt
        DrawCube((Vector3){0, 0, 0}, 0.01f, 0.02f, 0.01f, gunAccent);
    rlPopMatrix();
    DrawSphere(sightPos, 0.015f, Fade(gunAccent, 0.5f)); // Glow (spheres don't need rotation)
    
    // Grip
    Vector3 gripPos = Vector3Add(gunPos, Vector3Scale(up, -0.04f));
    gripPos = Vector3Add(gripPos, Vector3Scale(forward, -0.08f));
    rlPushMatrix();
        rlTranslatef(gripPos.x, gripPos.y, gripPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        DrawCube((Vector3){0, 0, 0}, 0.03f, 0.06f, 0.08f, (Color){ 30, 30, 35, 255 });
    rlPopMatrix();
    
    // Enhanced Muzzle Flash (when shooting)
    Vector3 flashPos = Vector3Add(barrelPos, Vector3Scale(forward, 0.08f));
    
    if (isRecoiling && recoilAngle > 0.5f) {
        // Flash intensity based on recoil (fades out)
        float intensity = (recoilAngle / 2.5f);
        float flashScale = 0.7f + (GetRandomValue(0, 30) / 100.0f); // Random size variation (smaller)
        
        // Bright white core (hottest part) - reduced size
        DrawSphere(flashPos, 0.04f * flashScale * intensity, (Color){ 255, 255, 255, 255 });
        
        // Yellow-orange inner glow - reduced size
        DrawSphere(flashPos, 0.07f * flashScale * intensity, (Color){ 255, 255, 100, 240 });
        DrawSphere(flashPos, 0.10f * flashScale * intensity, (Color){ 255, 200, 50, 200 });
        
        // Orange-red outer layers - reduced size
        DrawSphere(flashPos, 0.13f * flashScale * intensity, (Color){ 255, 150, 0, 160 });
        DrawSphere(flashPos, 0.16f * flashScale * intensity, (Color){ 255, 80, 0, 100 });
        
        // Randomized flash spikes/rays (star pattern) - smaller and fewer
        int numRays = 4;
        for (int i = 0; i < numRays; i++) {
            float angle = (i * 90.0f) + GetRandomValue(-10, 10); // Random angle variation
            float rayLength = 0.08f + (GetRandomValue(0, 5) / 100.0f); // Shorter rays
            
            rlPushMatrix();
                rlTranslatef(flashPos.x, flashPos.y, flashPos.z);
                rlRotatef(yaw, 0, 1, 0);
                rlRotatef(-pitch, 1, 0, 0);
                rlRotatef(angle, 0, 0, 1); // Rotate around forward axis
                
                // Draw ray as stretched cube
                Vector3 rayOffset = {rayLength * 0.5f, 0, 0};
                DrawCube(rayOffset, rayLength, 0.015f, 0.015f, 
                        (Color){ 255, 200, 100, (unsigned char)(200 * intensity) });
            rlPopMatrix();
        }
        
        // Smoke/heat distortion particles - smaller
        for (int i = 0; i < 2; i++) {
            float smokeOffset = 0.08f + (i * 0.05f);
            Vector3 smokePos = Vector3Add(flashPos, Vector3Scale(forward, smokeOffset));
            float smokeAlpha = 80.0f * (1.0f - (i * 0.4f)) * intensity;
            DrawSphere(smokePos, 0.03f + (i * 0.02f), 
                      (Color){ 80, 80, 80, (unsigned char)smokeAlpha });
        }
        
        // Bright lens flare effect - smaller
        rlPushMatrix();
            rlTranslatef(flashPos.x, flashPos.y, flashPos.z);
            rlRotatef(yaw, 0, 1, 0);
            rlRotatef(-pitch, 1, 0, 0);
            
            // Cross-shaped flare - reduced size
            DrawCube((Vector3){0, 0, 0}, 0.2f * intensity, 0.01f, 0.01f, (Color){ 255, 255, 200, 120 });
            DrawCube((Vector3){0, 0, 0}, 0.01f, 0.2f * intensity, 0.01f, (Color){ 255, 255, 200, 120 });
        rlPopMatrix();
        
    } else if (!isRecoiling) {
        // When not shooting, show a small cyan sight dot at the barrel tip
        DrawSphere(flashPos, 0.02f, (Color){ 0, 255, 255, 100 });
    }
}

void Gun::draw(Camera3D camera) {
    drawSimple(camera); // Use simple gun for now
}
