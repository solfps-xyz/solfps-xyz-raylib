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
}

Gun::~Gun() {
    // Cleanup if model is loaded
}

void Gun::update(float deltaTime, bool isMoving, bool isShooting) {
    // Debug output
    if (isShooting) {
        std::cout << "DEBUG Gun::update: isShooting = TRUE" << std::endl;
    }
    
    // Weapon bob while moving
    if (isMoving) {
        bobSpeed += deltaTime * 10.0f;
        bobOffset = sinf(bobSpeed) * 0.02f;
    } else {
        bobSpeed = 0.0f;
        bobOffset *= 0.9f; // Smooth return
    }
    
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
    std::cout << "Gun recoil applied! recoilAngle: " << recoilAngle << ", isRecoiling: " << isRecoiling << std::endl;
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
        DrawCube((Vector3){0, 0, 0}, 0.08f, 0.08f, 0.3f, gunBody);
        DrawCubeWires((Vector3){0, 0, 0}, 0.08f, 0.08f, 0.3f, Fade(gunAccent, 0.3f));
    rlPopMatrix();
    
    // Barrel
    Vector3 barrelPos = Vector3Add(gunPos, Vector3Scale(forward, 0.2f));
    rlPushMatrix();
        rlTranslatef(barrelPos.x, barrelPos.y, barrelPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        DrawCube((Vector3){0, 0, 0}, 0.02f, 0.02f, 0.15f, (Color){ 60, 60, 65, 255 });
    rlPopMatrix();
    
    // Magazine
    Vector3 magPos = Vector3Add(gunPos, Vector3Scale(up, -0.06f));
    rlPushMatrix();
        rlTranslatef(magPos.x, magPos.y, magPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
        DrawCube((Vector3){0, 0, 0}, 0.04f, 0.08f, 0.12f, gunBody);
    rlPopMatrix();
    
    // Neon sight
    Vector3 sightPos = Vector3Add(gunPos, Vector3Scale(up, 0.05f));
    sightPos = Vector3Add(sightPos, Vector3Scale(forward, 0.05f));
    rlPushMatrix();
        rlTranslatef(sightPos.x, sightPos.y, sightPos.z);
        rlRotatef(yaw, 0, 1, 0);
        rlRotatef(-pitch, 1, 0, 0);
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
    
    // Muzzle flash (when shooting)
    // Always show a dim barrel tip for reference
    Vector3 flashPos = Vector3Add(barrelPos, Vector3Scale(forward, 0.1f));
    
    if (isRecoiling) { // Simplified - show whenever recoiling
        // Multiple layers for a more visible flash (spheres don't need rotation)
        DrawSphere(flashPos, 0.15f, (Color){ 255, 255, 0, 255 });
        DrawSphere(flashPos, 0.2f, (Color){ 255, 150, 0, 220 });
        DrawSphere(flashPos, 0.25f, (Color){ 255, 100, 0, 150 });
        
        // Flash cone
        rlPushMatrix();
            rlTranslatef(flashPos.x, flashPos.y, flashPos.z);
            rlRotatef(yaw, 0, 1, 0);
            rlRotatef(-pitch, 1, 0, 0);
            DrawCube((Vector3){0, 0, 0}, 0.15f, 0.15f, 0.25f, (Color){ 255, 200, 0, 240 });
            DrawCubeWires((Vector3){0, 0, 0}, 0.2f, 0.2f, 0.3f, (Color){ 255, 255, 255, 220 });
        rlPopMatrix();
    } else {
        // When not shooting, show a small cyan sight dot at the barrel tip
        DrawSphere(flashPos, 0.02f, (Color){ 0, 255, 255, 100 });
    }
}

void Gun::draw(Camera3D camera) {
    drawSimple(camera); // Use simple gun for now
}
