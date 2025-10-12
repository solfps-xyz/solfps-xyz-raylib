#include "gun.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <cmath>

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
        recoilAngle -= deltaTime * 15.0f; // Recoil recovery
        if (recoilAngle <= 0.0f) {
            recoilAngle = 0.0f;
            isRecoiling = false;
        }
    }
}

void Gun::applyRecoil() {
    recoilAngle = 3.0f; // Kick up angle
    isRecoiling = true;
}

void Gun::drawSimple(Camera3D camera) {
    // Disable depth test for weapon draw
    rlDisableDepthTest();
    
    // Calculate gun position relative to camera
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));
    
    // Gun base position (right side, slightly down, forward)
    Vector3 gunPos = camera.position;
    gunPos = Vector3Add(gunPos, Vector3Scale(right, 0.25f)); // Right
    gunPos = Vector3Add(gunPos, Vector3Scale(up, -0.15f + bobOffset)); // Down + bob
    gunPos = Vector3Add(gunPos, Vector3Scale(forward, 0.4f)); // Forward
    
    // Apply recoil
    gunPos = Vector3Add(gunPos, Vector3Scale(up, -recoilAngle * 0.02f));
    gunPos = Vector3Add(gunPos, Vector3Scale(forward, -recoilAngle * 0.01f));
    
    // Draw simple gun body (rectangular prism)
    Color gunBody = (Color){ 40, 40, 45, 255 };
    Color gunAccent = (Color){ 0, 200, 255, 255 }; // Neon cyan
    
    // Main body
    DrawCube(gunPos, 0.08f, 0.08f, 0.3f, gunBody);
    
    // Barrel
    Vector3 barrelPos = Vector3Add(gunPos, Vector3Scale(forward, 0.2f));
    DrawCube(barrelPos, 0.02f, 0.02f, 0.15f, (Color){ 60, 60, 65, 255 });
    
    // Magazine
    Vector3 magPos = Vector3Add(gunPos, Vector3Scale(up, -0.06f));
    DrawCube(magPos, 0.04f, 0.08f, 0.12f, gunBody);
    
    // Neon sight
    Vector3 sightPos = Vector3Add(gunPos, Vector3Scale(up, 0.05f));
    sightPos = Vector3Add(sightPos, Vector3Scale(forward, 0.05f));
    DrawCube(sightPos, 0.01f, 0.02f, 0.01f, gunAccent);
    
    // Grip
    Vector3 gripPos = Vector3Add(gunPos, Vector3Scale(up, -0.04f));
    gripPos = Vector3Add(gripPos, Vector3Scale(forward, -0.08f));
    DrawCube(gripPos, 0.03f, 0.06f, 0.08f, (Color){ 30, 30, 35, 255 });
    
    // Muzzle flash (when shooting)
    if (isRecoiling && recoilAngle > 2.5f) {
        Vector3 flashPos = Vector3Add(barrelPos, Vector3Scale(forward, 0.1f));
        DrawCube(flashPos, 0.05f, 0.05f, 0.1f, (Color){ 255, 200, 0, 200 });
        DrawCubeWires(flashPos, 0.08f, 0.08f, 0.15f, (Color){ 255, 150, 0, 150 });
    }
    
    rlEnableDepthTest();
}

void Gun::draw(Camera3D camera) {
    drawSimple(camera); // Use simple gun for now
}
