#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <cstdio>
#include <iostream>

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
    
    // Footstep audio
    currentFootstepIndex = 0;
    currentInstanceIndex = 0;
    footstepTimer = 0.0f;
    footstepInterval = 0.4f; // Time between footsteps (will adjust based on speed)
    lastHorizontalSpeed = 0.0f;
    
    // Load footstep sounds
    #if defined(PLATFORM_WEB)
        for (int i = 0; i < MAX_FOOTSTEP_SOUNDS; i++) {
            char path[256];
            snprintf(path, sizeof(path), "assets/character/audio/footsteps/Floor_step%d.wav", i);
            if (FileExists(path)) {
                footstepSounds[i] = LoadSound(path);
                SetSoundVolume(footstepSounds[i], 0.3f); // Lower volume for footsteps
            }
        }
    #else
        for (int i = 0; i < MAX_FOOTSTEP_SOUNDS; i++) {
            char path[256];
            snprintf(path, sizeof(path), "assets/character/audio/footsteps/Floor_step%d.wav", i);
            footstepSounds[i] = LoadSound(path);
            SetSoundVolume(footstepSounds[i], 0.3f);
        }
    #endif
    
    // Create sound instances for multi-channel playback
    for (int i = 0; i < MAX_FOOTSTEP_INSTANCES; i++) {
        footstepInstances[i] = LoadSoundAlias(footstepSounds[0]); // Start with first sound
    }
}

Player::~Player() {
    // Cleanup footstep sound instances
    for (int i = 0; i < MAX_FOOTSTEP_INSTANCES; i++) {
        UnloadSoundAlias(footstepInstances[i]);
    }
    // Cleanup footstep sounds
    for (int i = 0; i < MAX_FOOTSTEP_SOUNDS; i++) {
        UnloadSound(footstepSounds[i]);
    }
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
        
        // Update horizontal velocity for footstep system
        velocity.x = moveDir.x / deltaTime;
        velocity.z = moveDir.z / deltaTime;
    } else {
        // No horizontal movement
        velocity.x = 0.0f;
        velocity.z = 0.0f;
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
    
    // Update footsteps
    updateFootsteps(deltaTime);
    
    // DON'T reset isShooting here - let main.cpp handle it after effects are processed
    // isShooting = false;
}

void Player::playFootstep() {
    // Cycle through the 9 different footstep sounds
    currentFootstepIndex = (currentFootstepIndex + 1) % MAX_FOOTSTEP_SOUNDS;
    
    // Update the current instance to use the new sound
    UnloadSoundAlias(footstepInstances[currentInstanceIndex]);
    footstepInstances[currentInstanceIndex] = LoadSoundAlias(footstepSounds[currentFootstepIndex]);
    
    // Play the sound on the current instance
    PlaySound(footstepInstances[currentInstanceIndex]);
    
    // Move to next instance for multi-channel support
    currentInstanceIndex = (currentInstanceIndex + 1) % MAX_FOOTSTEP_INSTANCES;
}

void Player::updateFootsteps(float deltaTime) {
    // Calculate horizontal movement speed
    float horizontalSpeed = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    
    // Only play footsteps if moving on ground
    if (isGrounded && horizontalSpeed > 0.1f) {
        // Adjust footstep interval based on speed (faster when sprinting)
        if (isSprinting) {
            footstepInterval = 0.3f; // Faster footsteps when sprinting
        } else {
            footstepInterval = 0.45f; // Normal walking pace
        }
        
        footstepTimer += deltaTime;
        
        if (footstepTimer >= footstepInterval) {
            playFootstep();
            footstepTimer = 0.0f;
        }
    } else {
        // Reset timer when not moving
        footstepTimer = 0.0f;
    }
    
    lastHorizontalSpeed = horizontalSpeed;
}
