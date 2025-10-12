#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <raymath.h>

class Player {
public:
    Camera3D camera;
    float yaw;
    float pitch;
    float moveSpeed;
    float sprintSpeed;
    float mouseSensitivity;
    float height;
    Vector3 velocity;
    float forwardVelocity; // Track forward movement speed separately
    bool isGrounded;
    bool isSprinting;
    
    // Gun state
    bool isShooting;
    float shootCooldown;
    int ammo;
    int maxAmmo;
    float recoilOffset;
    
    // Footstep audio
    static const int MAX_FOOTSTEP_SOUNDS = 9;
    static const int MAX_FOOTSTEP_INSTANCES = 4; // Multi-channel support
    Sound footstepSounds[MAX_FOOTSTEP_SOUNDS];
    Sound footstepInstances[MAX_FOOTSTEP_INSTANCES];
    int currentFootstepIndex;
    int currentInstanceIndex;
    float footstepTimer;
    float footstepInterval;
    float lastHorizontalSpeed;
    
    Player();
    ~Player();
    void update(float deltaTime);
    void handleInput(float deltaTime);
    void handleMouseLook();
    void applyGravity(float deltaTime);
    void shoot();
    void reload();
    void playFootstep();
    void updateFootsteps(float deltaTime);
    Vector3 getForward();
    Vector3 getRight();
    float getForwardSpeed(); // Get speed in forward direction only
};

#endif // PLAYER_H
