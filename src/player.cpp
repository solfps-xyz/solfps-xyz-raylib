#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include <cmath>

Player::Player() {
    camera.position = (Vector3){ 0.0f, 2.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    yaw = -90.0f;
    pitch = 0.0f;
    moveSpeed = 5.0f;
    sprintSpeed = 8.0f;
    mouseSensitivity = 0.003f;
    height = 1.8f;
    
    velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
    isGrounded = false;
    isSprinting = false;
    
    // Gun state
    isShooting = false;
    shootCooldown = 0.0f;
    ammo = 30;
    maxAmmo = 30;
    recoilOffset = 0.0f;
}

void Player::handleMouseLook() {
    Vector2 mouseDelta = GetMouseDelta();
    yaw += mouseDelta.x * mouseSensitivity;
    pitch -= mouseDelta.y * mouseSensitivity;
    
    // Clamp pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

Vector3 Player::getForward() {
    Vector3 direction;
    direction.x = cosf(yaw) * cosf(pitch);
    direction.y = sinf(pitch);
    direction.z = sinf(yaw) * cosf(pitch);
    return Vector3Normalize(direction);
}

Vector3 Player::getRight() {
    Vector3 forward = getForward();
    Vector3 flatForward = { forward.x, 0.0f, forward.z };
    flatForward = Vector3Normalize(flatForward);
    return Vector3Normalize(Vector3CrossProduct(flatForward, camera.up));
}

void Player::handleInput(float deltaTime) {
    Vector3 forward = getForward();
    Vector3 flatForward = { forward.x, 0.0f, forward.z };
    flatForward = Vector3Normalize(flatForward);
    Vector3 right = getRight();
    
    // Sprint
    isSprinting = IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_W);
    float currentSpeed = isSprinting ? sprintSpeed : moveSpeed;
    
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
    
    // WASD movement
    if (IsKeyDown(KEY_W)) {
        moveDir = Vector3Add(moveDir, flatForward);
    }
    if (IsKeyDown(KEY_S)) {
        moveDir = Vector3Subtract(moveDir, flatForward);
    }
    if (IsKeyDown(KEY_A)) {
        moveDir = Vector3Subtract(moveDir, right);
    }
    if (IsKeyDown(KEY_D)) {
        moveDir = Vector3Add(moveDir, right);
    }
    
    // Normalize and apply speed
    if (Vector3Length(moveDir) > 0) {
        moveDir = Vector3Normalize(moveDir);
        moveDir = Vector3Scale(moveDir, currentSpeed * deltaTime);
        camera.position = Vector3Add(camera.position, moveDir);
    }
    
    // Jump
    if (IsKeyPressed(KEY_SPACE) && isGrounded) {
        velocity.y = 8.0f;
        isGrounded = false;
    }
    
    // Shooting
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && shootCooldown <= 0.0f && ammo > 0) {
        shoot();
    }
    
    // Reload
    if (IsKeyPressed(KEY_R) && ammo < maxAmmo) {
        reload();
    }
}

void Player::applyGravity(float deltaTime) {
    if (!isGrounded) {
        velocity.y -= 20.0f * deltaTime; // Gravity
    }
    
    camera.position.y += velocity.y * deltaTime;
    
    // Simple ground check
    if (camera.position.y <= height) {
        camera.position.y = height;
        velocity.y = 0.0f;
        isGrounded = true;
    }
}

void Player::shoot() {
    isShooting = true;
    ammo--;
    shootCooldown = 0.1f; // 600 RPM
    recoilOffset = 0.5f;
}

void Player::reload() {
    ammo = maxAmmo;
}

void Player::update(float deltaTime) {
    // Update cooldowns
    if (shootCooldown > 0.0f) {
        shootCooldown -= deltaTime;
    }
    
    // Recoil recovery
    if (recoilOffset > 0.0f) {
        recoilOffset -= deltaTime * 2.0f;
        if (recoilOffset < 0.0f) recoilOffset = 0.0f;
    }
    
    handleMouseLook();
    handleInput(deltaTime);
    applyGravity(deltaTime);
    
    // Update camera target
    Vector3 direction = getForward();
    camera.target = Vector3Add(camera.position, direction);
    
    isShooting = false;
}
