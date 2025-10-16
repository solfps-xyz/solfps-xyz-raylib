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
    bool isCrouching;
    float crouchSpeed;
    float crouchHeight;
    float standingHeight;
    float currentHeight; // Smoothly interpolated height
    
    // Health system
    float health;
    float maxHealth;
    float lastDamageTime;
    float damageFlashTimer;
    
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
    Sound footstepInstances[MAX_FOOTSTEP_SOUNDS][MAX_FOOTSTEP_INSTANCES]; // Pre-load all instances
    int currentFootstepIndex;
    int currentInstanceIndex;
    float footstepTimer;
    float footstepInterval;
    float lastHorizontalSpeed;
    
    Player();
    ~Player();
    void update(float deltaTime);
    void handleInput(float deltaTime);
    void handleMobileInput(float deltaTime, Vector2 moveVector, bool sprint, bool jump, bool crouch, bool shoot, bool reload);
    void handleMouseLook();
    void handleMobileLook(Vector2 lookDelta);
    void applyGravity(float deltaTime);
    void shoot();
    void reload();
    void playFootstep();
    void updateFootsteps(float deltaTime);
    void takeDamage(float damage);
    void regenerateHealth(float deltaTime);
    Vector3 getForward();
    Vector3 getRight();
    float getForwardSpeed(); // Get speed in forward direction only
};

#endif // PLAYER_H
